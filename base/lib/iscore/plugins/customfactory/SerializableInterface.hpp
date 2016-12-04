#pragma once
#include <iscore/plugins/customfactory/UuidKey.hpp>

#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>


/**
 * @brief Generic serialization method for abstract classes.
 *
 * This class and AbstractSerializer<JSONObject, T> are used
 * for serializing classes that are part of a polymorphic hierarchy.
 *
 * The problem is reloading polymorphic classes : we have to save
 * the identifier of the factory used to instantiate them.
 *
 * Base classes should provide only serialization code for their own data
 * in Visitor<Reader<...>> and Visitor<Writer<...>>.
 *
 * Likewise, subclasses should only save their own data.
 * These classes ensure that everything will be saved in the correct order.
 *
 */
template <typename T>
struct AbstractSerializer<DataStream, T>
{
  static void readFrom(DataStream::Serializer& s, const T& obj)
  {
    // We save in a byte array so that
    // we have a chance to save it as-is and reload it later
    // if the plug-in is not found on the system.
    QByteArray b;
    DataStream::Serializer sub{&b};

    sub.readFrom(obj.concreteFactoryKey().impl());
    sub.readFrom_impl(obj);
    obj.serialize_impl(sub.toVariant());
    sub.insertDelimiter();

    s.stream() << std::move(b);
  }
};

template <typename T>
struct AbstractSerializer<JSONObject, T>
{
  static void readFrom(JSONObject::Serializer& s, const T& obj)
  {
    s.m_obj[s.strings.uuid] = toJsonValue(obj.concreteFactoryKey().impl());
    s.readFrom_impl(obj);
    obj.serialize_impl(s.toVariant());
  }
};

namespace iscore
{
// FIXME why is this not used everywhere
struct concrete
{
  using is_concrete_tag = std::integral_constant<bool, true>;
};

/**
 * @brief Provides a common serialization mechanism to abstract types.
 */
template <typename T>
class SerializableInterface
{
public:
  using key_type = UuidKey<T>;
  using is_abstract_base_tag = std::integral_constant<bool, true>;

  SerializableInterface() = default;
  virtual ~SerializableInterface() = default;
  virtual UuidKey<T> concreteFactoryKey() const = 0;

  virtual void serialize_impl(const VisitorVariant& vis) const
  {
  }
};
}

template <typename Type>
Type deserialize_key(Deserializer<JSONObject>& des)
{
  return fromJsonValue<iscore::uuid_t>(des.m_obj[des.strings.uuid]);
}

template <typename Type>
Type deserialize_key(Deserializer<DataStream>& des)
{
  iscore::uuid_t uid;
  des.writeTo(uid);
  return uid;
}
/**
 * @brief deserialize_interface Reload a polymorphic type
 * @param factories The list of factories where the correct factory is.
 * @param des The deserializer instance
 * @param args Used to provide additional arguments for the factory "load" method (for instance, a context).
 *
 * @return An instance of the object if a factory was found. Else, if there is one available, the "missing factory" element.
 * There is no guarantee that the return value points to a valid object, it should always be checked.
 */
template <typename FactoryList_T, typename... Args>
auto deserialize_interface(
    const FactoryList_T& factories,
    DataStream::Deserializer& des,
    Args&&... args) -> typename FactoryList_T::object_type*
{
  QByteArray b;
  des.stream() >> b;
  DataStream::Deserializer sub{b};

  // Deserialize the interface identifier
  try
  {
    auto k = deserialize_key<
        typename FactoryList_T::factory_type::ConcreteFactoryKey>(sub);

    // Get the factory
    if (auto concrete_factory = factories.get(k))
    {
      // Create the object
      auto obj = concrete_factory->load(
          sub.toVariant(), std::forward<Args>(args)...);

      sub.checkDelimiter();

      return obj;
    }
  }
  catch (...)
  {
  }

  // If the object could not be loaded, we try to load a "missing" verson of
  // it.
  return factories.loadMissing(sub.toVariant(), std::forward<Args>(args)...);
}

template <typename FactoryList_T, typename... Args>
auto deserialize_interface(
    const FactoryList_T& factories,
    JSONObject::Deserializer& des,
    Args&&... args) -> typename FactoryList_T::object_type*
{
  // Deserialize the interface identifier
  try
  {
    auto k = deserialize_key<
        typename FactoryList_T::factory_type::ConcreteFactoryKey>(des);

    // Get the factory
    if (auto concrete_factory = factories.get(k))
    {
      // Create the object
      return concrete_factory->load(
          des.toVariant(), std::forward<Args>(args)...);
    }
  }
  catch (...)
  {
  }

  // If the object could not be loaded, we try to load a "missing" verson of
  // it.
  return factories.loadMissing(des.toVariant(), std::forward<Args>(args)...);
}

/**
 * @macro SERIALIZABLE_MODEL_METADATA_IMPL Provides default implementations of methods of SerializableInterface.
 *
 * If the class is clonable, use MODEL_METADATA_IMPL.
 */
#define SERIALIZABLE_MODEL_METADATA_IMPL(Model_T)                     \
  static key_type static_concreteFactoryKey()                         \
  {                                                                   \
    return Metadata<ConcreteFactoryKey_k, Model_T>::get();            \
  }                                                                   \
  key_type concreteFactoryKey() const final override                  \
  {                                                                   \
    return static_concreteFactoryKey();                               \
  }                                                                   \
                                                                      \
  void serialize_impl(const VisitorVariant& vis) const final override \
  {                                                                   \
    serialize_dyn(vis, *this);                                        \
  }

/**
 * @macro MODEL_METADATA_IMPL Provides default implementations of methods of SerializableInterface.
 *
 * Should be used most of the time when deriving from an abstract model class.
 */
#define MODEL_METADATA_IMPL(Model_T)                       \
  SERIALIZABLE_MODEL_METADATA_IMPL(Model_T)                \
  Model_T* clone(const id_type& newId, QObject* newParent) \
      const final override                                 \
  {                                                        \
    return new Model_T{*this, newId, newParent};           \
  }

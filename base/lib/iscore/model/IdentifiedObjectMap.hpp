#pragma once
#include <iscore/model/IdentifiedObject.hpp>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index_container.hpp>

#include <boost/iterator/indirect_iterator.hpp>
// This file contains a fast map for items based on their identifier,
// based on boost's multi-index maps.

namespace bmi = boost::multi_index;

/**
 * @brief A map to access child objects through their id.
 */
template <class Element, class Model = Element, class Enable = void>
class IdContainer
{
};

template <typename Element, typename Model, typename Map>
/**
 * @brief The MapBase class
 *
 * A generic map type, which provides reference-like access to the stored
 * pointers.
 */
class MapBase
{
public:
  using value_type = Element;
  using model_type = Model;

  MapBase() = default;
  MapBase(const MapBase& other) = delete;
  /*
  template<typename T>
  MapBase(const T& container)
  {
      for(auto& element : container)
      {
          insert(&element);
      }
  }
  */

  ~MapBase()
  {
    // To ensure that children are deleted before their parents
    for (auto elt : this->get())
    {
      delete elt;
    }
  }

  void insert(value_type* t)
  {
    ISCORE_ASSERT(m_map.find(t->id()) == m_map.end());
    m_map.insert(t);
  }

  std::size_t size() const
  {
    return m_map.size();
  }

  bool empty() const
  {
    return m_map.empty();
  }

  template <typename T>
  void remove(const T& t)
  {
    m_map.erase(t);
  }

  void clear()
  {
    m_map.clear();
  }

  auto& get()
  {
    return m_map.template get<0>();
  }
  const auto& get() const
  {
    return m_map.template get<0>();
  }

protected:
  Map m_map;
};

// We have to write two implementations since const_mem_fun does not handle
// inheritance.

/** This map is for classes which inherit from
 * IdentifiedObject<T> and don't have an id() method by themselves, e.g. all the model objects.
 *
 * Additionnally, items are ordered; iteration occurs on the ordered iterators.
 *
 * In the implementation :
 * * `get<0>()` gets the hashed (like std::unordered_map) iterator.
 * * `get<1>()` gets the ordered (like std::vector) iterator.
 */
template <typename Element, typename Model>
class
    IdContainer<Element, Model, std::enable_if_t<std::is_base_of<IdentifiedObject<Model>, Element>::value>>
    : public MapBase<Element, Model, bmi::multi_index_container<Element*, bmi::indexed_by<bmi::hashed_unique<bmi::const_mem_fun<IdentifiedObject<Model>, const Id<Model>&, &IdentifiedObject<Model>::id>>, bmi::sequenced<>>>>
{
public:
  using MapBase<Element, Model, bmi::multi_index_container<Element*, bmi::indexed_by<bmi::hashed_unique<bmi::const_mem_fun<IdentifiedObject<Model>, const Id<Model>&, &IdentifiedObject<Model>::id>>, bmi::sequenced<>>>>::
      MapBase;

  auto& ordered() { return this->m_map.template get<1>(); }

  auto begin() const
  {
    return boost::make_indirect_iterator(
        this->m_map.template get<1>().begin());
  }
  auto cbegin() const
  {
    return boost::make_indirect_iterator(
        this->m_map.template get<1>().cbegin());
  }
  auto end() const
  {
    return boost::make_indirect_iterator(this->m_map.template get<1>().end());
  }
  auto cend() const
  {
    return boost::make_indirect_iterator(this->m_map.template get<1>().cend());
  }

  auto find(const Id<Model>& id) const
  {
    auto it = this->m_map.find(id);

    auto p_it = this->m_map.template project<1>(it);
    return boost::make_indirect_iterator(p_it);
  }

  Element& at(const Id<Model>& id) const
  {
    if (id.m_ptr)
    {
      ISCORE_ASSERT(id.m_ptr->parent() == (*this->m_map.find(id))->parent());
      return safe_cast<Element&>(*id.m_ptr);
    }
    auto item = this->m_map.find(id);
    ISCORE_ASSERT(item != this->m_map.end());

    id.m_ptr = *item;
    return safe_cast<Element&>(**item);
  }

  void swap(const Id<Model>& t1, const Id<Model>& t2)
  {
    if (t1 == t2)
      return;

    auto& map = this->m_map;
    auto& hash = map.template get<0>();
    auto& seq = map.template get<1>();

    // 1. Find elements
    auto pos1 = hash.find(t1);
    ISCORE_ASSERT(pos1 != hash.end());
    auto pos2 = hash.find(t2);
    ISCORE_ASSERT(pos2 != hash.end());

    auto p1 = map.template project<1>(pos1);
    auto p2 = map.template project<1>(pos2);

    // 2. Find the furthest element
    auto beg = seq.begin();
    const std::size_t dist_p1 = std::distance(beg, p1);
    const std::size_t dist_p2 = std::distance(beg, p2);

    auto dist_last = std::max(dist_p1, dist_p2);

    auto first = dist_p1 < dist_p2 ? p1 : p2;
    auto last = dist_p1 < dist_p2 ? p2 : p1;

    // 3. Perform swapping
    seq.relocate(first, last);
    if (dist_last + 1 < hash.size())
    {
      std::advance(beg, dist_last + 1);
    }
    else
    {
      beg = seq.end();
    }

    seq.relocate(beg, first);
  }

  void relocate(const Id<Model>& t1, const Id<Model>& t2)
  {
    if (t1 == t2)
      return;

    auto& map = this->m_map;
    auto& hash = map.template get<0>();
    auto& seq = map.template get<1>();

    // 1. Find elements
    auto pos1 = hash.find(t1);
    ISCORE_ASSERT(pos1 != hash.end());
    auto pos2 = hash.find(t2);
    ISCORE_ASSERT(pos2 != hash.end());

    auto p1 = map.template project<1>(pos1);
    auto p2 = map.template project<1>(pos2);

    seq.relocate(p1, p2);
  }

  void putToEnd(const Id<Model>& t1)
  {
    auto& map = this->m_map;
    auto& hash = map.template get<0>();
    auto& seq = map.template get<1>();

    // 1. Find elements
    auto pos1 = hash.find(t1);
    ISCORE_ASSERT(pos1 != hash.end());

    auto p1 = map.template project<1>(pos1);

    seq.relocate(seq.end(), p1);
  }
};

/** This specialization is for classes which directly have an id() method
 * like a Presenter whose id() would return its model's.
 */
template <typename Element, typename Model>
class
    IdContainer<Element, Model, std::enable_if_t<!std::is_base_of<IdentifiedObject<Model>, Element>::value>>
    : public MapBase<Element, Model, bmi::multi_index_container<Element*, bmi::indexed_by<bmi::hashed_unique<bmi::const_mem_fun<Element, const Id<Model>&, &Element::id>>>>>
{
public:
  using MapBase<Element, Model, bmi::multi_index_container<Element*, bmi::indexed_by<bmi::hashed_unique<bmi::const_mem_fun<Element, const Id<Model>&, &Element::id>>>>>::
      MapBase;

  auto begin() const
  {
    return boost::make_indirect_iterator(this->m_map.begin());
  }
  auto cbegin() const
  {
    return boost::make_indirect_iterator(this->m_map.cbegin());
  }
  auto end() const
  {
    return boost::make_indirect_iterator(this->m_map.end());
  }
  auto cend() const
  {
    return boost::make_indirect_iterator(this->m_map.cend());
  }

  auto find(const Id<Model>& id) const
  {
    return boost::make_indirect_iterator(this->m_map.find(id));
  }

  auto& at(const Id<Model>& id) const
  {
    auto item = this->m_map.find(id);
    ISCORE_ASSERT(item != this->m_map.end());
    return **item;
  }
};

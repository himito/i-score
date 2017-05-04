#pragma once
#include <ossia/detail/destination_index.hpp>
#include <QHash>
#include <QList>
#include <QMetaObject>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <boost/functional/hash.hpp>
#include <cstddef>
#include <iscore/tools/std/Optional.hpp>
#include <iscore_lib_state_export.h>
#include <memory>
#include <string>
#include <iscore/tools/std/HashMap.hpp>

namespace ossia
{
struct destination_qualifiers;
struct unit_t;
}

namespace State
{
struct ISCORE_LIB_STATE_EXPORT DestinationQualifiers
{
  Q_GADGET
public:
  DestinationQualifiers();
  DestinationQualifiers(const DestinationQualifiers& other);
  DestinationQualifiers(DestinationQualifiers&& other);
  DestinationQualifiers& operator=(const DestinationQualifiers& other);
  DestinationQualifiers& operator=(DestinationQualifiers&& other);
  ~DestinationQualifiers();

  DestinationQualifiers(const ossia::destination_qualifiers&);
  DestinationQualifiers& operator=(const ossia::destination_qualifiers&);

  operator const ossia::destination_qualifiers&() const;
  operator ossia::destination_qualifiers&();

  bool operator==(const State::DestinationQualifiers& other) const;
  bool operator!=(const State::DestinationQualifiers& other) const;

  const ossia::destination_qualifiers& get() const;
  ossia::destination_qualifiers& get();

private:
  std::unique_ptr<ossia::destination_qualifiers> qualifiers;
};
/**
 * @brief The Address struct
 *
 * Represents an address in the style of Jamoma :
 *
 *  aDevice:/aNode/anotherNode
 *
 */
struct ISCORE_LIB_STATE_EXPORT Address
{
  Address() noexcept;
  Address(const Address& other) noexcept;
  Address(Address&&) noexcept;
  Address& operator=(const Address& other) noexcept;
  Address& operator=(Address&& other) noexcept;
  Address(QString d, QStringList p) noexcept;

  // Data
  QString device; // No device means that this is the invisible root node.

  QStringList path; // Note : path is empty if address is root: "device:/"
  // In terms of Device::Node, this means that the node is the device node.

  // Check that the given string is a valid address
  // Note: a "maybe" concept would help here.
  static bool validateString(const QString& str);
  static bool validateFragment(const QString& s);

  // Make an address from a valid address string
  static optional<Address> fromString(const QString& str); // TODO return optional

  /**
   * @brief toString
   * @return aDevice:/and/path if valid, else an empty string.
   */
  QString toString() const;

  /**
   * @brief toShortString
   * @return If short, "dev:/foo", else the last fifteen chars.
   */
  QString toShortString() const;

  bool operator==(const Address& a) const;
  bool operator!=(const Address& a) const;
};

using AccessorVector = ossia::destination_index;
ISCORE_LIB_STATE_EXPORT
QString toString(const ossia::destination_qualifiers& d);

struct ISCORE_LIB_STATE_EXPORT AddressAccessor
{
  AddressAccessor() noexcept;
  AddressAccessor(const AddressAccessor& other) noexcept;
  AddressAccessor(AddressAccessor&& other) noexcept;
  AddressAccessor& operator=(const AddressAccessor& other) noexcept;
  AddressAccessor& operator=(AddressAccessor&& other) noexcept;

  explicit AddressAccessor(State::Address a) noexcept;
  AddressAccessor(State::Address a, const AccessorVector& v) noexcept;
  AddressAccessor(
      State::Address a, const AccessorVector& v,
      const ossia::unit_t&) noexcept;

  AddressAccessor& operator=(const Address& a);
  AddressAccessor& operator=(Address&& a);

  State::Address address;
  DestinationQualifiers qualifiers;

  // Utility
  QString toString() const;
  QString toShortString() const;

  static optional<AddressAccessor> fromString(const QString& str);
  bool operator==(const AddressAccessor& other) const;
  bool operator!=(const AddressAccessor& a) const;
};

/**
 * @brief The AddressAccessorHead struct
 *
 * The head of an address : just the last aprt, e.g. "baz" in "foo:/bar/baz"
 * but with potential qualifiers
 */
struct ISCORE_LIB_STATE_EXPORT AddressAccessorHead
{
  QString name;
  DestinationQualifiers qualifiers;

  QString toString() const;
};

ISCORE_LIB_STATE_EXPORT QDebug operator<<(QDebug d, const State::Address& a);
ISCORE_LIB_STATE_EXPORT QDebug
operator<<(QDebug d, const DestinationQualifiers& a);
ISCORE_LIB_STATE_EXPORT QDebug
operator<<(QDebug d, const State::AccessorVector& a);
ISCORE_LIB_STATE_EXPORT QDebug
operator<<(QDebug d, const State::AddressAccessorHead& a);
ISCORE_LIB_STATE_EXPORT QDebug
operator<<(QDebug d, const State::AddressAccessor& a);
ISCORE_LIB_STATE_EXPORT QStringList stringList(const State::Address& addr);
}

namespace std
{
template <>
struct ISCORE_LIB_STATE_EXPORT hash<State::Address>
{
  std::size_t operator()(const State::Address& k) const;
};

template <>
struct ISCORE_LIB_STATE_EXPORT hash<State::AddressAccessor>
{
  std::size_t operator()(const State::AddressAccessor& k) const;
};
}

namespace boost
{
template <>
struct ISCORE_LIB_STATE_EXPORT hash<State::Address>
{
  std::size_t operator()(const State::Address& k) const;
};
}

Q_DECLARE_METATYPE(State::DestinationQualifiers)
Q_DECLARE_METATYPE(State::Address)
Q_DECLARE_METATYPE(State::AddressAccessor)

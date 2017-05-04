#include <algorithm>

#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/state/destination_qualifiers.hpp>
#include <ossia/network/base/name_validation.hpp>
#include <QStringBuilder>
#include <State/Address.hpp>
#include <State/Expression.hpp>
#include <iscore/tools/Todo.hpp>
namespace State
{

DestinationQualifiers::DestinationQualifiers()
    : qualifiers{std::make_unique<ossia::destination_qualifiers>()}
{
}

DestinationQualifiers::DestinationQualifiers(
    const DestinationQualifiers& other)
    : qualifiers{
          std::make_unique<ossia::destination_qualifiers>(*other.qualifiers)}
{
}

DestinationQualifiers::DestinationQualifiers(DestinationQualifiers&& other)
    : qualifiers{std::move(other.qualifiers)}
{
  other.qualifiers = std::make_unique<ossia::destination_qualifiers>();
}

DestinationQualifiers& DestinationQualifiers::
operator=(const DestinationQualifiers& other)
{
  *qualifiers = *other.qualifiers;
  return *this;
}

DestinationQualifiers& DestinationQualifiers::
operator=(DestinationQualifiers&& other)
{
  *qualifiers = std::move(*other.qualifiers);
  return *this;
}

DestinationQualifiers::~DestinationQualifiers()
{
}

DestinationQualifiers::DestinationQualifiers(
    const ossia::destination_qualifiers& other)
    : qualifiers{std::make_unique<ossia::destination_qualifiers>(other)}
{
}

DestinationQualifiers& DestinationQualifiers::
operator=(const ossia::destination_qualifiers& other)
{
  *qualifiers = other;
  return *this;
}

bool DestinationQualifiers::
operator==(const DestinationQualifiers& other) const
{
  return *qualifiers == *other.qualifiers;
}

bool DestinationQualifiers::
operator!=(const DestinationQualifiers& other) const
{
  return *qualifiers != *other.qualifiers;
}

const ossia::destination_qualifiers& DestinationQualifiers::get() const
{
  return *qualifiers;
}

ossia::destination_qualifiers& DestinationQualifiers::get()
{
  return *qualifiers;
}

DestinationQualifiers::operator const ossia::destination_qualifiers&() const
{
  return *qualifiers;
}

DestinationQualifiers::operator ossia::destination_qualifiers&()
{
  return *qualifiers;
}

Address::Address() noexcept = default;
Address::Address(Address&&) noexcept = default;

Address::Address(const Address& other) noexcept
    : device{other.device}, path{other.path}
{
}

Address& Address::operator=(const Address& other) noexcept
{
  device = other.device;
  path = other.path;
  return *this;
}

Address& Address::operator=(Address&& other) noexcept
{
  device = std::move(other.device);
  path = std::move(other.path);
  return *this;
}

Address::Address(QString d, QStringList p) noexcept
    : device{std::move(d)}, path{std::move(p)}
{
}

bool Address::validateString(const QString& str)
{
  auto firstcolon = str.indexOf(":");
  auto firstslash = str.indexOf("/");

  bool valid = str == QString(str.toUtf8()) && (firstcolon > 0)
               && (firstslash == (firstcolon + 1) && !str.contains("//"));

  QStringList path = str.split("/");
  valid &= !path.empty();

  path.first().remove(":");

  Foreach(path, [&](const auto& fragment) {
    valid &= validateFragment(fragment);
  });

  return valid;
}

bool Address::validateFragment(const QString& s)
{
  return ossia::all_of(s, &ossia::net::is_valid_character_for_name<QChar>);
}

optional<Address> Address::fromString(const QString& str)
{
  if (!validateString(str))
    return {};

  QStringList path = str.split("/");
  ISCORE_ASSERT(path.size() > 0);

  auto device = path.first().remove(":");
  path.removeFirst();         // Remove the device.
  if (path.first().isEmpty()) // case "device:/"
  {
    return State::Address{device, {}};
  }

  return State::Address{device, path};
}

QString Address::toString() const
{
  QString ad = device % ":/" % path.join("/");
  if (!validateString(ad))
    ad.clear();

  return ad;
}

QString Address::toShortString() const
{
  auto str = toString();
  return str.size() < 15 ? str : "..." % str.right(12);
}

bool Address::operator==(const Address& a) const
{
  return device == a.device && path == a.path;
}

bool Address::operator!=(const Address& a) const
{
  return !(*this == a);
}

AddressAccessor::AddressAccessor() noexcept
{
}

AddressAccessor::AddressAccessor(const AddressAccessor& other) noexcept
    : address{other.address}, qualifiers{other.qualifiers}
{
}

AddressAccessor::AddressAccessor(AddressAccessor&& other) noexcept
    : address{std::move(other.address)}
    , qualifiers{std::move(other.qualifiers)}
{
}

AddressAccessor& AddressAccessor::
operator=(const AddressAccessor& other) noexcept
{
  address = other.address;
  qualifiers = other.qualifiers;
  return *this;
}

AddressAccessor& AddressAccessor::operator=(AddressAccessor&& other) noexcept
{
  address = std::move(other.address);
  qualifiers = std::move(other.qualifiers);
  return *this;
}

AddressAccessor::AddressAccessor(Address a) noexcept : address{std::move(a)}
{
}

AddressAccessor::AddressAccessor(Address a, const AccessorVector& v) noexcept
    : address{std::move(a)}, qualifiers{ossia::destination_qualifiers{v, {}}}
{
}

AddressAccessor::AddressAccessor(
    Address a, const AccessorVector& v, const ossia::unit_t& u) noexcept
    : address{std::move(a)}, qualifiers{ossia::destination_qualifiers{v, u}}
{
}

AddressAccessor& AddressAccessor::operator=(const Address& a)
{
  address = a;
  qualifiers.get().accessors.clear();
  return *this;
}

AddressAccessor& AddressAccessor::operator=(Address&& a)
{
  address = std::move(a);
  qualifiers.get().accessors.clear();
  return *this;
}

QString AddressAccessor::toString() const
{
  return address.toString() + State::toString(qualifiers);
}

QString AddressAccessor::toShortString() const
{
  return address.toShortString() + State::toString(qualifiers);
}

optional<AddressAccessor> AddressAccessor::fromString(const QString& str)
{
  return parseAddressAccessor(str);
}

bool AddressAccessor::operator==(const AddressAccessor& other) const
{
  return address == other.address && qualifiers == other.qualifiers;
}

bool AddressAccessor::operator!=(const AddressAccessor& a) const
{
  return !(*this == a);
}

QString AddressAccessorHead::toString() const
{
  return name + State::toString(qualifiers);
}

QDebug operator<<(QDebug d, const State::Address& a)
{
  d.noquote().nospace() << a.device % ":/" % a.path.join('/');
  return d;
}

QDebug operator<<(QDebug d, const State::AccessorVector& a)
{
  if (!a.empty())
  {
    QString str;

    for (auto acc : a)
    {
      str += "[" % QString::number(acc) % "]";
    }

    d.noquote().nospace() << str;
  }

  return d;
}

QDebug operator<<(QDebug d, const State::DestinationQualifiers& val)
{
  auto& a = val.get();
  d.noquote().nospace() << a.accessors;
  if (a.unit)
    d << QString::fromStdString(ossia::get_pretty_unit_text(a.unit));

  return d;
}

QDebug operator<<(QDebug d, const State::AddressAccessor& a)
{
  d.noquote().nospace() << a.address << a.qualifiers;
  return d;
}

QDebug operator<<(QDebug d, const State::AddressAccessorHead& a)
{
  d.noquote().nospace() << a.name << a.qualifiers;
  return d;
}

QStringList stringList(const Address& addr)
{
  return QStringList{} << addr.device << addr.path;
}

QString toString(const ossia::destination_qualifiers& qualifiers)
{
  QString str;
  if (qualifiers.unit)
  {
    auto unit_text
        = QString::fromStdString(ossia::get_pretty_unit_text(qualifiers.unit));
    if (!qualifiers.accessors.empty())
    {
      char c
          = ossia::get_unit_accessor(qualifiers.unit, qualifiers.accessors[0]);
      if (c != 0)
      {
        unit_text += '.';
        unit_text += c;
      }
    }

    str += "@[" % std::move(unit_text) % "]";
  }
  else
  {
    if(!qualifiers.accessors.empty())
    {
      str += '@';
      for (auto acc : qualifiers.accessors)
      {
        str += "[" % QString::number(acc) % "]";
      }
    }
  }
  return str;
}
}

std::size_t std::hash<State::Address>::
operator()(const State::Address& k) const
{
  using std::size_t;
  using std::hash;
  using std::string;

// Compute individual hash values for first,
// second and third and combine them using XOR
// and bit shifting:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
  return (
      (qHash(k.device) ^ (qHashRange(k.path.begin(), k.path.end()) << 1))
      >> 1);
#else
  auto h = qHash(k.device);
  for (const auto& elt : k.path)
  {
    h = (h ^ (qHash(elt) << 1)) >> 1;
  }
  return h;
#endif
}

namespace boost
{
template <>
struct hash<ossia::unit_variant>
{
  std::size_t operator()(const ossia::unit_variant& k) const
  {
    std::size_t seed = 0;
    boost::hash_combine(seed, k.which());
    if(k)
    {
      ossia::apply_nonnull([&] (const auto& ds) {
        boost::hash_combine(seed, ds.which());
      }, k);
    }
    return seed;
  }
};
}

std::size_t std::hash<State::AddressAccessor>::
operator()(const State::AddressAccessor& k) const
{
  std::size_t seed = 0;
  boost::hash_combine(seed, k.address);
  auto& qual = k.qualifiers.get();
  boost::hash_range(seed, qual.accessors.begin(), qual.accessors.end());
  boost::hash_combine(seed, qual.unit.v);
  return seed;
}

std::size_t boost::hash<State::Address>::
operator()(const State::Address& k) const
{
  return std::hash<State::Address>{}(k);
}

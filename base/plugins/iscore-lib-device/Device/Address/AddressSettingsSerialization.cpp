#include <QDataStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>

#include "AddressSettings.hpp"
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <Device/Address/ClipMode.hpp>
#include <Device/Address/Domain.hpp>
#include <Device/Address/IOType.hpp>
#include <State/Value.hpp>
#include <iscore/serialization/AnySerialization.hpp>


template <>
void DataStreamReader::read(
    const Device::AddressSettingsCommon& n)
{
  m_stream << n.value << n.domain << n.ioType << n.clipMode << n.unit
           << n.repetitionFilter << n.extendedAttributes;
}

template <>
void DataStreamWriter::write(Device::AddressSettingsCommon& n)
{
  m_stream >> n.value >> n.domain >> n.ioType >> n.clipMode >> n.unit
           >> n.repetitionFilter >> n.extendedAttributes;
}

template <>
void JSONObjectReader::read(
    const Device::AddressSettingsCommon& n)
{
  // Metadata
  if(n.ioType)
    obj[strings.ioType] = Device::AccessModeText()[*n.ioType];
  obj[strings.ClipMode] = Device::ClipModeStringMap()[n.clipMode];
  obj[strings.Unit]
      = QString::fromStdString(ossia::get_pretty_unit_text(n.unit));

  obj[strings.RepetitionFilter] = static_cast<bool>(n.repetitionFilter);

  // Value, domain and type
  readFrom(n.value);
  obj[strings.Domain] = toJsonObject(n.domain);

  obj[strings.Extended] = toJsonObject(n.extendedAttributes);
}

template <>
void JSONObjectWriter::write(Device::AddressSettingsCommon& n)
{
  n.ioType = Device::AccessModeText().key(obj[strings.ioType].toString());
  n.clipMode
      = Device::ClipModeStringMap().key(obj[strings.ClipMode].toString());
  n.unit
      = ossia::parse_pretty_unit(obj[strings.Unit].toString().toStdString());

  n.repetitionFilter = (ossia::repetition_filter) obj[strings.RepetitionFilter].toBool();

  writeTo(n.value);
  n.domain = fromJsonObject<Device::Domain>(obj[strings.Domain].toObject());

  n.extendedAttributes = fromJsonObject<iscore::any_map>(obj[strings.Extended]);
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
DataStreamReader::read(const Device::AddressSettings& n)
{
  readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
  m_stream << n.name;

  insertDelimiter();
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
DataStreamWriter::write(Device::AddressSettings& n)
{
  writeTo(static_cast<Device::AddressSettingsCommon&>(n));
  m_stream >> n.name;

  checkDelimiter();
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
JSONObjectReader::read(const Device::AddressSettings& n)
{
  readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
  obj[strings.Name] = n.name;
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
JSONObjectWriter::write(Device::AddressSettings& n)
{
  writeTo(static_cast<Device::AddressSettingsCommon&>(n));
  n.name = obj[strings.Name].toString();
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
DataStreamReader::read(const Device::FullAddressSettings& n)
{
  readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
  m_stream << n.address;

  insertDelimiter();
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
DataStreamWriter::write(Device::FullAddressSettings& n)
{
  writeTo(static_cast<Device::AddressSettingsCommon&>(n));
  m_stream >> n.address;

  checkDelimiter();
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
JSONObjectReader::read(const Device::FullAddressSettings& n)
{
  readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
  obj[strings.Address] = toJsonObject(n.address);
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
JSONObjectWriter::write(Device::FullAddressSettings& n)
{
  writeTo(static_cast<Device::AddressSettingsCommon&>(n));
  n.address = fromJsonObject<State::Address>(obj[strings.Address]);
}

template <>
ISCORE_LIB_DEVICE_EXPORT void DataStreamReader::read(
    const Device::FullAddressAccessorSettings& n)
{
  m_stream << n.value << n.domain << n.ioType << n.clipMode
           << n.repetitionFilter << n.extendedAttributes << n.address;
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
DataStreamWriter::write(Device::FullAddressAccessorSettings& n)
{
  m_stream >> n.value >> n.domain >> n.ioType >> n.clipMode
      >> n.repetitionFilter >> n.extendedAttributes >> n.address;
}

template <>
ISCORE_LIB_DEVICE_EXPORT void JSONObjectReader::read(
    const Device::FullAddressAccessorSettings& n)
{
  // Metadata
  if(n.ioType)
    obj[strings.ioType] = Device::AccessModeText()[*n.ioType];
  obj[strings.ClipMode] = Device::ClipModeStringMap()[n.clipMode];

  obj[strings.RepetitionFilter] = static_cast<bool>(n.repetitionFilter);

  // Value, domain and type
  readFrom(n.value);
  obj[strings.Domain] = toJsonObject(n.domain);
  obj[strings.Extended] = toJsonObject(n.extendedAttributes);

  obj[strings.Address] = toJsonObject(n.address);
}

template <>
ISCORE_LIB_DEVICE_EXPORT void
JSONObjectWriter::write(Device::FullAddressAccessorSettings& n)
{
  n.ioType = Device::AccessModeText().key(obj[strings.ioType].toString());
  n.clipMode
      = Device::ClipModeStringMap().key(obj[strings.ClipMode].toString());

  n.repetitionFilter = (ossia::repetition_filter)obj[strings.RepetitionFilter].toBool();


  writeTo(n.value);

  n.domain = fromJsonObject<Device::Domain>(obj[strings.Domain].toObject());
  n.extendedAttributes = fromJsonObject<iscore::any_map>(obj[strings.Extended]);


  n.address = fromJsonObject<State::AddressAccessor>(obj[strings.Address]);
}


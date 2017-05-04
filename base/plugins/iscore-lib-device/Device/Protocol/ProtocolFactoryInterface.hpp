#pragma once
#include <QString>
#include <QVariant>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <iscore/serialization/VisitorCommon.hpp>

#include <iscore_lib_device_export.h>

struct VisitorVariant;

namespace Device
{
struct DeviceSettings;
class DeviceInterface;
class ProtocolSettingsWidget;
class ISCORE_LIB_DEVICE_EXPORT ProtocolFactory
    : public iscore::Interface<ProtocolFactory>
{
  ISCORE_INTERFACE("3f69d72e-318d-42dc-b48c-a806036592f1")

public:
  virtual ~ProtocolFactory();

  virtual QString prettyName() const = 0;

  /** The one with the highest priority
   * will show up first in the protocol list */
  virtual int visualPriority() const;

  virtual DeviceInterface* makeDevice(
      const Device::DeviceSettings& settings,
      const iscore::DocumentContext& ctx)
      = 0;
  virtual ProtocolSettingsWidget* makeSettingsWidget() = 0;
  virtual const Device::DeviceSettings& defaultSettings() const = 0;

  // Save
  virtual void serializeProtocolSpecificSettings(
      const QVariant& data, const VisitorVariant& visitor) const = 0;

  template <typename T>
  void serializeProtocolSpecificSettings_T(
      const QVariant& data, const VisitorVariant& visitor) const
  {
    serialize_dyn(visitor, data.value<T>());
  }

  // Load
  virtual QVariant
  makeProtocolSpecificSettings(const VisitorVariant& visitor) const = 0;

  template <typename T>
  QVariant makeProtocolSpecificSettings_T(const VisitorVariant& vis) const
  {
    return QVariant::fromValue(deserialize_dyn<T>(vis));
  }

  // Returns true if the two devicesettings can coexist at the same time.
  virtual bool checkCompatibility(
      const Device::DeviceSettings& a,
      const Device::DeviceSettings& b) const = 0;
};
}

Q_DECLARE_METATYPE(UuidKey<Device::ProtocolFactory>)

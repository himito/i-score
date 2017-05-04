#pragma once
#include <Device/Protocol/ProtocolFactoryInterface.hpp>
#include <QString>
#include <QVariant>

namespace Device
{
class DeviceInterface;
class ProtocolSettingsWidget;
}
namespace iscore
{
struct DeviceSettings;
} // namespace iscore
struct VisitorVariant;

namespace Engine
{
namespace Network
{
class LocalProtocolFactory final : public Device::ProtocolFactory
{
  ISCORE_CONCRETE("3fad3354-e103-4bc4-a34f-2d0cc4b02565")

public:
  static const Device::DeviceSettings& static_defaultSettings();

private:
  // Implement with OSSIA::Device
  QString prettyName() const override;

  Device::DeviceInterface* makeDevice(
      const Device::DeviceSettings& settings,
      const iscore::DocumentContext& ctx) override;

  const Device::DeviceSettings& defaultSettings() const override;

  Device::ProtocolSettingsWidget* makeSettingsWidget() override;

  QVariant
  makeProtocolSpecificSettings(const VisitorVariant& visitor) const override;

  void serializeProtocolSpecificSettings(
      const QVariant& data, const VisitorVariant& visitor) const override;

  bool checkCompatibility(
      const Device::DeviceSettings& a,
      const Device::DeviceSettings& b) const override;
};
}
}

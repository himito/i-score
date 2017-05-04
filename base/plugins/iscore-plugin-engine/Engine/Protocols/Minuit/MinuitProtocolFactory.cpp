#include <QObject>

#include "MinuitDevice.hpp"
#include "MinuitProtocolFactory.hpp"
#include <Device/Protocol/DeviceSettings.hpp>
#include <Engine/Protocols/Minuit/MinuitProtocolSettingsWidget.hpp>
#include <Engine/Protocols/Minuit/MinuitSpecificSettings.hpp>
#include <ossia/network/base/device.hpp>

namespace Device
{
class DeviceInterface;
class ProtocolSettingsWidget;
}

struct VisitorVariant;

namespace Engine
{
namespace Network
{
QString MinuitProtocolFactory::prettyName() const
{
  return QObject::tr("Minuit");
}

int MinuitProtocolFactory::visualPriority() const
{
  return 1;
}

Device::DeviceInterface* MinuitProtocolFactory::makeDevice(
    const Device::DeviceSettings& settings, const iscore::DocumentContext& ctx)
{
  return new MinuitDevice{settings};
}

const Device::DeviceSettings& MinuitProtocolFactory::defaultSettings() const
{
  static const Device::DeviceSettings settings = [&]() {
    Device::DeviceSettings s;
    s.protocol = concreteKey();
    s.name = "Minuit";
    MinuitSpecificSettings specif;
    s.deviceSpecificSettings = QVariant::fromValue(specif);
    return s;
  }();
  return settings;
}

Device::ProtocolSettingsWidget* MinuitProtocolFactory::makeSettingsWidget()
{
  return new MinuitProtocolSettingsWidget;
}

QVariant MinuitProtocolFactory::makeProtocolSpecificSettings(
    const VisitorVariant& visitor) const
{
  return makeProtocolSpecificSettings_T<MinuitSpecificSettings>(visitor);
}

void MinuitProtocolFactory::serializeProtocolSpecificSettings(
    const QVariant& data, const VisitorVariant& visitor) const
{
  serializeProtocolSpecificSettings_T<MinuitSpecificSettings>(data, visitor);
}

bool MinuitProtocolFactory::checkCompatibility(
    const Device::DeviceSettings& a, const Device::DeviceSettings& b) const
{
  // TODO we should check also for other devices.  Devices should have a "open
  // ports" method that
  // returns the ports they use.
  auto a_p = a.deviceSpecificSettings.value<MinuitSpecificSettings>();
  auto b_p = b.deviceSpecificSettings.value<MinuitSpecificSettings>();
  return a.name != b.name && a_p.inputPort != b_p.inputPort;
}
}
}

#include <QObject>

#include "MIDIDevice.hpp"
#include "MIDIProtocolFactory.hpp"
#include "MIDIProtocolSettingsWidget.hpp"
#include <Device/Protocol/DeviceSettings.hpp>
#include <Engine/Protocols/MIDI/MIDISpecificSettings.hpp>

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
QString MIDIProtocolFactory::prettyName() const
{
  return QObject::tr("MIDI");
}

Device::DeviceInterface* MIDIProtocolFactory::makeDevice(
    const Device::DeviceSettings& settings, const iscore::DocumentContext& ctx)
{
  return new MIDIDevice{settings};
}

const Device::DeviceSettings& MIDIProtocolFactory::defaultSettings() const
{
  static const Device::DeviceSettings settings = [&]() {
    Device::DeviceSettings s;
    s.protocol = concreteKey();
    s.name = "Midi";
    MIDISpecificSettings specif;
    s.deviceSpecificSettings = QVariant::fromValue(specif);
    return s;
  }();
  return settings;
}

Device::ProtocolSettingsWidget* MIDIProtocolFactory::makeSettingsWidget()
{
  return new MIDIProtocolSettingsWidget;
}

QVariant MIDIProtocolFactory::makeProtocolSpecificSettings(
    const VisitorVariant& visitor) const
{
  return makeProtocolSpecificSettings_T<MIDISpecificSettings>(visitor);
}

void MIDIProtocolFactory::serializeProtocolSpecificSettings(
    const QVariant& data, const VisitorVariant& visitor) const
{
  serializeProtocolSpecificSettings_T<MIDISpecificSettings>(data, visitor);
}

bool MIDIProtocolFactory::checkCompatibility(
    const Device::DeviceSettings& a, const Device::DeviceSettings& b) const
{
  return a.name != b.name;
}
}
}

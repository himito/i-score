#pragma once
#include <ossia/network/midi/midi.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <Device/Protocol/ProtocolSettingsWidget.hpp>

class QComboBox;
class QCheckBox;
class QRadioButton;
class QWidget;
class QLineEdit;

namespace Engine
{
namespace Network
{
class MIDIProtocolSettingsWidget final : public Device::ProtocolSettingsWidget
{
  Q_OBJECT

public:
  MIDIProtocolSettingsWidget(QWidget* parent = nullptr);

private:
  Device::DeviceSettings getSettings() const override;

  void setSettings(const Device::DeviceSettings& settings) override;

  void updateDevices(ossia::net::midi::midi_info::Type);
  void updateInputDevices();
  void updateOutputDevices();

  QLineEdit* m_name;
  QCheckBox* m_inButton;
  QCheckBox* m_outButton;
  QComboBox* m_deviceCBox;
};
}
}

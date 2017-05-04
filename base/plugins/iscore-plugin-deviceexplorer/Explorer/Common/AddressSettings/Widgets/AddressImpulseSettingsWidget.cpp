#include "AddressImpulseSettingsWidget.hpp"

namespace Explorer
{
AddressImpulseSettingsWidget::AddressImpulseSettingsWidget(QWidget* parent)
    : AddressSettingsWidget(parent)
{
}

Device::AddressSettings AddressImpulseSettingsWidget::getSettings() const
{
  auto set = getCommonSettings();
  set.value = State::ValueImpl{State::impulse{}};
  return set;
}

void AddressImpulseSettingsWidget::setSettings(
    const Device::AddressSettings& settings)
{
  setCommonSettings(settings);
}

Device::AddressSettings
AddressImpulseSettingsWidget::getDefaultSettings() const
{
  return {};
}
}

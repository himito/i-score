#include "AddressNoneSettingsWidget.hpp"
#include <Explorer/Common/AddressSettings/Widgets/AddressSettingsWidget.hpp>

namespace Explorer
{
AddressNoneSettingsWidget::AddressNoneSettingsWidget(QWidget* parent)
    : AddressSettingsWidget(AddressSettingsWidget::no_widgets_t{}, parent)
{
}

Device::AddressSettings AddressNoneSettingsWidget::getSettings() const
{
  return getCommonSettings();
}

void AddressNoneSettingsWidget::setSettings(
    const Device::AddressSettings& settings)
{
  setCommonSettings(settings);
}

Device::AddressSettings AddressNoneSettingsWidget::getDefaultSettings() const
{
  return {};
}
}

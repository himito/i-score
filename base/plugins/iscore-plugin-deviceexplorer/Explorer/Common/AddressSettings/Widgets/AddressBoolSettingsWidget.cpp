#include <QComboBox>
#include <QFormLayout>
#include <State/ValueConversion.hpp>

#include <QString>
#include <QVariant>

#include "AddressBoolSettingsWidget.hpp"
#include <Explorer/Common/AddressSettings/Widgets/AddressSettingsWidget.hpp>
#include <State/Value.hpp>

class QWidget;

namespace Explorer
{
AddressBoolSettingsWidget::AddressBoolSettingsWidget(QWidget* parent)
    : AddressSettingsWidget(parent)
{
  m_cb = new QComboBox;
  m_cb->addItem(tr("false"), false); // index 0
  m_cb->addItem(tr("true"), true);   // index 1
  // The order is important because then
  // setting the index from a bool is a one-liner in setSettings.

  m_layout->insertRow(0, makeLabel(tr("Value"), this), m_cb);
}

Device::AddressSettings AddressBoolSettingsWidget::getSettings() const
{
  auto settings = getCommonSettings();
  settings.value.val = m_cb->currentData().toBool();

  return settings;
}

Device::AddressSettings AddressBoolSettingsWidget::getDefaultSettings() const
{
  return {};
}

void AddressBoolSettingsWidget::setSettings(
    const Device::AddressSettings& settings)
{
  setCommonSettings(settings);
  m_cb->setCurrentIndex(State::convert::value<bool>(settings.value));
}
}

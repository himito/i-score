#pragma once
#include "AddressSettingsWidget.hpp"
#include <Device/Address/AddressSettings.hpp>

class QComboBox;
class QWidget;

namespace Explorer
{
class AddressBoolSettingsWidget final : public AddressSettingsWidget
{
public:
  explicit AddressBoolSettingsWidget(QWidget* parent = nullptr);

  Device::AddressSettings getSettings() const override;
  Device::AddressSettings getDefaultSettings() const override;

  void setSettings(const Device::AddressSettings& settings) override;

private:
  QComboBox* m_cb{};
};
}

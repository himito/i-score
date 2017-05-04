#pragma once
#include <Device/Address/AddressSettings.hpp>
#include <Explorer/Common/AddressSettings/Widgets/AddressSettingsWidget.hpp>
#include <QComboBox>
#include <QFormLayout>
#include <State/ValueConversion.hpp>
#include <State/Widgets/Values/NumericValueWidget.hpp>
#include <State/Widgets/Values/VecWidgets.hpp>
namespace Explorer
{
template <std::size_t N>
class AddressVecSettingsWidget final : public AddressSettingsWidget
{
public:
  explicit AddressVecSettingsWidget(QWidget* parent = nullptr)
      : AddressSettingsWidget(parent)
  {
    m_valueEdit = new State::VecWidget<N>{this};
    m_domainSelector = new QComboBox{this};
    m_domainSelector->addItems({tr("Float"), tr("Vec")});
    connect(
        m_domainSelector, SignalUtils::QComboBox_currentIndexChanged_int(),
        this, &AddressVecSettingsWidget<N>::on_domainTypeChange);

    m_domainFloatEdit = new State::NumericDomainWidget<float>{this};
    m_domainVecEdit = new State::VecDomainWidget<N>{this};
    m_domainVecEdit->setHidden(true);

    m_layout->insertRow(0, makeLabel(tr("Value"), this), m_valueEdit);
    m_layout->insertRow(
        1, makeLabel(tr("Domain Type"), this), m_domainSelector);
    m_layout->insertRow(2, makeLabel(tr("Domain"), this), m_domainFloatEdit);

    m_domainSelector->setCurrentIndex(0);
  }

  Device::AddressSettings getSettings() const override
  {
    auto settings = getCommonSettings();
    settings.value.val = m_valueEdit->value();
    if (m_domainSelector->currentIndex() == 0)
      settings.domain = m_domainFloatEdit->domain();
    else
      settings.domain = m_domainVecEdit->domain();
    return settings;
  }

  void setSettings(const Device::AddressSettings& settings) override
  {
    setCommonSettings(settings);
    m_valueEdit->setValue(
        State::convert::value<std::array<float, N>>(settings.value));
    if (settings.domain.get().v.target<ossia::domain_base<float>>())
    {
      m_domainFloatEdit->set_domain(settings.domain);
      m_domainSelector->setCurrentIndex(0);
    }
    else
    {
      m_domainVecEdit->set_domain(settings.domain);
      m_domainSelector->setCurrentIndex(1);
    }
  }

  Device::AddressSettings getDefaultSettings() const override
  {
    Device::AddressSettings s;
    s.value.val = std::array<float, N>{};
    s.domain = ossia::make_domain(float{0}, float{1});
    return {};
  }

  void on_domainTypeChange(int id)
  {
    switch (id)
    {
      // Float
      case 0:
      {
        m_layout->replaceWidget(
            m_domainVecEdit, m_domainFloatEdit, Qt::FindDirectChildrenOnly);
        m_domainVecEdit->setHidden(true);
        m_domainVecEdit->setDisabled(true);
        m_domainFloatEdit->setHidden(false);
        m_domainFloatEdit->setDisabled(false);
        break;
      }
      // Vec
      case 1:
      {
        m_layout->replaceWidget(
            m_domainFloatEdit, m_domainVecEdit, Qt::FindDirectChildrenOnly);
        m_domainVecEdit->setHidden(false);
        m_domainVecEdit->setDisabled(false);
        m_domainFloatEdit->setHidden(true);
        m_domainFloatEdit->setDisabled(true);
        break;
      }
    }
  }

  State::VecWidget<N>* m_valueEdit{};
  QComboBox* m_domainSelector{};
  State::NumericDomainWidget<float>* m_domainFloatEdit{};
  State::VecDomainWidget<N>* m_domainVecEdit{};
};
}

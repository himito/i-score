#pragma once

#include <Device/Address/AddressSettings.hpp>
#include <iscore_plugin_deviceexplorer_export.h>
#include <QWidget>

class QComboBox;
class QCheckBox;
class QFormLayout;


namespace Explorer
{
class ISCORE_PLUGIN_DEVICEEXPLORER_EXPORT AddressSettingsWidget : public QWidget
{
    public:
        struct no_widgets_t {};
        explicit AddressSettingsWidget(QWidget* parent = nullptr);
        explicit AddressSettingsWidget(no_widgets_t, QWidget* parent = nullptr);

        virtual ~AddressSettingsWidget();

        virtual Device::AddressSettings getSettings() const = 0;
        virtual void setSettings(const Device::AddressSettings& settings) = 0;

    protected:
        Device::AddressSettings getCommonSettings() const;
        void setCommonSettings(const Device::AddressSettings&);
        QFormLayout* m_layout;

    private:
        bool m_none_type{false};
        QComboBox* m_ioTypeCBox{};
        QComboBox* m_clipModeCBox{};
        QCheckBox* m_repetition{};
        QComboBox* m_tagsEdit{};
};
}


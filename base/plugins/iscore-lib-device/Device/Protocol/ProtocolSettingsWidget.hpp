#pragma once

#include <Device/Protocol/DeviceSettings.hpp>
#include <QString>
#include <QWidget>
#include <iscore_lib_device_export.h>

namespace Device
{
class ISCORE_LIB_DEVICE_EXPORT ProtocolSettingsWidget : public QWidget
{
public:
  explicit ProtocolSettingsWidget(QWidget* parent = nullptr) : QWidget(parent)
  {
  }

  virtual ~ProtocolSettingsWidget();
  virtual Device::DeviceSettings getSettings() const = 0;
  virtual QString getPath() const // TODO berk, remove me
  {
    return QString("");
  }
  virtual void setSettings(const Device::DeviceSettings& settings) = 0;
};
}

#pragma once
#include <QWidget>
#include <Device/Address/AddressSettings.hpp>
#include <iscore_plugin_deviceexplorer_export.h>

class QLineEdit;
namespace Explorer
{
class DeviceExplorerModel;
class ISCORE_PLUGIN_DEVICEEXPLORER_EXPORT AddressAccessorEditWidget final
    : public QWidget
{
  Q_OBJECT
public:
  AddressAccessorEditWidget(DeviceExplorerModel& model, QWidget* parent);

  void setAddress(const State::AddressAccessor& addr);
  void setFullAddress(Device::FullAddressAccessorSettings&& addr);

  const Device::FullAddressAccessorSettings& address() const;

  QString addressString() const;

  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent*) override;
signals:
  void addressChanged(const Device::FullAddressAccessorSettings&);

private:
  void customContextMenuEvent(const QPoint& p);

  QLineEdit* m_lineEdit{};
  Device::FullAddressAccessorSettings m_address;
  DeviceExplorerModel& m_model;
};
}

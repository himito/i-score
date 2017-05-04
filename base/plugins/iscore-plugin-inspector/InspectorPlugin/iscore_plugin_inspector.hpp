#pragma once
#include <QObject>
#include <iscore/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <vector>

namespace iscore
{
class InterfaceListBase;
class PanelFactory;
} // namespace iscore

// RENAMEME
class iscore_plugin_inspector : public QObject,
                                public iscore::Plugin_QtInterface,
                                public iscore::FactoryInterface_QtInterface,
                                public iscore::FactoryList_QtInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID FactoryInterface_QtInterface_iid)
  Q_INTERFACES(iscore::Plugin_QtInterface iscore::FactoryInterface_QtInterface
                   iscore::FactoryList_QtInterface)

  ISCORE_PLUGIN_METADATA(1, "0ed1520f-e120-458e-a5a8-b3f05f3b6b6c")
public:
  iscore_plugin_inspector();
  ~iscore_plugin_inspector();

  // Panel interface
  std::vector<std::unique_ptr<iscore::InterfaceBase>> factories(
      const iscore::ApplicationContext&,
      const iscore::InterfaceKey& factoryName) const override;

  // Factory for inspector widgets
  std::vector<std::unique_ptr<iscore::InterfaceListBase>>
  factoryFamilies() override;
};

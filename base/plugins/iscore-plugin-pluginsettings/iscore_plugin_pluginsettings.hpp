#pragma once
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>

class iscore_plugin_pluginsettings
    : public QObject,
      public iscore::Plugin_QtInterface,
      public iscore::FactoryInterface_QtInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID Plugin_QtInterface_iid)
  Q_INTERFACES(iscore::Plugin_QtInterface iscore::FactoryInterface_QtInterface)

  ISCORE_PLUGIN_METADATA(1, "f3407ffc-bb6a-494c-9a6e-d4f40028769e")
public:
  iscore_plugin_pluginsettings();
  virtual ~iscore_plugin_pluginsettings();

private:
  std::vector<std::unique_ptr<iscore::InterfaceBase>> factories(
      const iscore::ApplicationContext& ctx,
      const iscore::InterfaceKey& factoryName) const override;
};

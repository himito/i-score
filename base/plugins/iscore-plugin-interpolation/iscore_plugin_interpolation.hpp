#pragma once
#include <iscore/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>

class iscore_plugin_interpolation final
    : public QObject,
      public iscore::Plugin_QtInterface,
      public iscore::FactoryInterface_QtInterface,
      public iscore::CommandFactory_QtInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID FactoryInterface_QtInterface_iid)
  Q_INTERFACES(iscore::Plugin_QtInterface iscore::FactoryInterface_QtInterface
                   iscore::CommandFactory_QtInterface)
  ISCORE_PLUGIN_METADATA(1, "95fc1f7c-9ffd-4c2d-bb7f-bd43341dee8c")
public:
  iscore_plugin_interpolation();
  virtual ~iscore_plugin_interpolation();

private:
  // Process & inspector
  std::vector<std::unique_ptr<iscore::InterfaceBase>> factories(
      const iscore::ApplicationContext& ctx,
      const iscore::InterfaceKey& factoryName) const override;

  // CommandFactory_QtInterface interface
  std::pair<const CommandGroupKey, CommandGeneratorMap>
  make_commands() override;

};

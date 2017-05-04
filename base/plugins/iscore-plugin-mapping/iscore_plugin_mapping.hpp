#pragma once
#include <QObject>
#include <iscore/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <utility>
#include <vector>

#include <iscore/application/ApplicationContext.hpp>
#include <iscore/command/CommandGeneratorMap.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>

class iscore_plugin_mapping : public QObject,
                              public iscore::Plugin_QtInterface,
                              public iscore::FactoryInterface_QtInterface,
                              public iscore::CommandFactory_QtInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID FactoryInterface_QtInterface_iid)
  Q_INTERFACES(iscore::Plugin_QtInterface iscore::FactoryInterface_QtInterface
                   iscore::CommandFactory_QtInterface)

  ISCORE_PLUGIN_METADATA(1, "e097f02d-4676-492e-98b0-764963e1f792")
public:
  iscore_plugin_mapping();
  virtual ~iscore_plugin_mapping();

private:
  // Process & inspector
  std::vector<std::unique_ptr<iscore::InterfaceBase>> factories(
      const iscore::ApplicationContext& ctx,
      const iscore::InterfaceKey& factoryName) const override;

  // CommandFactory_QtInterface interface
  std::pair<const CommandGroupKey, CommandGeneratorMap>
  make_commands() override;
};

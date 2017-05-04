
#include <iscore_player_export.h>
#include <QCoreApplication>
#include <core/application/ApplicationSettings.hpp>
#include <core/application/SafeQApplication.hpp>
#include <core/document/DocumentBuilder.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentFactory.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentFactory.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <Engine/Executor/Settings/ExecutorModel.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <core/application/ApplicationInterface.hpp>
#include <core/application/ApplicationRegistrar.hpp>

#include <core/plugin/PluginManager.hpp>
#include <core/plugin/PluginDependencyGraph.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/customfactory/FactoryFamily.hpp>
#include <iscore/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <iscore/plugins/settingsdelegate/SettingsDelegateFactory.hpp>
#include <iscore/model/ComponentSerialization.hpp>
#include <iscore/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
#include <Process/ProcessList.hpp>
#include <Engine/Executor/ConstraintComponent.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>
#include <Engine/LocalTree/LocalTreeDocumentPlugin.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <QPluginLoader>
#include <QJsonDocument>
#include <ossia/context.hpp>
#include <ossia-qt/device/qml_device.hpp>
namespace iscore
{
class PlayerImpl :
    public QObject,
    public ApplicationInterface
{
  Q_OBJECT

public:
  PlayerImpl();

  PlayerImpl(bool);

  ~PlayerImpl();

  void init();
  void closeDocument();

  void exec();
  void close();

  void loadFile(QString file);
  void loadArray(QByteArray network);

  void registerDevice(ossia::net::device_base*);
  void releaseDevice(ossia::net::device_base*);
  void play();
  void stop();


  void loadPlugins(
      ApplicationRegistrar& registrar, const ApplicationContext& context);

signals:
  void sig_play();
  void sig_stop();
  void sig_close();
  void sig_loadFile(QString);
  void sig_registerDevice(ossia::net::device_base*);

private:
  void setupLoadedDocument();
  const ApplicationContext& context() const override;
  const ApplicationComponents& components() const override;

  int argc = 1;
  char** argv{};

  // Load core application and plug-ins
  std::unique_ptr<QCoreApplication> m_app{};

  // Application-specific
  ApplicationSettings m_globSettings;
  ApplicationComponentsData m_compData;

  ApplicationComponents m_components{m_compData};
  DocumentList m_documents;
  std::vector<std::unique_ptr<SettingsDelegateModel>> m_settings;
  ApplicationContext m_appContext{m_globSettings, m_components, m_documents, m_settings};

  // Document-specific
  std::unique_ptr<Document> m_currentDocument;
  Explorer::DeviceDocumentPlugin* m_devicesPlugin{};
  Engine::Execution::DocumentPlugin* m_execPlugin{};
  Engine::LocalTree::DocumentPlugin* m_localTreePlugin{};
  std::unique_ptr<Engine::Execution::ClockManager> m_clock;

  std::vector<ossia::net::device_base*> m_ownedDevices;
};

}

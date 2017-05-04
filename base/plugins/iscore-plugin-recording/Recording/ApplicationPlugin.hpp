#pragma once
#include <iscore/plugins/application/GUIApplicationPlugin.hpp>
#include <memory>
#include <vector>

#include "Record/RecordManager.hpp"
#include "Record/RecordMessagesManager.hpp"
namespace Engine
{
class ApplicationPlugin;
}
class QAction;
namespace Scenario
{
class ProcessModel;
struct Point;
} // namespace Scenario
namespace Recording
{
class ApplicationPlugin final : public QObject,
                                public iscore::GUIApplicationPlugin
{
public:
  ApplicationPlugin(const iscore::GUIApplicationContext& app);

  void record(const Scenario::ProcessModel&, Scenario::Point pt);
  void recordMessages(const Scenario::ProcessModel&, Scenario::Point pt);
  void stopRecord();

private:
  Engine::ApplicationPlugin* m_ossiaplug{};
  QAction* m_stopAction{};

  std::unique_ptr<RecordContext> m_currentContext{};
  std::unique_ptr<SingleRecorder<AutomationRecorder>> m_recManager;
  std::unique_ptr<SingleRecorder<MessageRecorder>> m_recMessagesManager;
};
}

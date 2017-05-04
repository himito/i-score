#pragma once

#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/actions/Menu.hpp>

#include <Process/Layer/LayerContextMenu.hpp>
#include <QAction>
#include <iscore_plugin_scenario_export.h>
namespace iscore
{
struct GUIElements;
}
namespace Scenario
{
class ScenarioApplicationPlugin;
class TemporalScenarioPresenter;
class ISCORE_PLUGIN_SCENARIO_EXPORT StateActions : public QObject
{
public:
  StateActions(ScenarioApplicationPlugin* parent);

  void makeGUIElements(iscore::GUIElements& ref);
  void setupContextMenu(Process::LayerContextMenuManager& ctxm);

private:
  CommandDispatcher<> dispatcher();

  ScenarioApplicationPlugin* m_parent{};
  QAction* m_refreshStates{};
  QAction* m_snapshot{};
};
}

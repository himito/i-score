#pragma once

#include <QList>
#include <QPoint>

#include <iscore/actions/Action.hpp>
#include <iscore/actions/Menu.hpp>
#include <iscore/selection/Selection.hpp>

class QAction;
class QActionGroup;
class QMenu;
class QToolBar;
namespace Scenario
{
class ScenarioApplicationPlugin;
class TemporalScenarioPresenter;
class ToolMenuActions : public QObject
{
public:
  ToolMenuActions(ScenarioApplicationPlugin* parent);

  void makeGUIElements(iscore::GUIElements& ref);

private:
  void keyPressed(int key);
  void keyReleased(int key);

  ScenarioApplicationPlugin* m_parent{};

  QActionGroup* m_scenarioScaleModeActionGroup{};
  QActionGroup* m_scenarioToolActionGroup{};

  QAction* m_scale{};
  QAction* m_grow{};

  QAction* m_shiftAction{};

  QAction* m_selecttool{};
  QAction* m_createtool{};
  QAction* m_playtool{};
};
}

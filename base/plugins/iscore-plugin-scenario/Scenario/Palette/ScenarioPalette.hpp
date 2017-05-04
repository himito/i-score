#pragma once
#include <Process/Tools/ToolPalette.hpp>
#include <QPoint>
#include <Scenario/Palette/Tool.hpp>
#include <Scenario/Palette/Tools/CreationToolState.hpp>
#include <Scenario/Palette/Tools/PlayToolState.hpp>
#include <Scenario/Palette/Tools/SmartTool.hpp>
#include <iscore/statemachine/GraphicsSceneToolPalette.hpp>

#include <Process/ProcessContext.hpp>
#include <Scenario/Palette/ScenarioPoint.hpp>

namespace iscore
{
class CommandStack;
class ObjectLocker;
}

namespace Scenario
{
class EditionSettings;
class TemporalScenarioPresenter;
class TemporalScenarioView;
class MoveConstraintInScenario_StateWrapper;
class MoveLeftBraceInScenario_StateWrapper;
class MoveRightBraceInScenario_StateWrapper;
class MoveEventInScenario_StateWrapper;
class MoveTimeNodeInScenario_StateWrapper;
class ProcessModel;
class ToolPalette final : public GraphicsSceneToolPalette
{
public:
  ToolPalette(Process::LayerContext&, TemporalScenarioPresenter& presenter);

  const TemporalScenarioPresenter& presenter() const
  {
    return m_presenter;
  }
  Scenario::EditionSettings& editionSettings() const;

  const Process::LayerContext& context() const
  {
    return m_context;
  }

  const Scenario::ProcessModel& model() const
  {
    return m_model;
  }

  void on_pressed(QPointF);
  void on_moved(QPointF);
  void on_released(QPointF);
  void on_cancel();

  void activate(Scenario::Tool);
  void desactivate(Scenario::Tool);

private:
  Scenario::Point ScenePointToScenarioPoint(QPointF point);

  TemporalScenarioPresenter& m_presenter;
  const Scenario::ProcessModel& m_model;
  Process::LayerContext& m_context;

  CreationTool<ProcessModel, Scenario::ToolPalette> m_createTool;
  SmartTool<ProcessModel, Scenario::ToolPalette, TemporalScenarioView, Scenario::MoveConstraintInScenario_StateWrapper, Scenario::MoveLeftBraceInScenario_StateWrapper, Scenario::MoveRightBraceInScenario_StateWrapper, Scenario::MoveEventInScenario_StateWrapper, Scenario::MoveTimeNodeInScenario_StateWrapper>
      m_selectTool;

  PlayToolState m_playTool;

  ToolPaletteInputDispatcher<Scenario::Tool, ToolPalette, Process::LayerContext, TemporalScenarioView>
      m_inputDisp;
};
}

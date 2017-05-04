#pragma once

#include <Scenario/Palette/ScenarioPaletteBaseEvents.hpp>
#include <iscore/statemachine/StateMachineUtils.hpp>
#include <iscore_plugin_scenario_export.h>
class QEvent;
namespace Scenario
{
class SlotState;
} // namespace Scenario

namespace Scenario
{
class ISCORE_PLUGIN_SCENARIO_EXPORT ClickOnSlotHandle_Transition final
    : public iscore::MatchedTransition<ClickOnSlotHandle_Event>
{
public:
  ClickOnSlotHandle_Transition(Scenario::SlotState& state);

  Scenario::SlotState& state() const;

protected:
  void onTransition(QEvent* ev) override;

private:
  Scenario::SlotState& m_state;
};
class ISCORE_PLUGIN_SCENARIO_EXPORT MoveOnSlotHandle_Transition final
    : public iscore::MatchedTransition<MoveOnSlotHandle_Event>
{
public:
  MoveOnSlotHandle_Transition(Scenario::SlotState& state);

  Scenario::SlotState& state() const;

protected:
  void onTransition(QEvent* ev) override;

private:
  Scenario::SlotState& m_state;
};

class ISCORE_PLUGIN_SCENARIO_EXPORT ReleaseOnSlotHandle_Transition final
    : public iscore::MatchedTransition<ReleaseOnSlotHandle_Event>
{
public:
  ReleaseOnSlotHandle_Transition(Scenario::SlotState& state);

  Scenario::SlotState& state() const;

protected:
  void onTransition(QEvent* ev) override;

private:
  Scenario::SlotState& m_state;
};
}

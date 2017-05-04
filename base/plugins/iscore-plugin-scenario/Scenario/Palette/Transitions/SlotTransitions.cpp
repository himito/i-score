#include <algorithm>

#include "SlotTransitions.hpp"
#include <Scenario/Palette/ScenarioPaletteBaseStates.hpp>
#include <iscore/model/path/Path.hpp>

class QEvent;

namespace Scenario
{
ClickOnSlotHandle_Transition::ClickOnSlotHandle_Transition(SlotState& state)
    : m_state{state}
{
}

SlotState& ClickOnSlotHandle_Transition::state() const
{
  return m_state;
}

void ClickOnSlotHandle_Transition::onTransition(QEvent* ev)
{
  auto qev = static_cast<event_type*>(ev);

  this->state().currentSlot = std::move(qev->path);
}

MoveOnSlotHandle_Transition::MoveOnSlotHandle_Transition(SlotState& state)
    : m_state{state}
{
}

SlotState& MoveOnSlotHandle_Transition::state() const
{
  return m_state;
}

void MoveOnSlotHandle_Transition::onTransition(QEvent* ev)
{
  auto qev = static_cast<event_type*>(ev);

  this->state().currentSlot = std::move(qev->path);
}

ReleaseOnSlotHandle_Transition::ReleaseOnSlotHandle_Transition(
    SlotState& state)
    : m_state{state}
{
}

SlotState& ReleaseOnSlotHandle_Transition::state() const
{
  return m_state;
}

void ReleaseOnSlotHandle_Transition::onTransition(QEvent* ev)
{
  auto qev = static_cast<event_type*>(ev);

  this->state().currentSlot = std::move(qev->path);
}
}

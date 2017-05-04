#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <algorithm>

#include "MoveNewState.hpp"
#include <Scenario/Document/State/StateModel.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>

#include <iscore/model/Identifier.hpp>

namespace Scenario
{
namespace Command
{
Scenario::Command::MoveNewState::MoveNewState(
    Path<Scenario::ProcessModel>&& scenarioPath,
    Id<StateModel>
        stateId,
    double y)
    : m_path(std::move(scenarioPath)), m_stateId{std::move(stateId)}, m_y{y}
{
  auto& scenar = m_path.find();
  m_oldy = scenar.state(m_stateId).heightPercentage();
}

void Scenario::Command::MoveNewState::undo() const
{
  auto& scenar = m_path.find();
  auto& state = scenar.state(m_stateId);
  state.setHeightPercentage(m_oldy);

  updateEventExtent(state.eventId(), scenar);
}

void Scenario::Command::MoveNewState::redo() const
{
  auto& scenar = m_path.find();
  auto& state = scenar.state(m_stateId);
  state.setHeightPercentage(m_y);

  updateEventExtent(state.eventId(), scenar);
}

void Scenario::Command::MoveNewState::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_stateId << m_oldy << m_y;
}

void Scenario::Command::MoveNewState::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_stateId >> m_oldy >> m_y;
}
}
}

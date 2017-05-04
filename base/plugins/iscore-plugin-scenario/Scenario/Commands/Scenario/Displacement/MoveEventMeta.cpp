#include <Scenario/Commands/Scenario/Displacement/MoveEventList.hpp>

#include <QByteArray>
#include <algorithm>

#include "MoveEventMeta.hpp"
#include <Scenario/Commands/Scenario/Displacement/SerializableMoveEvent.hpp>
#include <iscore/application/ApplicationContext.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>

#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/model/Identifier.hpp>

namespace Scenario
{
class EventModel;
namespace Command
{
MoveEventMeta::MoveEventMeta(
    Path<Scenario::ProcessModel>&& scenarioPath,
    Id<EventModel>
        eventId,
    TimeVal newDate,
    double y,
    ExpandMode mode)
    : SerializableMoveEvent{}
    , m_scenario{scenarioPath}
    , m_eventId{std::move(eventId)}
    , m_newY{y}
    , m_moveEventImplementation(
          context.interfaces<MoveEventList>()
              .get(context, MoveEventFactoryInterface::Strategy::MOVE)
              .make(
                  std::move(scenarioPath), m_eventId, std::move(newDate),
                  mode))
{
  auto& scenar = m_scenario.find();
  auto& ev = scenar.event(m_eventId);
  auto states = ev.states();
  if (states.size() == 1)
  {
    auto& st = scenar.states.at(states.front());
    m_oldY = st.heightPercentage();
  }
}

void MoveEventMeta::undo() const
{
  m_moveEventImplementation->undo();
  auto& scenar = m_scenario.find();
  auto& ev = scenar.event(m_eventId);
  auto states = ev.states();
  if (states.size() == 1)
  {
    auto& st = scenar.states.at(states.front());
    if (st.previousConstraint())
      updateConstraintVerticalPos(m_oldY, *st.previousConstraint(), scenar);
    if (st.nextConstraint())
      updateConstraintVerticalPos(m_oldY, *st.nextConstraint(), scenar);
    if (!st.previousConstraint() && !st.nextConstraint())
      st.setHeightPercentage(m_oldY);
  }
}

void MoveEventMeta::redo() const
{
  m_moveEventImplementation->redo();
  if (!m_scenario.valid())
    return;
  auto& scenar = m_scenario.find();
  auto& ev = scenar.event(m_eventId);
  auto states = ev.states();
  if (states.size() == 1)
  {
    auto& st = scenar.states.at(states.front());
    if (!st.previousConstraint() && !st.nextConstraint())
      st.setHeightPercentage(m_newY);
    if (st.previousConstraint())
      updateConstraintVerticalPos(m_newY, *st.previousConstraint(), scenar);
    if (st.nextConstraint())
      updateConstraintVerticalPos(m_newY, *st.nextConstraint(), scenar);
  }
}

const Path<Scenario::ProcessModel>& MoveEventMeta::path() const
{
  return m_moveEventImplementation->path();
}

void MoveEventMeta::serializeImpl(DataStreamInput& s) const
{
  s << m_scenario << m_eventId << m_oldY << m_newY
    << m_moveEventImplementation->serialize();
}

void MoveEventMeta::deserializeImpl(DataStreamOutput& s)
{
  QByteArray cmdData;
  s >> m_scenario >> m_eventId >> m_oldY >> m_newY >> cmdData;

  m_moveEventImplementation
      = context.interfaces<MoveEventList>()
            .get(context, MoveEventFactoryInterface::Strategy::MOVE)
            .make();

  m_moveEventImplementation->deserialize(cmdData);
}

void MoveEventMeta::update(
    const Id<EventModel>& eventId, const TimeVal& newDate, double y,
    ExpandMode mode)
{
  m_moveEventImplementation->update(eventId, newDate, y, mode);
  m_newY = y;
}
}
}

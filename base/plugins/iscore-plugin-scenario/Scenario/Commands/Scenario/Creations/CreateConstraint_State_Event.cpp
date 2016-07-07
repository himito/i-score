#include <Scenario/Process/Algorithms/StandardCreationPolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <iscore/tools/RandomNameProvider.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <QByteArray>
#include <vector>

#include "CreateConstraint_State_Event.hpp"
#include <Process/ModelMetadata.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateConstraint_State.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/NotifyingMap.hpp>



namespace Scenario
{
namespace Command
{
CreateConstraint_State_Event::CreateConstraint_State_Event(
        const Scenario::ProcessModel& scenario,
        Id<StateModel> startState,
        Id<TimeNodeModel> endTimeNode,
        double endStateY):
    m_newEvent{getStrongId(scenario.events)},
    m_createdName{RandomNameProvider::generateRandomName()},
    m_command{scenario,
              std::move(startState),
              m_newEvent,
              endStateY},
    m_endTimeNode{std::move(endTimeNode)}
{

}

CreateConstraint_State_Event::CreateConstraint_State_Event(
        const Path<Scenario::ProcessModel> &scenarioPath,
        Id<StateModel> startState,
        Id<TimeNodeModel> endTimeNode,
        double endStateY):
    CreateConstraint_State_Event{
        scenarioPath.find(),
        std::move(startState),
        std::move(endTimeNode),
        endStateY}
{

}

void CreateConstraint_State_Event::undo() const
{
    m_command.undo();

    ScenarioCreate<EventModel>::undo(
                m_newEvent,
                m_command.scenarioPath().find());
}

void CreateConstraint_State_Event::redo() const
{
    auto& scenar = m_command.scenarioPath().find();

    // Create the end event
    ScenarioCreate<EventModel>::redo(
                m_newEvent,
                scenar.timeNode(m_endTimeNode),
                {m_command.endStateY(), m_command.endStateY()},
                scenar);

    scenar.events.at(m_newEvent).metadata.setName(m_createdName);

    // The state + constraint between
    m_command.redo();
}

void CreateConstraint_State_Event::serializeImpl(DataStreamInput& s) const
{
    s << m_newEvent
      << m_createdName
      << m_command.serialize()
      << m_endTimeNode;
}

void CreateConstraint_State_Event::deserializeImpl(DataStreamOutput& s)
{
    QByteArray b;
    s >> m_newEvent >> m_createdName >> b >> m_endTimeNode;

    m_command.deserialize(b);
}
}
}

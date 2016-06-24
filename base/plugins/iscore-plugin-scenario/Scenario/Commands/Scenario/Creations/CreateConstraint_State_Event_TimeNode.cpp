#include <Scenario/Process/Algorithms/StandardCreationPolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <iscore/tools/RandomNameProvider.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <QByteArray>
#include <vector>

#include "CreateConstraint_State_Event_TimeNode.hpp"
#include <Process/ModelMetadata.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateConstraint_State_Event.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/NotifyingMap.hpp>



namespace Scenario
{
namespace Command
{
CreateConstraint_State_Event_TimeNode::CreateConstraint_State_Event_TimeNode(
        const Scenario::ScenarioModel& scenario,
        Id<StateModel> startState,
        TimeValue date,
        double endStateY):
    m_newTimeNode{getStrongId(scenario.timeNodes)},
    m_createdName{RandomNameProvider::generateRandomName()},
    m_command{scenario,
              std::move(startState),
              m_newTimeNode,
              endStateY},
    m_date{std::move(date)}
{

}

CreateConstraint_State_Event_TimeNode::CreateConstraint_State_Event_TimeNode(
        const Path<Scenario::ScenarioModel>& scenarioPath,
        Id<StateModel> startState,
        TimeValue date,
        double endStateY):
    CreateConstraint_State_Event_TimeNode{scenarioPath.find(),
                                          std::move(startState),
                                          std::move(date),
                                          endStateY}
{

}

void CreateConstraint_State_Event_TimeNode::undo() const
{
    m_command.undo();

    ScenarioCreate<TimeNodeModel>::undo(
                m_newTimeNode,
                m_command.scenarioPath().find());
}

void CreateConstraint_State_Event_TimeNode::redo() const
{
    auto& scenar = m_command.scenarioPath().find();

    // Create the end timenode
    ScenarioCreate<TimeNodeModel>::redo(
                m_newTimeNode,
                {m_command.endStateY(), m_command.endStateY()},
                m_date,
                scenar);

    scenar.timeNode(m_newTimeNode).metadata.setName(m_createdName);

    // The event + state + constraint between
    m_command.redo();
}

void CreateConstraint_State_Event_TimeNode::serializeImpl(DataStreamInput& s) const
{
    s << m_newTimeNode
      << m_createdName
      << m_command.serialize()
      << m_date;
}

void CreateConstraint_State_Event_TimeNode::deserializeImpl(DataStreamOutput& s)
{
    QByteArray b;
    s >> m_newTimeNode
            >> m_createdName
            >> b
            >> m_date;

    m_command.deserialize(b);
}
}
}

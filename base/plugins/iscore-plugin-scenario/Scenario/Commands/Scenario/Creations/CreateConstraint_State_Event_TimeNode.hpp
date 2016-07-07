#pragma once
#include <Process/TimeValue.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <QString>

#include "CreateConstraint_State_Event.hpp"
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_scenario_export.h>
struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class ProcessModel;
class ConstraintModel;
class EventModel;
class StateModel;
class TimeNodeModel;
namespace Command
{
class ISCORE_PLUGIN_SCENARIO_EXPORT CreateConstraint_State_Event_TimeNode final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), CreateConstraint_State_Event_TimeNode, "Create a constraint, a state, an event and a timenode")
        public:

            CreateConstraint_State_Event_TimeNode(
                const Scenario::ProcessModel& scenario,
                Id<StateModel> startState,
                TimeValue date,
                double endStateY);

        CreateConstraint_State_Event_TimeNode(
                const Path<Scenario::ProcessModel>& scenario,
                Id<StateModel> startState,
                TimeValue date,
                double endStateY);

        const Path<Scenario::ProcessModel>& scenarioPath() const
        { return m_command.scenarioPath(); }

        const Id<ConstraintModel>& createdConstraint() const
        { return m_command.createdConstraint(); }

        const Id<StateModel>& startState() const
        { return m_command.startState(); }

        const Id<StateModel>& createdState() const
        { return m_command.createdState(); }

        const Id<EventModel>& createdEvent() const
        { return m_command.createdEvent(); }

        const Id<TimeNodeModel>& createdTimeNode() const
        { return m_newTimeNode; }

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput&) const override;
        void deserializeImpl(DataStreamOutput&) override;

    private:
        Id<TimeNodeModel> m_newTimeNode;
        QString m_createdName;

        CreateConstraint_State_Event m_command;

        TimeValue m_date;
};
}
}

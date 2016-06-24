#pragma once
#include <iscore/tools/std/Optional.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <QString>

#include "CreateState.hpp"
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class EventModel;
class StateModel;
class ScenarioModel;
class TimeNodeModel;
namespace Command
{
class ISCORE_PLUGIN_SCENARIO_EXPORT CreateEvent_State final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), CreateEvent_State, "Create an event and a state")
        public:

          CreateEvent_State(
            const Scenario::ScenarioModel& scenario,
            Id<TimeNodeModel> timeNode,
            double stateY);
        CreateEvent_State(
          const Path<Scenario::ScenarioModel>& scenario,
          Id<TimeNodeModel> timeNode,
          double stateY);

        const Path<Scenario::ScenarioModel>& scenarioPath() const
        { return m_command.scenarioPath(); }

        const Id<StateModel>& createdState() const
        { return m_command.createdState(); }

        const Id<EventModel>& createdEvent() const
        { return m_newEvent; }

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput&) const override;
        void deserializeImpl(DataStreamOutput&) override;

    private:
        Id<EventModel> m_newEvent;
        QString m_createdName;

        CreateState m_command;

        Id<TimeNodeModel> m_timeNode;
};
}
}

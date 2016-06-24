#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <QMap>

#include <Process/State/MessageNode.hpp>
#include <State/Message.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

#include <iscore_plugin_scenario_export.h>
struct DataStreamInput;
struct DataStreamOutput;
namespace Process { class ProcessModel; }

namespace Scenario
{
class MessageItemModel;
namespace Command
{
class ISCORE_PLUGIN_SCENARIO_EXPORT AddMessagesToState final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), AddMessagesToState, "Add messages to state")
        public:

          AddMessagesToState(
            Path<MessageItemModel>&&,
            const State::MessageList& messages);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput&) const override;
        void deserializeImpl(DataStreamOutput&) override;

    private:
        Path<MessageItemModel> m_path;

        Process::MessageNode m_oldState, m_newState;

        QMap<Id<Process::ProcessModel>, State::MessageList> m_previousBackup;
        QMap<Id<Process::ProcessModel>, State::MessageList> m_followingBackup;
};
}
}

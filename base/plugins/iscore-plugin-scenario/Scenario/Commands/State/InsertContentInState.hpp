#pragma once
#include <Process/State/MessageNode.hpp>
#include <QJsonObject>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/Command.hpp>

#include <iscore/model/path/Path.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class StateModel;
namespace Command
{

class InsertContentInState final : public iscore::Command
{
  ISCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      InsertContentInState,
      "Insert content in a state")

public:
  InsertContentInState(
      const QJsonObject& stateData, Path<StateModel>&& targetState);

  void undo() const override;

  void redo() const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;

  void deserializeImpl(DataStreamOutput& s) override;

private:
  Process::MessageNode m_oldNode;
  Process::MessageNode m_newNode;
  Path<StateModel> m_state;
};
}
}

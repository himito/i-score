#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/Command.hpp>

#include <iscore/model/path/Path.hpp>
#include <iscore/model/Identifier.hpp>

#include <iscore_plugin_scenario_export.h>
namespace Process
{
class StateProcess;
class StateProcessFactory;
}
namespace Scenario
{
class StateModel;
namespace Command
{

class ISCORE_PLUGIN_SCENARIO_EXPORT RemoveStateProcess final
    : public iscore::Command
{
  ISCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      RemoveStateProcess,
      "Remove a state process")
public:
  RemoveStateProcess(
      Path<StateModel>&& statePath, Id<Process::StateProcess> processId);

  void undo() const override;
  void redo() const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<StateModel> m_path;
  UuidKey<Process::StateProcessFactory> m_processUuid;

  Id<Process::StateProcess> m_processId{};
};
}
}

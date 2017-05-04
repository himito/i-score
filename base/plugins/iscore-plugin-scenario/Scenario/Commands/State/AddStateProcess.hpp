#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/Command.hpp>

#include <iscore/model/path/Path.hpp>
#include <iscore/model/Identifier.hpp>

#include <iscore_plugin_scenario_export.h>

// RENAMEME

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
class ISCORE_PLUGIN_SCENARIO_EXPORT AddStateProcessToState final
    : public iscore::Command
{
  ISCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      AddStateProcessToState,
      "Add a state process")
public:
  AddStateProcessToState(
      Path<StateModel>&& state, UuidKey<Process::StateProcessFactory> process);
  AddStateProcessToState(
      Path<StateModel>&& state,
      Id<Process::StateProcess>
          idToUse,
      UuidKey<Process::StateProcessFactory>
          process);

  void undo() const override;
  void redo() const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<StateModel> m_path;
  UuidKey<Process::StateProcessFactory> m_processName;

  Id<Process::StateProcess> m_createdProcessId{};
};
}
}

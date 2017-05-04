#pragma once
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/Command.hpp>

#include <iscore/model/path/Path.hpp>

struct DataStreamInput;
struct DataStreamOutput;
namespace Process
{
class ProcessModel;
}

namespace Scenario
{
namespace Command
{
class SetProcessDuration final : public iscore::Command
{
  ISCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      SetProcessDuration,
      "Change a process duration")

public:
  SetProcessDuration(Path<Process::ProcessModel>&& path, TimeVal newVal);

  void undo() const override;
  void redo() const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<Process::ProcessModel> m_path;
  TimeVal m_old;
  TimeVal m_new;
};
}
}

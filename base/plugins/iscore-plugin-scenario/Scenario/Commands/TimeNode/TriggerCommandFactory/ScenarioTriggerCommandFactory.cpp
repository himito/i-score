#include <Scenario/Commands/TimeNode/AddTrigger.hpp>
#include <Scenario/Commands/TimeNode/RemoveTrigger.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <QByteArray>

#include <Scenario/Process/ScenarioInterface.hpp>
#include "ScenarioTriggerCommandFactory.hpp"
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
namespace Scenario
{
namespace Command
{

bool ScenarioTriggerCommandFactory::matches(const TimeNodeModel& tn) const
{
  return dynamic_cast<Scenario::ProcessModel*>(tn.parent());
}

iscore::SerializableCommand*ScenarioTriggerCommandFactory::make_addTriggerCommand(const TimeNodeModel& tn) const
{
  if(dynamic_cast<Scenario::ProcessModel*>(tn.parent()))
  {
    return new Scenario::Command::AddTrigger<Scenario::ProcessModel>(tn);
  }
  return nullptr;
}

iscore::SerializableCommand*ScenarioTriggerCommandFactory::make_removeTriggerCommand(const TimeNodeModel& tn) const
{
  if(dynamic_cast<Scenario::ProcessModel*>(tn.parent()))
  {
    return new Scenario::Command::RemoveTrigger<Scenario::ProcessModel>(tn);
  }
  return nullptr;
}
}
}

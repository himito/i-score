#include <QByteArray>
#include <Scenario/Commands/TimeNode/AddTrigger.hpp>
#include <Scenario/Commands/TimeNode/RemoveTrigger.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

#include "BaseScenarioTriggerCommandFactory.hpp"
#include <Scenario/Process/ScenarioInterface.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{
bool BaseScenarioTriggerCommandFactory::matches(const TimeNodeModel& tn) const
{
  return dynamic_cast<BaseScenario*>(tn.parent());
}

iscore::Command*
BaseScenarioTriggerCommandFactory::make_addTriggerCommand(
    const TimeNodeModel& tn) const
{
  if (dynamic_cast<BaseScenario*>(tn.parent()))
  {
    return new Scenario::Command::AddTrigger<BaseScenario>(tn);
  }
  return nullptr;
}

iscore::Command*
BaseScenarioTriggerCommandFactory::make_removeTriggerCommand(
    const TimeNodeModel& tn) const
{
  if (dynamic_cast<BaseScenario*>(tn.parent()))
  {
    return new Scenario::Command::RemoveTrigger<BaseScenario>(tn);
  }
  return nullptr;
}
}
}

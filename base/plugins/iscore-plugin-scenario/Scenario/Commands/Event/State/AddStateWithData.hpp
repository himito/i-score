#pragma once

#include <Scenario/Commands/Scenario/Creations/CreateState.hpp>
#include <Scenario/Commands/State/AddMessagesToState.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <iscore/command/AggregateCommand.hpp>

namespace Scenario
{
namespace Command
{
class AddStateWithData final : public iscore::AggregateCommand
{
  ISCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      AddStateWithData,
      "Drop a new state in an event")
public:
  AddStateWithData(
      const Scenario::ProcessModel& scenario,
      Id<EventModel>
          ev,
      double ypos,
      State::MessageList&& stateData)
  {
    auto createStateCmd = new CreateState{scenario, std::move(ev), ypos};

    // We create the path of the to-be state
    auto path = createStateCmd->scenarioPath().extend(
        createStateCmd->createdState());

    addCommand(createStateCmd);
    addCommand(new AddMessagesToState{std::move(path), stateData});
  }
};
}
}

#include "RecordTools.hpp"
#include <Recording/Record/RecordProviderFactory.hpp>

#include <Scenario/Commands/Constraint/Rack/AddSlotToRack.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateConstraint_State_Event_TimeNode.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateTimeNode_Event_State.hpp>
#include <Scenario/Commands/Scenario/ShowRackInViewModel.hpp>
#include <Scenario/Palette/ScenarioPoint.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Explorer/Explorer/DeviceExplorerModel.hpp>
namespace Recording
{


static void GetParametersRecursive(Device::Node* parent, std::vector<Device::Node*>& vec)
{
  vec.reserve(vec.size() + parent->childCount());
  for (auto& node : *parent)
  {
    vec.push_back(&node);
    GetParametersRecursive(&node, vec);
  }
}
static std::vector<Device::Node*>
GetParametersRecursive(const std::vector<Device::Node*>& parents)
{
  std::vector<Device::Node*> res;
  for (auto node : parents)
  {
    res.push_back(node);
    GetParametersRecursive(node, res);
  }

  for (auto it = res.begin(); it != res.end();)
  {
    bool ok = true;
    Device::Node* n = *it;
    ok &= n->is<Device::AddressSettings>();
    if (ok)
    {
      auto& as = n->get<Device::AddressSettings>();
      ok &= as.value.val.isValid()
            && (std::size_t)as.value.val.which()
                   < (std::size_t)State::ValueType::NoValue;
    }

    if (ok)
    {
      ++it;
    }
    else
    {
      it = res.erase(it);
    }
  }

  return res;
}

RecordListening
GetAddressesToRecordRecursive(Explorer::DeviceExplorerModel& explorer)
{
  RecordListening recordListening;

  auto parameters = GetParametersRecursive(
      explorer.uniqueSelectedNodes(explorer.selectedIndexes()).parents);

  // First get the addresses to listen.
  for (auto node_ptr : parameters)
  {
    // TODO use address settings instead.
    auto& node = *node_ptr;
    if (!node.is<Device::AddressSettings>())
      continue;

    auto addr = Device::address(node);
    // TODO shall we check if the address is in, out, recordable ?
    // Recording an automation of strings would actually have a meaning
    // here (for instance recording someone typing).

    // We sort the addresses by device to optimize.
    auto dev_it = ossia::find_if(recordListening, [&](const auto& vec) {
      return Device::deviceName(*vec.front()) == addr.address.device;
    });

    if (dev_it != recordListening.end())
    {
      dev_it->push_back(node_ptr);
    }
    else
    {
      recordListening.push_back({node_ptr});
    }
  }

  return recordListening;
}
/*
RecordListening GetAddressesToRecord(
        Explorer::DeviceExplorerModel& explorer)
{
    std::vector<std::vector<Device::FullAddressSettings>> recordListening;

    auto indices = explorer.selectedIndexes(); // TODO maybe
filterUniqueParents and then recurse on the listening ??

    // First get the addresses to listen.
    for(auto& index : indices)
    {
        // TODO use address settings instead.
        auto& node = explorer.nodeFromModelIndex(index);
        if(!node.is<Device::AddressSettings>())
            continue;

        auto addr = Device::address(node);
        // TODO shall we check if the address is in, out, recordable ?
        // Recording an automation of strings would actually have a meaning
        // here (for instance recording someone typing).

        // We sort the addresses by device to optimize.
        auto dev_it = find_if(recordListening,
                              [&] (const auto& vec)
        { return vec.front().address.device == addr.device; });

        auto& as = node.get<Device::AddressSettings>();
        if(dev_it != recordListening.end())
        {
            dev_it->push_back(Device::FullAddressSettings::make<Device::FullAddressSettings::as_child>(as,
addr));
        }
        else
        {
            recordListening.push_back({Device::FullAddressSettings::make<Device::FullAddressSettings::as_child>(as,
addr)});
        }
    }

    return recordListening;
}
*/


Box CreateBox(RecordContext& context)
{
  // Get the clicked point in scenario and create a state + constraint + state
  // there
  // Create an automation + a rack + a slot + process views for all
  // automations.
  auto default_end_date = context.point.date;
  auto cmd_start = new Scenario::Command::CreateTimeNode_Event_State{
      context.scenario, context.point.date, context.point.y};
  cmd_start->redo();
  context.dispatcher.submitCommand(cmd_start);

  // TODO what happens if we go past the end of our scenario ? Stop recording
  // ??
  auto cmd_end = new Scenario::Command::CreateConstraint_State_Event_TimeNode{
      context.scenario, cmd_start->createdState(), default_end_date,
      context.point.y};
  cmd_end->redo();
  context.dispatcher.submitCommand(cmd_end);

  auto& cstr = context.scenario.constraints.at(cmd_end->createdConstraint());

  auto cmd_move = new Scenario::Command::MoveNewEvent(
      context.scenario,
      cstr.id(),
      cmd_end->createdEvent(),
      default_end_date,
      0,
      true,
      ExpandMode::CannotExpand);
  context.dispatcher.submitCommand(cmd_move);

  auto cmd_slot = new Scenario::Command::AddSlotToRack{cstr};
  cmd_slot->redo();
  context.dispatcher.submitCommand(cmd_slot);

  auto cmd_showrack = new Scenario::Command::ShowRack{cstr};
  cmd_showrack->redo();
  context.dispatcher.submitCommand(cmd_showrack);

  return {cstr, *cmd_move, cmd_end->createdEvent()};
}

}

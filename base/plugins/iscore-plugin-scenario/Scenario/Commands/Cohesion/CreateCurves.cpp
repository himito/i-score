#include <Scenario/Commands/Cohesion/CreateCurveFromStates.hpp>
#include <Scenario/Commands/Cohesion/InterpolateMacro.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <Explorer/Explorer/DeviceExplorerModel.hpp>
#include <Process/State/MessageNode.hpp>
#include <QList>
#include <QPointer>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>

#include "CreateCurves.hpp"
#include <Device/Address/AddressSettings.hpp>
#include <Device/Node/DeviceNode.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/selection/Selectable.hpp>
#include <iscore/selection/SelectionStack.hpp>
#include <iscore/model/IdentifiedObjectAbstract.hpp>

#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/network/domain/domain.hpp>
#include <Automation/AutomationModel.hpp>
namespace Scenario
{
static std::vector<Device::FullAddressSettings>
getSelectedAddresses(const iscore::DocumentContext& doc)
{
  // First get the addresses
  auto& device_explorer = Explorer::deviceExplorerFromContext(doc);

  std::vector<Device::FullAddressSettings> addresses;
  for (const auto& index : device_explorer.selectedIndexes())
  {
    const auto& node = device_explorer.nodeFromModelIndex(index);
    if (node.is<Device::AddressSettings>())
    {
      const Device::AddressSettings& addr
          = node.get<Device::AddressSettings>();
      if (addr.value.val.isNumeric() || addr.value.val.isArray())
      {
        Device::FullAddressSettings as;
        static_cast<Device::AddressSettingsCommon&>(as) = addr;
        as.address = Device::address(node).address;
        addresses.push_back(std::move(as));
      }
    }
  }
  return addresses;
}

void CreateCurves(
    const QList<const Scenario::ConstraintModel*>& selected_constraints,
    const iscore::CommandStackFacade& stack)
{
  if (selected_constraints.empty())
    return;

  // For each constraint, interpolate between the states in its start event and
  // end event.
  auto& doc
      = iscore::IDocument::documentContext(*selected_constraints.first());

  auto addresses = getSelectedAddresses(doc);
  if (addresses.empty())
    return;

  CreateCurvesFromAddresses(selected_constraints, addresses, stack);
}

void CreateCurvesFromAddresses(
    const QList<const Scenario::ConstraintModel*>& selected_constraints,
    const std::vector<Device::FullAddressSettings>& addresses,
    const iscore::CommandStackFacade& stack)
{
  if (selected_constraints.empty())
    return;

  // They should all be in the same scenario so we can select the first.
  // FIXME check that the other "cohesion" methods also use ScenarioInterface
  // and not Scenario::ProcessModel
  auto scenar = dynamic_cast<Scenario::ScenarioInterface*>(
      selected_constraints.first()->parent());

  int added_processes = 0;
  // Then create the commands
  auto big_macro
      = new Scenario::Command::AddMultipleProcessesToMultipleConstraintsMacro;

  for (const auto& constraint_ptr : selected_constraints)
  {
    auto& constraint = *constraint_ptr;
    // Generate brand new ids for the processes
    auto process_ids = getStrongIdRange<Process::ProcessModel>(
        addresses.size(), constraint.processes);
    auto macro = Scenario::Command::makeAddProcessMacro(constraint, addresses.size());

    const Scenario::StateModel& ss = startState(constraint, *scenar);
    const auto& es = endState(constraint, *scenar);

    std::vector<State::AddressAccessor> existing_automations;
    for (const auto& proc : constraint.processes)
    {
      if (auto autom = dynamic_cast<const Automation::ProcessModel*>(&proc))
        existing_automations.push_back(autom->address());
    }

    int i = 0;
    for (const Device::FullAddressSettings& as : addresses)
    {
      State::AddressAccessor addr{as.address, {}, as.unit};
      // First, we skip the curve if there is already a curve
      // with this address in the constraint.
      if (ossia::contains(existing_automations, addr))
        continue;

      // Then we set-up all the necessary values
      // min / max
      Curve::CurveDomain dom{as.domain.get(), as.value};

      // start value / end value
      Process::MessageNode* s_node = Device::try_getNodeFromString(
          ss.messages().rootNode(), stringList(as.address));
      if (s_node)
      {
        if (auto val = s_node->value())
        {
          dom.start = State::convert::value<double>(*val);
          dom.min = std::min(dom.start, dom.min);
          dom.max = std::max(dom.start, dom.max);
        }
      }

      Process::MessageNode* e_node = Device::try_getNodeFromString(
          es.messages().rootNode(), stringList(as.address));
      if (e_node)
      {
        if (auto val = e_node->value())
        {
          dom.end = State::convert::value<double>(*val);
          dom.min = std::min(dom.end, dom.min);
          dom.max = std::max(dom.end, dom.max);
        }
      }

      // Send the command.
      macro->addCommand(new Scenario::Command::CreateAutomationFromStates{
          constraint, macro->slotsToUse, process_ids[i], addr, dom});

      i++;
      added_processes++;
    }
    big_macro->addCommand(macro);
  }

  if (added_processes > 0)
  {
    CommandDispatcher<> disp{stack};
    disp.submitCommand(big_macro);
  }
  else
  {
    delete big_macro;
  }
}
}

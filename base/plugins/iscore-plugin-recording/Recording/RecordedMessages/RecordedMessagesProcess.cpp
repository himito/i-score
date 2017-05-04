#include <Engine/Executor/DocumentPlugin.hpp>
#include <Engine/OSSIA2iscore.hpp>
#include <Engine/iscore2OSSIA.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <vector>

#include <ossia/editor/state/message.hpp>
#include <ossia/editor/state/state.hpp>

#include "RecordedMessagesProcess.hpp"
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <Recording/RecordedMessages/RecordedMessagesProcessModel.hpp>

namespace RecordedMessages
{
namespace Executor
{
ProcessExecutor::ProcessExecutor(
    const Explorer::DeviceDocumentPlugin& devices,
    const RecordedMessagesList& lst)
    : m_devices{devices.list()}, m_list{lst}
{
  m_list.detach();
}

ossia::state_element ProcessExecutor::state()
{
  return state(parent()->get_date() / parent()->get_nominal_duration());
}

ossia::state_element ProcessExecutor::state(double t)
{
  ossia::state st;
  ossia::time_constraint& par_cst = *parent();

  auto cur_pos = t;
  auto span = par_cst.get_granularity(); // TODO this does not make sense :
  // granularity is in MS

  auto max_pos = cur_pos + span;

  // Look for all the messages

  int n = m_list.size();
  for (int i = 0; i < n; i++)
  {
    auto& mess = m_list[i];
    auto time = mess.percentage * par_cst.get_nominal_duration();
    if (time >= cur_pos && time < max_pos)
    {
      st.add(Engine::iscore_to_ossia::message(mess.message, m_devices));
    }
  }

  if(unmuted())
    return st;
  return {};
}

ossia::state_element ProcessExecutor::offset(ossia::time_value off)
{
  return state(off / parent()->get_nominal_duration());
}

Component::Component(
    Engine::Execution::ConstraintComponent& parentConstraint,
    RecordedMessages::ProcessModel& element,
    const Engine::Execution::Context& ctx,
    const Id<iscore::Component>& id,
    QObject* parent)
    : ::Engine::Execution::
          ProcessComponent_T<RecordedMessages::ProcessModel, ProcessExecutor>{
              parentConstraint,
              element,
              ctx,
              id,
              "RecordedMessagesComponent",
              parent}
{
  m_ossia_process = std::make_shared<ProcessExecutor>(ctx.devices, element.messages());
}
}
}

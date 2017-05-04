#include "DefaultClockManager.hpp"

#include <Engine/Executor/BaseScenarioComponent.hpp>
#include <Engine/Executor/ConstraintComponent.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>
#include <Engine/Executor/ExecutorContext.hpp>
#include <Engine/Executor/Settings/ExecutorModel.hpp>
#include <Engine/OSSIA2iscore.hpp>

#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>

#include <ossia/editor/scenario/clock.hpp>
namespace Engine
{
namespace Execution
{
DefaultClockManager::~DefaultClockManager() = default;
DefaultClockManager::DefaultClockManager(const Context& ctx)
    : ClockManager{ctx}
{
  auto& bs = ctx.scenario;
  ossia::time_constraint& ossia_cst = *bs.baseConstraint().OSSIAConstraint();

  ossia_cst.set_drive_mode(ossia::clock::drive_mode::INTERNAL);
  ossia_cst.set_granularity(ossia::time_value(
      context.doc.app.settings<Settings::Model>().getRate()));
  ossia_cst.set_callback(makeDefaultCallback(bs));
}
ossia::time_constraint::exec_callback
DefaultClockManager::makeDefaultCallback(
    Engine::Execution::BaseScenarioElement& bs)
{
  auto& cst = bs.baseConstraint();
  return [this, &bs, &iscore_cst = cst.iscoreConstraint() ](
      ossia::time_value position,
      ossia::time_value date,
      const ossia::state_element& state)
  {
    ossia::launch(state);

    auto currentTime = Engine::ossia_to_iscore::time(date);

    auto& cstdur = iscore_cst.duration;
    const auto& maxdur = cstdur.maxDuration();

    if (!maxdur.isInfinite())
      cstdur.setPlayPercentage(currentTime / cstdur.maxDuration());
    else
      cstdur.setPlayPercentage(currentTime / cstdur.defaultDuration());

    // Run some commands if they have been submitted.
    //! TODO it could be interesting to count here the time remaining to the next tick between
    //! each command and do the max that we can achieve
    for(int i = 0; i < 64; i++)
    {
      ExecutionCommand c;
      context.executionQueue.try_dequeue(c);
      if(c)
      {
        c();
      }
      else
      {
        break;
      }
    }
  };
}

void DefaultClockManager::play_impl(
    const TimeVal& t, BaseScenarioElement& bs)
{
  bs.baseConstraint().play(t);
}

void DefaultClockManager::pause_impl(BaseScenarioElement& bs)
{
  bs.baseConstraint().pause();
}

void DefaultClockManager::resume_impl(BaseScenarioElement& bs)
{
  bs.baseConstraint().resume();
}

void DefaultClockManager::stop_impl(BaseScenarioElement& bs)
{
  bs.baseConstraint().stop();
}

DefaultClockManagerFactory::~DefaultClockManagerFactory() = default;
QString DefaultClockManagerFactory::prettyName() const
{
  return QObject::tr("Default");
}

std::unique_ptr<ClockManager>
DefaultClockManagerFactory::make(const Engine::Execution::Context& ctx)
{
  return std::make_unique<DefaultClockManager>(ctx);
}
}
}

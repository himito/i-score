#pragma once
#include <ossia/editor/scenario/time_constraint.hpp>
#include <Engine/Executor/ClockManager/ClockManagerFactory.hpp>
namespace Scenario
{
class ConstraintModel;
}
namespace Engine
{
namespace Execution
{
class BaseScenarioElement;
class ISCORE_PLUGIN_ENGINE_EXPORT DefaultClockManager final
    : public ClockManager
{
public:
  DefaultClockManager(const Context& ctx);

  virtual ~DefaultClockManager();

  //! Creates an execution callback for the root ossia::time_constraint
  ossia::time_constraint::exec_callback
  makeDefaultCallback(Engine::Execution::BaseScenarioElement&);

private:
  void play_impl(const TimeVal& t, BaseScenarioElement&) override;
  void pause_impl(BaseScenarioElement&) override;
  void resume_impl(BaseScenarioElement&) override;
  void stop_impl(BaseScenarioElement&) override;
};

class ISCORE_PLUGIN_ENGINE_EXPORT DefaultClockManagerFactory final
    : public ClockManagerFactory
{
  ISCORE_CONCRETE("583e9c52-e136-46b6-852f-7eef2993e9eb")

public:
  virtual ~DefaultClockManagerFactory();
  QString prettyName() const override;
  std::unique_ptr<ClockManager>
  make(const Engine::Execution::Context& ctx) override;
};
}
}

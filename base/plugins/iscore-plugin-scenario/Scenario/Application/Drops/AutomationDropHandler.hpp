#pragma once
#include <Scenario/Application/Drops/ScenarioDropHandler.hpp>

namespace Scenario
{

class DropProcessInScenario final : public DropHandler
{
  ISCORE_CONCRETE("9a094988-b05f-4e10-8e0d-56e8d46e084d")

  bool drop(
      const Scenario::TemporalScenarioPresenter&,
      QPointF pos,
      const QMimeData* mime) override;
};

/**
 * @brief The ProcessDropHandler class
 * Will create a blank process.
 */
class DropProcessInConstraint final : public ConstraintDropHandler
{
  ISCORE_CONCRETE("08f5aec5-3a42-45c8-b3db-aa45a851dd09")

  bool
  drop(const Scenario::ConstraintModel&, const QMimeData* mime) override;
};

/**
 * @brief The AutomationDropHandler class
 * Will create an automation where the addresses are dropped.
 */
class AutomationDropHandler final : public ConstraintDropHandler
{
  ISCORE_CONCRETE("851c98e1-4bcb-407b-9a72-8288d83c9f38")

  bool
  drop(const Scenario::ConstraintModel&, const QMimeData* mime) override;
};
}

#pragma once
#include <Scenario/Inspector/Constraint/ConstraintInspectorDelegate.hpp>
#include <list>

#include <Process/ExpandMode.hpp>
#include <Process/TimeValue.hpp>

class OngoingCommandDispatcher;
class QWidget;

namespace Scenario
{
class TriggerInspectorWidget;
class ConstraintInspectorWidget;
class ConstraintModel;

class BaseConstraintInspectorDelegate final
    : public ConstraintInspectorDelegate
{
public:
  BaseConstraintInspectorDelegate(const ConstraintModel& cst);

  void updateElements() override;
  void addWidgets_pre(
      std::list<QWidget*>& widgets,
      ConstraintInspectorWidget* parent) override;
  void addWidgets_post(
      std::list<QWidget*>& widgets,
      ConstraintInspectorWidget* parent) override;

  void on_defaultDurationChanged(
      OngoingCommandDispatcher& dispatcher,
      const TimeVal& val,
      ExpandMode) const override;

private:
  TriggerInspectorWidget* m_triggerLine{};
};
}

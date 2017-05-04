#include "ScenarioInspectorWidgetFactoryWrapper.hpp"

#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>

#include <Scenario/Inspector/Constraint/ConstraintInspectorFactory.hpp>
#include <Scenario/Inspector/Summary/SummaryInspectorWidget.hpp>
#include <Scenario/Inspector/TimeNode/TimeNodeInspectorWidget.hpp>

namespace Scenario
{
Inspector::InspectorWidgetBase*
ScenarioInspectorWidgetFactoryWrapper::makeWidget(
    const QList<const QObject*>& sourceElements,
    const iscore::DocumentContext& doc,
    QWidget* parent) const
{
  std::set<const ConstraintModel*> constraints;
  std::set<const TimeNodeModel*> timenodes;
  std::set<const EventModel*> events;
  std::set<const StateModel*> states;

  if (sourceElements.empty())
    return nullptr;

  auto scenar = dynamic_cast<ScenarioInterface*>(sourceElements[0]->parent());
  auto abstr = safe_cast<const IdentifiedObjectAbstract*>(sourceElements[0]);
  ISCORE_ASSERT(scenar); // because else, matches should have return false

  for (auto elt : sourceElements)
  {
    if (auto st = dynamic_cast<const StateModel*>(elt))
    {
      if (auto ev = scenar->findEvent(st->eventId()))
      {
        auto tn = scenar->findTimeNode(ev->timeNode());
        if (!tn)
          continue;
        states.insert(st);
        events.insert(ev);
        timenodes.insert(tn);
      }
    }
    else if (auto ev = dynamic_cast<const EventModel*>(elt))
    {
      auto tn = scenar->findTimeNode(ev->timeNode());
      if (!tn)
        continue;
      events.insert(ev);
      timenodes.insert(tn);
    }
    else if (auto tn = dynamic_cast<const TimeNodeModel*>(elt))
    {
      timenodes.insert(tn);
    }
    else if (auto cstr = dynamic_cast<const ConstraintModel*>(elt))
    {
      constraints.insert(cstr);
    }
  }

  if (timenodes.size() == 1 && constraints.empty())
    return new TimeNodeInspectorWidget{**timenodes.begin(), doc, parent};

  if (constraints.size() == 1 && timenodes.empty())
  {
    return ConstraintInspectorFactory{}.makeWidget(
        {*constraints.begin()}, doc, parent);
  }

  return new SummaryInspectorWidget{
      abstr, std::move(constraints), std::move(timenodes), std::move(events),
      std::move(states), doc, parent}; // the default InspectorWidgetBase need
                                       // an only IdentifiedObject : this will
                                       // be "abstr"
}

bool ScenarioInspectorWidgetFactoryWrapper::matches(
    const QList<const QObject*>& objects) const
{
  return std::any_of(objects.begin(), objects.end(), [](const QObject* obj) {
    return dynamic_cast<const StateModel*>(obj)
           || dynamic_cast<const EventModel*>(obj)
           || dynamic_cast<const TimeNodeModel*>(obj)
           || dynamic_cast<const ConstraintModel*>(obj);
  });
}
}

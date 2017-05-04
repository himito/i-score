#include <QString>
#include <Scenario/Document/Event/EventModel.hpp>

#include "EventInspectorFactory.hpp"
#include "EventInspectorWidget.hpp"

class QObject;
class QWidget;
namespace Scenario
{
Inspector::InspectorWidgetBase* EventInspectorFactory::makeWidget(
    const QList<const QObject*>& sourceElements,
    const iscore::DocumentContext& doc,
    QWidget* parentWidget) const
{
  // TODO !!
  return new Inspector::InspectorWidgetBase{
      static_cast<const EventModel&>(*sourceElements.first()), doc,
      parentWidget};
}

bool EventInspectorFactory::matches(const QList<const QObject*>& objects) const
{
  return dynamic_cast<const EventModel*>(objects.first());
}
}

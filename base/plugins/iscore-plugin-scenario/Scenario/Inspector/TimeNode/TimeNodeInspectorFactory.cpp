#include <QString>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

#include "TimeNodeInspectorFactory.hpp"
#include "TimeNodeInspectorWidget.hpp"

namespace Scenario
{
Inspector::InspectorWidgetBase* TimeNodeInspectorFactory::makeWidget(
    const QList<const QObject*>& sourceElements,
    const iscore::DocumentContext& doc,
    QWidget* parent) const
{
  auto& timeNode = static_cast<const TimeNodeModel&>(*sourceElements.first());
  return new TimeNodeInspectorWidget{timeNode, doc, parent};
}

bool TimeNodeInspectorFactory::matches(
    const QList<const QObject*>& objects) const
{
  return dynamic_cast<const TimeNodeModel*>(objects.first());
}
}

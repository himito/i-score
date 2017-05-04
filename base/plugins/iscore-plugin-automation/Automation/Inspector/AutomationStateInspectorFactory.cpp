#include <QString>

#include "AutomationStateInspector.hpp"
#include "AutomationStateInspectorFactory.hpp"
#include <Automation/State/AutomationState.hpp>
#include <Inspector/InspectorWidgetFactoryInterface.hpp>

namespace Automation
{
StateInspectorFactory::StateInspectorFactory() : InspectorWidgetFactory{}
{
}

Inspector::InspectorWidgetBase* StateInspectorFactory::makeWidget(
    const QList<const QObject*>& sourceElements,
    const iscore::DocumentContext& doc,
    QWidget* parent) const
{
  return new StateInspectorWidget{
      safe_cast<const ProcessState&>(*sourceElements.first()), doc, parent};
}

bool StateInspectorFactory::matches(const QList<const QObject*>& objects) const
{
  return dynamic_cast<const ProcessState*>(objects.first());
}
}

#include <Scenario/Process/ScenarioModel.hpp>
#include <QString>

#include <Scenario/Process/ScenarioProcessMetadata.hpp>
#include "ScenarioInspectorFactory.hpp"
#include "ScenarioInspectorWidget.hpp"

class InspectorWidgetBase;
class QObject;
class QWidget;
namespace iscore {
class Document;
}  // namespace iscore

Process::InspectorWidgetDelegate* ScenarioInspectorFactory::make(
        const Process::ProcessModel& process,
        const iscore::DocumentContext&,
        QWidget* parent) const
{
    return new ScenarioInspectorWidget{
        static_cast<const Scenario::ProcessModel&>(process),
                parent};
}

bool ScenarioInspectorFactory::matches(const Process::ProcessModel& process) const
{
    return dynamic_cast<const Scenario::ProcessModel*>(&process);
}

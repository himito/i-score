#include <Loop/LoopProcessModel.hpp>

#include "LoopInspectorWidget.hpp"
#include <QVBoxLayout>

#include <Inspector/InspectorWidgetList.hpp>
#include <Process/ProcessList.hpp>
#include <Scenario/Inspector/Constraint/ConstraintInspectorDelegateFactory.hpp>
#include <Scenario/Inspector/Constraint/ConstraintInspectorWidget.hpp>

#include <iscore/application/ApplicationContext.hpp>
#include <iscore/document/DocumentContext.hpp>
LoopInspectorWidget::LoopInspectorWidget(
    const Loop::ProcessModel& object,
    const iscore::DocumentContext& doc,
    QWidget* parent)
    : InspectorWidgetDelegate_T{object, parent}
{
  // FIXME URGENT add implemented virtual destructors to every class that
  // inherits from a virtual.
  auto& appContext = doc.app;
  auto& constraintWidgetFactory
      = appContext.interfaces<Scenario::ConstraintInspectorDelegateFactoryList>();

  auto& constraint = object.constraint();

  auto delegate = constraintWidgetFactory.make(
      &Scenario::ConstraintInspectorDelegateFactory::make, constraint);
  if (!delegate)
    return;

  auto lay = new QVBoxLayout;
  this->setLayout(lay);
  auto& widgetFact = appContext.interfaces<Inspector::InspectorWidgetList>();
  auto& processFact = appContext.interfaces<Process::ProcessFactoryList>();
  lay->addWidget(new Scenario::ConstraintInspectorWidget{
      widgetFact, processFact, constraint, std::move(delegate), doc, this});
}

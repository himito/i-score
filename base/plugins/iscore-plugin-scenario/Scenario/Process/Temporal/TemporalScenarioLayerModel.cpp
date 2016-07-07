#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/Temporal/TemporalConstraintViewModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <QVector>

#include <Process/LayerModel.hpp>
#include "TemporalScenarioLayerModel.hpp"
#include "TemporalScenarioPanelProxy.hpp"
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

namespace Process {
class LayerModelPanelProxy;
}
class QObject;

namespace Scenario
{
class ConstraintViewModel;
TemporalScenarioLayerModel::TemporalScenarioLayerModel(
        const Id<LayerModel>& viewModelId,
        const QMap<Id<ConstraintModel>, Id<ConstraintViewModel> >& constraintIds,
        Scenario::ProcessModel& model,
        QObject* parent) :
    AbstractScenarioLayerModel {viewModelId,
                              "TemporalScenarioLayer",
                              model,
                              parent}
{
    for(auto& key : constraintIds.keys())
    {
        makeConstraintViewModel(key, constraintIds.value(key));
    }
}

TemporalScenarioLayerModel::TemporalScenarioLayerModel(
        const TemporalScenarioLayerModel& source,
        const Id<LayerModel>& id,
        Scenario::ProcessModel& newScenario,
        QObject* parent) :
    AbstractScenarioLayerModel {source,
                              id,
                              "TemporalScenarioLayer",
                              newScenario,
                              parent
}
{
    for(TemporalConstraintViewModel* src_constraint : constraintsViewModels(source))
    {
        // OPTMIZEME (inside addConstraintViewModel)
        addConstraintViewModel(
                    src_constraint->clone(
                        src_constraint->id(),
                        newScenario.constraints.at(Id<ConstraintModel>{src_constraint->model().id()}),
                        this));
    }
}

Process::LayerModelPanelProxy* TemporalScenarioLayerModel::make_panelProxy(QObject* parent) const
{
    return new TemporalScenarioPanelProxy{*this, parent};
}


void TemporalScenarioLayerModel::makeConstraintViewModel(
        const Id<ConstraintModel>& constraintModelId,
        const Id<ConstraintViewModel>& constraintViewModelId)
{
    auto& constraint_model = model(*this).constraint(constraintModelId);

    auto constraint_view_model =
        constraint_model.makeConstraintViewModel<constraint_layer_type> (
            constraintViewModelId,
            this);

    addConstraintViewModel(constraint_view_model);
}

void TemporalScenarioLayerModel::addConstraintViewModel(constraint_layer_type* constraint_view_model)
{
    m_constraints.push_back(constraint_view_model);

    emit constraintViewModelCreated(*constraint_view_model);
}

void TemporalScenarioLayerModel::on_constraintRemoved(const ConstraintModel& cstr)
{
    auto cvms = constraintsViewModels(*this);
    for(auto& constraint_view_model : cvms)
    {
        if(&constraint_view_model->model() == &cstr)
        {
            removeConstraintViewModel(constraint_view_model->id());
            return;
        }
    }
}
}

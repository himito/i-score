
#include <Loop/LoopPanelProxy.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/Temporal/TemporalConstraintViewModel.hpp>
#include <iscore/tools/std/Optional.hpp>


#include "LoopLayer.hpp"
#include <Process/LayerModel.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

namespace Scenario
{
class ConstraintViewModel;
}
namespace Process { class LayerModelPanelProxy; }

class QObject;

namespace Loop
{
Layer::Layer(
        Loop::ProcessModel& model,
        const Id<LayerModel>& id,
        QObject* parent) :
    LayerModel {id, Metadata<ObjectKey_k, Layer>::get(), model, parent}
{
    m_constraint = model.constraint().makeConstraintViewModel<Scenario::TemporalConstraintViewModel>(
                Id<Scenario::ConstraintViewModel>{0},
                this);
}

Layer::Layer(
        const Layer& source,
        Loop::ProcessModel& model,
        const Id<LayerModel>& id,
        QObject* parent) :
    LayerModel {id, Metadata<ObjectKey_k, Layer>::get(), model, parent}
{
    m_constraint = source.m_constraint->clone(
                source.constraint().id(),
                model.constraint(),
                this);
}

Process::LayerModelPanelProxy* Layer::make_panelProxy(
        QObject* parent) const
{
    return new PanelProxy{*this, parent};
}


const Loop::ProcessModel& Layer::model() const
{
    return static_cast<const Loop::ProcessModel&>(processModel());
}

}

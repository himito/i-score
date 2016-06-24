#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModelSerialization.hpp>
#include <Scenario/Process/Temporal/TemporalScenarioLayerModel.hpp>
#include <QByteArray>
#include <QDebug>
#include <QPair>
#include <algorithm>
#include <iterator>
#include <stdexcept>

#include <Process/Process.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModelIdMap.hpp>
#include <Scenario/Process/AbstractScenarioLayerModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include "TemporalConstraintViewModel.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ObjectPath.hpp>

template <typename T> class Reader;

template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Reader<DataStream>>::readFrom(const Scenario::TemporalConstraintViewModel& constraint)
{
    readFrom(static_cast<const Scenario::ConstraintViewModel&>(constraint));
}

template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Reader<JSONObject>>::readFrom(const Scenario::TemporalConstraintViewModel& constraint)
{
    readFrom(static_cast<const Scenario::ConstraintViewModel&>(constraint));
}
namespace Scenario
{
ISCORE_PLUGIN_SCENARIO_EXPORT SerializedConstraintViewModels serializeConstraintViewModels(
        const ConstraintModel& constraint,
        const Scenario::ScenarioModel& scenario)
{
    SerializedConstraintViewModels map;
    // The other constraint view models are in their respective scenario view models
    for(const auto& viewModel : layers(scenario))
    {
        const auto& cstrVM = viewModel->constraint(constraint.id());

        auto lm_id = iscore::IDocument::path(*viewModel);
        QByteArray arr;

        if(const auto& temporalCstrVM = dynamic_cast<const Scenario::TemporalConstraintViewModel*>(&cstrVM))
        {
            Serializer<DataStream> cvmReader{&arr};
            cvmReader.readFrom(*temporalCstrVM);
        }
        else
        {
            ISCORE_TODO;
        }

        map.append({lm_id, {cstrVM.type(), arr}});
    }

    return map;
}


ISCORE_PLUGIN_SCENARIO_EXPORT void deserializeConstraintViewModels(
        const SerializedConstraintViewModels& vms,
        const Scenario::ScenarioModel& scenar)
{
    using namespace std;
    for(auto& viewModel : layers(scenar))
    {
        if(TemporalScenarioLayerModel* temporalSVM = dynamic_cast<TemporalScenarioLayerModel*>(viewModel))
        {
            auto svm_id = iscore::IDocument::path(static_cast<const AbstractScenarioLayerModel&>(*temporalSVM));

            auto it = std::find_if(begin(vms), end(vms),
                          [&] (const auto& elt) { return elt.first == svm_id; });
            if(it != end(vms))
            {
                Deserializer<DataStream> d{((*it).second.second)};
                auto cvm = loadConstraintViewModel(d, temporalSVM);
                temporalSVM->addConstraintViewModel(cvm);
            }
            else
            {
                throw runtime_error("undo RemoveConstraint : missing identifier.");
            }
        }
        else
        {
            ISCORE_TODO;
        }
    }
}
}

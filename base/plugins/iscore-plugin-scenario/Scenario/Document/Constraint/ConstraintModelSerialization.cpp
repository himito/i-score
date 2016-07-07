#include <Process/Process.hpp>

#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/Rack/RackModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/FullView/FullViewConstraintViewModel.hpp>

#include <iscore/tools/std/Optional.hpp>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <sys/types.h>
#include <algorithm>

#include <Process/ModelMetadata.hpp>
#include <Process/ProcessList.hpp>
#include <Process/TimeValue.hpp>
#include <iscore/application/ApplicationContext.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

namespace Scenario
{
class StateModel;
}

template <typename T> class IdentifiedObject;
template <typename T> class Reader;
template <typename T> class Writer;

// Note : comment gérer le cas d'un process shared model qui ne sait se sérializer qu'en binaire, dans du json?
// Faire passer l'info en base64 ?
template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Reader<DataStream>>::readFrom(const Scenario::ConstraintModel& constraint)
{
    readFrom(static_cast<const IdentifiedObject<Scenario::ConstraintModel>&>(constraint));

    // Metadata
    readFrom(constraint.metadata);

    // Processes
    m_stream << (int32_t) constraint.processes.size();
    for(const auto& process : constraint.processes)
    {
        readFrom(process);
    }

    // Rackes
    m_stream << (int32_t) constraint.racks.size();
    for(const auto& rack : constraint.racks)
    {
        readFrom(rack);
    }

    // Full view
    readFrom(*constraint.m_fullViewModel);

    // Common data
    m_stream << constraint.duration
             << constraint.m_startState
             << constraint.m_endState

             << constraint.m_startDate
             << constraint.m_heightPercentage
             << constraint.m_looping;

    readFrom(constraint.pluginModelList);

    insertDelimiter();
}

template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Writer<DataStream>>::writeTo(Scenario::ConstraintModel& constraint)
{
    writeTo(constraint.metadata);

    // Processes
    int32_t process_count;
    m_stream >> process_count;

    auto& pl = context.components.factory<Process::ProcessList>();
    for(; process_count -- > 0;)
    {
        constraint.processes.add(deserialize_interface(pl, *this, &constraint));
    }

    // Rackes
    int32_t rack_count;
    m_stream >> rack_count;

    for(; rack_count -- > 0;)
    {
        constraint.racks.add(new Scenario::RackModel(*this, &constraint));
    }

    // Full view
    Id<Scenario::ConstraintModel> savedConstraintId;
    m_stream >> savedConstraintId; // Necessary because it is saved; however it is not required here.
    //(todo how to fix this ?)
    constraint.setFullView(new Scenario::FullViewConstraintViewModel {*this, constraint, &constraint});

    // Common data
    m_stream >> constraint.duration
             >> constraint.m_startState
             >> constraint.m_endState

             >> constraint.m_startDate
             >> constraint.m_heightPercentage
             >> constraint.m_looping;


    constraint.pluginModelList = iscore::ElementPluginModelList{*this, &constraint};

    checkDelimiter();
}



template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Reader<JSONObject>>::readFrom(const Scenario::ConstraintModel& constraint)
{
    readFrom(static_cast<const IdentifiedObject<Scenario::ConstraintModel>&>(constraint));
    m_obj["Metadata"] = toJsonObject(constraint.metadata);

    // Processes
    m_obj["Processes"] = toJsonArray(constraint.processes);

    // Rackes
    m_obj["Rackes"] = toJsonArray(constraint.racks);

    // Full view
    m_obj["FullView"] = toJsonObject(*constraint.fullView());


    // Common data
    // The fields will go in the same level as the
    // rest of the constraint
    readFrom(constraint.duration);

    m_obj["StartState"] = toJsonValue(constraint.m_startState);
    m_obj["EndState"] = toJsonValue(constraint.m_endState);

    m_obj["StartDate"] = toJsonValue(constraint.m_startDate);
    m_obj["HeightPercentage"] = constraint.m_heightPercentage;
    m_obj["Looping"] = constraint.m_looping;

    m_obj["PluginsMetadata"] = toJsonValue(constraint.pluginModelList);
}

template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Writer<JSONObject>>::writeTo(Scenario::ConstraintModel& constraint)
{
    constraint.metadata = fromJsonObject<ModelMetadata>(m_obj["Metadata"]);

    auto& pl = context.components.factory<Process::ProcessList>();

    QJsonArray process_array = m_obj["Processes"].toArray();
    for(const auto& json_vref : process_array)
    {
        Deserializer<JSONObject> deserializer{json_vref.toObject()};
        auto proc = deserialize_interface(pl, deserializer, &constraint);
        if(proc)
            constraint.processes.add(proc);
    }

    QJsonArray rack_array = m_obj["Rackes"].toArray();
    for(const auto& json_vref : rack_array)
    {
        Deserializer<JSONObject> deserializer {json_vref.toObject() };
        constraint.racks.add(new Scenario::RackModel(deserializer, &constraint));
    }

    constraint.setFullView(new Scenario::FullViewConstraintViewModel {
                               Deserializer<JSONObject>{m_obj["FullView"].toObject() },
                               constraint,
                               &constraint});

    writeTo(constraint.duration);
    constraint.m_startState = fromJsonValue<Id<Scenario::StateModel>> (m_obj["StartState"]);
    constraint.m_endState = fromJsonValue<Id<Scenario::StateModel>> (m_obj["EndState"]);

    constraint.m_startDate = fromJsonValue<TimeValue> (m_obj["StartDate"]);
    constraint.m_heightPercentage = m_obj["HeightPercentage"].toDouble();
    constraint.m_looping = m_obj["Looping"].toBool();



    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    constraint.pluginModelList = iscore::ElementPluginModelList{elementPluginDeserializer, &constraint};
}

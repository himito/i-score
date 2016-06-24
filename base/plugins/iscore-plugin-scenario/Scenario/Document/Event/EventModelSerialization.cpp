
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <QDataStream>
#include <QtGlobal>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <algorithm>

#include <Process/ModelMetadata.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <State/Expression.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/TreeNode.hpp>

namespace Scenario
{
class TimeNodeModel;
}
template <typename T> class IdentifiedObject;
template <typename T> class Reader;
template <typename T> class Writer;


template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Reader<DataStream>>::readFrom(const Scenario::EventModel& ev)
{
    readFrom(static_cast<const IdentifiedObject<Scenario::EventModel>&>(ev));

    readFrom(ev.metadata);
    readFrom(ev.pluginModelList);

    m_stream << ev.m_timeNode
             << ev.m_states
             << ev.m_condition
             << ev.m_extent
             << ev.m_date;

    insertDelimiter();
}

template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Writer<DataStream>>::writeTo(Scenario::EventModel& ev)
{
    writeTo(ev.metadata);
    ev.pluginModelList = iscore::ElementPluginModelList(*this, &ev);

    m_stream >> ev.m_timeNode
             >> ev.m_states
             >> ev.m_condition
             >> ev.m_extent
             >> ev.m_date;

    checkDelimiter();
}




template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Reader<JSONObject>>::readFrom(const Scenario::EventModel& ev)
{
    readFrom(static_cast<const IdentifiedObject<Scenario::EventModel>&>(ev));
    m_obj["Metadata"] = toJsonObject(ev.metadata);

    m_obj["TimeNode"] = toJsonValue(ev.m_timeNode);
    m_obj["States"] = toJsonArray(ev.m_states);

    m_obj["Condition"] = toJsonObject(ev.m_condition);

    m_obj["Extent"] = toJsonValue(ev.m_extent);
    m_obj["Date"] = toJsonValue(ev.m_date);

    m_obj["PluginsMetadata"] = toJsonValue(ev.pluginModelList);
}

template<>
ISCORE_PLUGIN_SCENARIO_EXPORT void Visitor<Writer<JSONObject>>::writeTo(Scenario::EventModel& ev)
{
    ev.metadata = fromJsonObject<ModelMetadata>(m_obj["Metadata"]);

    ev.m_timeNode = fromJsonValue<Id<Scenario::TimeNodeModel>> (m_obj["TimeNode"]);
    fromJsonValueArray(m_obj["States"].toArray(), ev.m_states);

    fromJsonObject(m_obj["Condition"], ev.m_condition);

    ev.m_extent = fromJsonValue<Scenario::VerticalExtent>(m_obj["Extent"]);
    ev.m_date = fromJsonValue<TimeValue>(m_obj["Date"]);

    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    ev.pluginModelList = iscore::ElementPluginModelList(elementPluginDeserializer, &ev);
}

#pragma once
#include <iscore/tools/Metadata.hpp>
#include <Process/ModelMetadata.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <iscore/selection/Selectable.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <QObject>

#include <QString>
#include <QVector>
#include <chrono>
#include <iscore_plugin_scenario_export.h>
#include <iscore/component/Component.hpp>
class DataStream;
class JSONObject;

namespace Scenario
{
class EventModel;
class ScenarioInterface;
class TriggerModel;

class ISCORE_PLUGIN_SCENARIO_EXPORT TimeNodeModel final : public IdentifiedObject<TimeNodeModel>
{
        Q_OBJECT

        ISCORE_SERIALIZE_FRIENDS(Scenario::TimeNodeModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Scenario::TimeNodeModel, JSONObject)

    public:
        /** Properties of the class **/
        iscore::Components components;
        Selectable selection;
        ModelMetadata metadata;
        iscore::ElementPluginModelList pluginModelList;

        /** The class **/
        TimeNodeModel(
                const Id<TimeNodeModel>& id,
                const VerticalExtent& extent,
                const TimeValue& date,
                QObject* parent);

        template<typename DeserializerVisitor>
        TimeNodeModel(DeserializerVisitor&& vis, QObject* parent) :
            IdentifiedObject{vis, parent}
        {
            vis.writeTo(*this);
        }

        TimeNodeModel(
                const TimeNodeModel& source,
                const Id<TimeNodeModel>& id,
                QObject* parent);

        // Data of the TimeNode
        const VerticalExtent& extent() const;
        void setExtent(const VerticalExtent &extent);

        const TimeValue& date() const;
        void setDate(const TimeValue&);

        void addEvent(const Id<EventModel>&);
        bool removeEvent(const Id<EventModel>&);
        const QVector<Id<EventModel>>& events() const;
        void setEvents(const QVector<Id<EventModel>>& events);

        Scenario::TriggerModel* trigger() const;
        QString expression() const;

        bool hasTrigger() const;

    signals:
        void extentChanged(const VerticalExtent&);
        void dateChanged(const TimeValue&);

        void newEvent(const Id<EventModel>& eventId);
        void eventRemoved(const Id<EventModel>& eventId);

    private:
        VerticalExtent m_extent;
        TimeValue m_date{std::chrono::seconds{0}};
        TriggerModel* m_trigger;

        QVector<Id<EventModel>> m_events;
};
}

DEFAULT_MODEL_METADATA(Scenario::TimeNodeModel, "Time Node")

#include <Process/Style/ScenarioStyle.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
#include <QtGlobal>

#include <Process/ModelMetadata.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>
#include "TimeNodeModel.hpp"
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

namespace Scenario
{
TimeNodeModel::TimeNodeModel(
        const Id<TimeNodeModel>& id,
        const VerticalExtent& extent,
        const TimeValue& date,
        QObject* parent):
    IdentifiedObject<TimeNodeModel> {id, Metadata<ObjectKey_k, TimeNodeModel>::get(), parent},
    pluginModelList{iscore::IDocument::documentContext(*parent), this},
    m_extent{extent},
    m_date{date},
    m_trigger{new TriggerModel{Id<TriggerModel>(0), this} }
{
    metadata.setName(QString("TimeNode.%1").arg(*this->id().val()));
    metadata.setLabel("TimeNode");
    metadata.setColor(ScenarioStyle::instance().TimenodeDefault);
}

TimeNodeModel::TimeNodeModel(
        const TimeNodeModel &source,
        const Id<TimeNodeModel>& id,
        QObject* parent):
    IdentifiedObject<TimeNodeModel> {id, Metadata<ObjectKey_k, TimeNodeModel>::get(), parent},
    metadata{source.metadata},
    pluginModelList{source.pluginModelList, this},
    m_extent{source.m_extent},
    m_date{source.m_date},
    m_events(source.m_events)
{
    m_trigger = new TriggerModel{Id<TriggerModel>(0), this};
    m_trigger->setExpression(source.trigger()->expression());
    m_trigger->setActive(source.trigger()->active());
}

void TimeNodeModel::addEvent(const Id<EventModel>& eventId)
{
    m_events.push_back(eventId);
    emit newEvent(eventId);

    auto scenar = dynamic_cast<ScenarioInterface*>(parent());
    if(scenar)
    {
        // There may be no scenario when we are cloning without a parent.
        // TODO this addEvent should be in an outside algorithm.
        auto& theEvent = scenar->event(eventId);
        theEvent.changeTimeNode(this->id());
    }
}

bool TimeNodeModel::removeEvent(const Id<EventModel>& eventId)
{
    if(m_events.indexOf(eventId) >= 0)
    {
        m_events.remove(m_events.indexOf(eventId));
        emit eventRemoved(eventId);
        return true;
    }

    return false;
}

const TimeValue& TimeNodeModel::date() const
{
    return m_date;
}

void TimeNodeModel::setDate(const TimeValue& date)
{
    m_date = date;
    emit dateChanged(m_date);
}

const QVector<Id<EventModel> >& TimeNodeModel::events() const
{
    return m_events;
}

void TimeNodeModel::setEvents(const QVector<Id<EventModel>>& events)
{
    m_events = events;
}

TriggerModel* TimeNodeModel::trigger() const
{
    return m_trigger;
}

QString TimeNodeModel::expression() const
{
    return m_trigger->expression().toString();
}

const VerticalExtent& TimeNodeModel::extent() const
{
    return m_extent;
}

void TimeNodeModel::setExtent(const VerticalExtent &extent)
{
    // TODO if extent != ...
    m_extent = extent;
    emit extentChanged(m_extent);
}

bool TimeNodeModel::hasTrigger() const
{
    return m_trigger->active();
}

}

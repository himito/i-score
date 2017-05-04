#include <Process/Style/ScenarioStyle.hpp>
#include <QObject>
#include <iscore/document/DocumentInterface.hpp>

#include <QPoint>

#include "EventModel.hpp"
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Event/ExecutionStatus.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>
#include <State/Expression.hpp>
#include <iscore/model/ModelMetadata.hpp>
#include <iscore/model/IdentifiedObject.hpp>
#include <iscore/model/Identifier.hpp>

namespace Scenario
{
EventModel::EventModel(
    const Id<EventModel>& id,
    const Id<TimeNodeModel>& timenode,
    const VerticalExtent& extent,
    const TimeVal& date,
    QObject* parent)
    : Entity{id, Metadata<ObjectKey_k, EventModel>::get(), parent}
    , m_timeNode{timenode}
    , m_condition{}
    , m_extent{extent}
    , m_date{date}
    , m_offset{OffsetBehavior::True}
{
  metadata().setInstanceName(*this);
  metadata().setColor(ScenarioStyle::instance().EventDefault);
}

EventModel::EventModel(
    const EventModel& source, const Id<EventModel>& id, QObject* parent)
    : Entity{source, id, Metadata<ObjectKey_k, EventModel>::get(), parent}
    , m_timeNode{source.m_timeNode}
    , m_states(source.m_states)
    , m_condition{source.m_condition}
    , m_extent{source.m_extent}
    , m_date{source.m_date}
    , m_offset{source.m_offset}
{
  metadata().setInstanceName(*this);
}

VerticalExtent EventModel::extent() const
{
  return m_extent;
}

void EventModel::setExtent(const VerticalExtent& extent)
{
  if (extent != m_extent)
  {
    m_extent = extent;
    emit extentChanged(m_extent);
  }
}

const TimeVal& EventModel::date() const
{
  return m_date;
}

void EventModel::setDate(const TimeVal& date)
{
  if (m_date != date)
  {
    m_date = date;
    emit dateChanged(m_date);
  }
}

void EventModel::setStatus(ExecutionStatus status)
{
  if (m_status.get() == status)
    return;

  m_status.set(status);
  emit statusChanged(status);

  auto scenar = dynamic_cast<ScenarioInterface*>(parent());
  ISCORE_ASSERT(scenar);

  for (auto& state : m_states)
  {
    scenar->state(state).setStatus(status);
  }
}

void EventModel::setOffsetBehavior(OffsetBehavior f)
{
  if (m_offset != f)
  {
    m_offset = f;
    emit offsetBehaviorChanged(f);
  }
}

void EventModel::translate(const TimeVal& deltaTime)
{
  setDate(m_date + deltaTime);
}

ExecutionStatus EventModel::status() const
{
  return m_status.get();
}

void EventModel::reset()
{
  setStatus(ExecutionStatus::Editing);
}

// TODO Maybe remove the need for this by passing to the scenario instead ?

void EventModel::addState(const Id<StateModel>& ds)
{
  auto idx = m_states.indexOf(ds, 0);
  if (idx != -1)
    return;
  m_states.append(ds);
  emit statesChanged();
}

void EventModel::removeState(const Id<StateModel>& ds)
{
  auto idx = m_states.indexOf(ds, 0);
  if (idx != -1)
  {
    m_states.remove(idx);
    emit statesChanged();
  }
}

const QVector<Id<StateModel>>& EventModel::states() const
{
  return m_states;
}

const State::Expression& EventModel::condition() const
{
  return m_condition;
}

OffsetBehavior EventModel::offsetBehavior() const
{
  return m_offset;
}

void EventModel::setCondition(const State::Expression& arg)
{
  if (m_condition != arg)
  {
    m_condition = arg;
    emit conditionChanged(arg);
  }
}
}

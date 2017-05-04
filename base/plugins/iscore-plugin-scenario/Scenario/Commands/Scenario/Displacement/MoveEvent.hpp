#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>

#include <Process/ExpandMode.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/Scenario/Displacement/SerializableMoveEvent.hpp>
#include <Scenario/Process/Algorithms/StandardDisplacementPolicy.hpp>
#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Tools/dataStructures.hpp>
#include <Scenario/Tools/elementFindingHelper.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/model/Identifier.hpp>

struct ElementsProperties;

#include <tests/helpers/ForwardDeclaration.hpp>

namespace Scenario
{
class EventModel;
class TimeNodeModel;
class ConstraintModel;
class ProcessModel;
namespace Command
{
/**
    * This class use the new Displacement policy class
    */
template <class DisplacementPolicy>
class MoveEvent final : public SerializableMoveEvent
{
  // No ISCORE_COMMAND here since it's a template.

public:
  const CommandGroupKey& parentKey() const noexcept override
  {
    return ScenarioCommandFactoryName();
  }
  const CommandKey& key() const noexcept override
  {
    static const QByteArray name
        = QString{"MoveEvent_%1"}.arg(DisplacementPolicy::name()).toLatin1();
    static const CommandKey kagi{name.constData()};
    return kagi;
  }
  QString description() const override
  {
    return QObject::tr("Move an event with %1")
        .arg(DisplacementPolicy::name());
  }

  MoveEvent() : SerializableMoveEvent{}
  {
  }
  /**
           * @brief MoveEvent
           * @param scenarioPath
           * @param eventId
           * @param newDate
           * !!! in the future it would be better to give directly the delta
   * time of the mouse displacement  !!!
           * @param mode
           */
  MoveEvent(
      Path<Scenario::ProcessModel>&& scenarioPath,
      const Id<EventModel>& eventId,
      const TimeVal& newDate,
      ExpandMode mode)
      : SerializableMoveEvent{}, m_path{std::move(scenarioPath)}, m_mode{mode}
  {
    auto& scenar = m_path.find();
    DisplacementPolicy::init(scenar, {scenar.event(eventId).timeNode()});
    // we need to compute the new time delta and store this initial event id
    // for recalculate the delta on updates
    // NOTE: in the future in would be better to give directly the delta value
    // to this method ?,
    // in that way we wouldn't need to keep the initial event and recalculate
    // the delta
    m_eventId = eventId;
    m_initialDate = getDate(scenar, eventId);

    update(eventId, newDate, 0, m_mode);
  }

  void update(
      const Id<EventModel>& eventId,
      const TimeVal& newDate,
      double,
      ExpandMode) override
  {
    // we need to compute the new time delta
    // NOTE: in the future in would be better to give directly the delta value
    // to this method
    TimeVal deltaDate = newDate - m_initialDate;

    auto& scenario = m_path.find();

    // NOTICE: multiple event displacement functionnality already available,
    // this is "retro" compatibility
    QVector<Id<TimeNodeModel>> draggedElements;
    draggedElements.push_back(
        scenario.events.at(eventId).timeNode()); // retrieve corresponding
                                                 // timenode and store it in
                                                 // array

    // the displacement is computed here and we don't need to know how.
    DisplacementPolicy::computeDisplacement(
        scenario, draggedElements, deltaDate, m_savedElementsProperties);
  }

  void undo() const override
  {
    auto& scenario = m_path.find();

    // update positions using old stored dates
    DisplacementPolicy::revertPositions(
        scenario,
        [&](Process::ProcessModel& p, const TimeVal& t) {
          p.setParentDuration(m_mode, t);
        },
        m_savedElementsProperties);

    updateEventExtent(m_eventId, scenario);
  }

  void redo() const override
  {
    auto& scenario = m_path.find();

    // update positions using new stored dates
    DisplacementPolicy::updatePositions(
        scenario,
        [&](Process::ProcessModel& p, const TimeVal& t) {
          p.setParentDuration(m_mode, t);
        },
        m_savedElementsProperties);

    updateEventExtent(m_eventId, scenario);
  }

  const Path<Scenario::ProcessModel>& path() const override
  {
    return m_path;
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_savedElementsProperties << m_path << m_eventId << m_initialDate
      << (int)m_mode;
  }

  void deserializeImpl(DataStreamOutput& s) override
  {
    int mode;
    s >> m_savedElementsProperties >> m_path >> m_eventId >> m_initialDate
        >> mode;

    m_mode = static_cast<ExpandMode>(mode);
  }

private:
  ElementsProperties m_savedElementsProperties;
  Path<Scenario::ProcessModel> m_path;

  ExpandMode m_mode{ExpandMode::Scale};

  Id<EventModel> m_eventId;
  /**
   * @brief m_initialDate
   * the delta will be calculated from the initial date
   */
  TimeVal
      m_initialDate; // used to compute the deltaTime and respect undo behavior
};
}
}

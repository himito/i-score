#pragma once

#include <Scenario/Commands/Scenario/Displacement/SerializableMoveEvent.hpp>

#include <Process/ExpandMode.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/model/path/Path.hpp>

#include <iscore/model/Identifier.hpp>
#include <iscore_plugin_scenario_export.h>
struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class EventModel;
class ProcessModel;
namespace Command
{

class ISCORE_PLUGIN_SCENARIO_EXPORT MoveEventOnCreationMeta final
    : public SerializableMoveEvent
{
  ISCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      MoveEventOnCreationMeta,
      "Move an event on creation")
public:
  MoveEventOnCreationMeta(
      Path<Scenario::ProcessModel>&& scenarioPath,
      Id<EventModel>
          eventId,
      TimeVal newDate,
      ExpandMode mode);
  ~MoveEventOnCreationMeta();

  void undo() const override;
  void redo() const override;

  const Path<Scenario::ProcessModel>& path() const override;

  // Command interface
protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  std::unique_ptr<SerializableMoveEvent> m_moveEventImplementation;

  // SerializableMoveEvent interface
public:
  void update(
      const Id<EventModel>& eventId,
      const TimeVal& newDate,
      double,
      ExpandMode mode) override;
};
}
}

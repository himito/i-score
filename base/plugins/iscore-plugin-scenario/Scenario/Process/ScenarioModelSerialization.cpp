#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <Scenario/Application/ScenarioValidity.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <algorithm>
#include <iscore/tools/std/Optional.hpp>
#include <sys/types.h>

#include "ScenarioFactory.hpp"
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Process/Algorithms/ProcessPolicy.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/model/ModelMetadata.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <iscore/model/EntityMap.hpp>
#include <iscore/model/Identifier.hpp>
#include <iscore_plugin_scenario_export.h>

namespace Process
{
class ProcessModel;
}
class QObject;
struct VisitorVariant;
template <typename T>
class Reader;
template <typename T>
class Writer;


template <>
void DataStreamReader::read(
    const Scenario::ProcessModel& scenario)
{
  m_stream << scenario.m_startTimeNodeId;
  m_stream << scenario.m_startEventId;
  m_stream << scenario.m_startStateId;

  // Constraints
  const auto& constraints = scenario.constraints;
  m_stream << (int32_t)constraints.size();

  for (const auto& constraint : constraints)
  {
    readFrom(constraint);
  }

  // Timenodes
  const auto& timenodes = scenario.timeNodes;
  m_stream << (int32_t)timenodes.size();

  for (const auto& timenode : timenodes)
  {
    readFrom(timenode);
  }

  // Events
  const auto& events = scenario.events;
  m_stream << (int32_t)events.size();

  for (const auto& event : events)
  {
    readFrom(event);
  }

  // States
  const auto& states = scenario.states;
  m_stream << (int32_t)states.size();

  for (const auto& state : states)
  {
    readFrom(state);
  }

  // Comments
  const auto& comments = scenario.comments;
  m_stream << (int32_t)comments.size();

  for (const auto& cmt : comments)
  {
    readFrom(cmt);
  }

  insertDelimiter();
}


template <>
void DataStreamWriter::write(Scenario::ProcessModel& scenario)
{
  m_stream >> scenario.m_startTimeNodeId;
  m_stream >> scenario.m_startEventId;
  m_stream >> scenario.m_startStateId;

  // Constraints
  int32_t constraint_count;
  m_stream >> constraint_count;

  for (; constraint_count-- > 0;)
  {
    auto constraint = new Scenario::ConstraintModel{*this, &scenario};
    scenario.constraints.add(constraint);
  }

  // Timenodes
  int32_t timenode_count;
  m_stream >> timenode_count;

  for (; timenode_count-- > 0;)
  {
    auto tnmodel = new Scenario::TimeNodeModel{*this, &scenario};
    scenario.timeNodes.add(tnmodel);
  }

  // Events
  int32_t event_count;
  m_stream >> event_count;

  for (; event_count-- > 0;)
  {
    auto evmodel = new Scenario::EventModel{*this, &scenario};
    scenario.events.add(evmodel);
  }

  // States
  int32_t state_count;
  m_stream >> state_count;

  auto& stack = iscore::IDocument::documentContext(scenario).commandStack;
  for (; state_count-- > 0;)
  {
    auto stmodel = new Scenario::StateModel{*this, stack, &scenario};
    scenario.states.add(stmodel);
  }

  int32_t cmt_count;
  m_stream >> cmt_count;

  for (; cmt_count-- > 0;)
  {
    auto cmtModel = new Scenario::CommentBlockModel{*this, &scenario};
    scenario.comments.add(cmtModel);
  }

  // Finally, we re-set the constraints before and after the states
  for (const Scenario::ConstraintModel& constraint : scenario.constraints)
  {
    Scenario::SetPreviousConstraint(
        scenario.states.at(constraint.endState()), constraint);
    Scenario::SetNextConstraint(
        scenario.states.at(constraint.startState()), constraint);
  }

  // Scenario::ScenarioValidityChecker::checkValidity(scenario);
  checkDelimiter();
}


template <>
void JSONObjectReader::read(
    const Scenario::ProcessModel& scenario)
{
  obj["StartTimeNodeId"] = toJsonValue(scenario.m_startTimeNodeId);
  obj["StartEventId"] = toJsonValue(scenario.m_startEventId);
  obj["StartStateId"] = toJsonValue(scenario.m_startStateId);

  obj["TimeNodes"] = toJsonArray(scenario.timeNodes);
  obj["Events"] = toJsonArray(scenario.events);
  obj["States"] = toJsonArray(scenario.states);
  obj["Constraints"] = toJsonArray(scenario.constraints);
  obj["Comments"] = toJsonArray(scenario.comments);
}


template <>
void JSONObjectWriter::write(Scenario::ProcessModel& scenario)
{
  scenario.m_startTimeNodeId
      = fromJsonValue<Id<Scenario::TimeNodeModel>>(obj["StartTimeNodeId"]);
  scenario.m_startEventId
      = fromJsonValue<Id<Scenario::EventModel>>(obj["StartEventId"]);
  scenario.m_startStateId
      = fromJsonValue<Id<Scenario::StateModel>>(obj["StartStateId"]);

  const auto& constraints = obj["Constraints"].toArray();
  for (const auto& json_vref : constraints)
  {
    auto constraint = new Scenario::ConstraintModel{
        JSONObject::Deserializer{json_vref.toObject()}, &scenario};
    scenario.constraints.add(constraint);
  }

  const auto& timenodes = obj["TimeNodes"].toArray();
  for (const auto& json_vref : timenodes)
  {
    auto tnmodel = new Scenario::TimeNodeModel{
        JSONObject::Deserializer{json_vref.toObject()}, &scenario};

    scenario.timeNodes.add(tnmodel);
  }

  const auto& events = obj["Events"].toArray();
  for (const auto& json_vref : events)
  {
    auto evmodel = new Scenario::EventModel{
        JSONObject::Deserializer{json_vref.toObject()}, &scenario};

    scenario.events.add(evmodel);
  }

  const auto& comments = obj["Comments"].toArray();
  for (const auto& json_vref : comments)
  {
    auto cmtmodel = new Scenario::CommentBlockModel{
        JSONObject::Deserializer{json_vref.toObject()}, &scenario};

    scenario.comments.add(cmtmodel);
  }

  auto& stack = iscore::IDocument::documentContext(scenario).commandStack;
  const auto& states = obj["States"].toArray();
  for (const auto& json_vref : states)
  {
    auto stmodel = new Scenario::StateModel{
        JSONObject::Deserializer{json_vref.toObject()}, stack, &scenario};

    scenario.states.add(stmodel);
  }

  // Finally, we re-set the constraints before and after the states
  for (const Scenario::ConstraintModel& constraint : scenario.constraints)
  {
    Scenario::SetPreviousConstraint(
        scenario.states.at(constraint.endState()), constraint);
    Scenario::SetNextConstraint(
        scenario.states.at(constraint.startState()), constraint);
  }

  // Scenario::ScenarioValidityChecker::checkValidity(scenario);
}

Process::ProcessModel*
Scenario::ScenarioFactory::load(const VisitorVariant& vis, QObject* parent)
{
  return deserialize_dyn(vis, [&](auto&& deserializer) {
    return new Scenario::ProcessModel{deserializer, parent};
  });
}

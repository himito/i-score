#pragma once

#include <Scenario/Commands/Scenario/Displacement/MoveEvent.hpp>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>

#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
#include <Scenario/Process/Algorithms/GoodOldDisplacementPolicy.hpp>
#include <Scenario/Process/Algorithms/StandardDisplacementPolicy.hpp>
#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <State/Expression.hpp>
#include <iscore/model/tree/TreeNode.hpp>

#include <iscore/command/Command.hpp>

#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/model/Identifier.hpp>

#include <iscore/serialization/DataStreamVisitor.hpp>

namespace Scenario
{

namespace Command
{
template <typename Scenario_T>
class ISCORE_PLUGIN_SCENARIO_EXPORT MergeTimeNodes final : std::false_type
{
};
template <>
class ISCORE_PLUGIN_SCENARIO_EXPORT MergeTimeNodes<ProcessModel> final
    : public iscore::Command
{
  // No ISCORE_COMMAND here since it's a template.
public:
  const CommandGroupKey& parentKey() const noexcept override
  {
    return ScenarioCommandFactoryName();
  }
  static const CommandKey& static_key() noexcept
  {
    auto name = QString("MergeTimeNodes");
    static const CommandKey kagi{std::move(name)};
    return kagi;
  }
  const CommandKey& key() const noexcept override
  {
    return static_key();
  }
  QString description() const noexcept override
  {
    return QObject::tr("Merging TimeNodes");
  }

  MergeTimeNodes() = default;

  MergeTimeNodes(
      Path<ProcessModel>&& scenar,
      Id<TimeNodeModel>
          clickedTn,
      Id<TimeNodeModel>
          hoveredTn)
      : m_scenarioPath{scenar}
      , m_movingTnId{std::move(clickedTn)}
      , m_destinationTnId{std::move(hoveredTn)}
  {
    auto& scenario = m_scenarioPath.find();
    auto& tn = scenario.timeNode(m_movingTnId);
    auto& destinantionTn = scenario.timeNode(m_destinationTnId);

    QByteArray arr;
    DataStream::Serializer s{&arr};
    s.readFrom(tn);
    m_serializedTimeNode = arr;

    m_moveCommand = new MoveEvent<GoodOldDisplacementPolicy>{
        Path<ProcessModel>{scenario}, tn.events().front(),
        destinantionTn.date(), ExpandMode::Scale};

    m_targetTrigger = destinantionTn.trigger()->expression();
    m_targetTriggerActive = destinantionTn.trigger()->active();
  }

  void undo() const override
  {
    auto& scenar = m_scenarioPath.find();
    auto& globalTn = scenar.timeNode(m_destinationTnId);

    DataStream::Deserializer s{m_serializedTimeNode};
    auto recreatedTn = new TimeNodeModel{s, &scenar};

    auto events_in_timenode = recreatedTn->events();
    // we remove and re-add events in recreated Tn
    // to ensure correct parentship between elements.
    for (auto evId : events_in_timenode)
    {
      recreatedTn->removeEvent(evId);
      globalTn.removeEvent(evId);
    }
    for (auto evId : events_in_timenode)
    {
      recreatedTn->addEvent(evId);
    }

    scenar.timeNodes.add(recreatedTn);

    globalTn.trigger()->setExpression(m_targetTrigger);
    globalTn.trigger()->setActive(m_targetTriggerActive);

    m_moveCommand->undo();
    updateTimeNodeExtent(m_destinationTnId, scenar);
  }
  void redo() const override
  {
    m_moveCommand->redo();

    auto& scenar = m_scenarioPath.find();
    auto& movingTn = scenar.timeNode(m_movingTnId);
    auto& destinationTn = scenar.timeNode(m_destinationTnId);

    auto movingEvents = movingTn.events();
    for (auto& evId : movingEvents)
    {
      movingTn.removeEvent(evId);
      destinationTn.addEvent(evId);
    }
    destinationTn.trigger()->setActive(
        destinationTn.trigger()->active() || movingTn.trigger()->active());
    destinationTn.trigger()->setExpression(movingTn.trigger()->expression());

    scenar.timeNodes.remove(m_movingTnId);
    updateTimeNodeExtent(m_destinationTnId, scenar);
  }

  void update(
      Path<ProcessModel> scenar,
      const Id<TimeNodeModel>& clickedTn,
      const Id<TimeNodeModel>& hoveredTn)
  {
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_scenarioPath << m_movingTnId << m_destinationTnId
      << m_serializedTimeNode << m_moveCommand->serialize() << m_targetTrigger
      << m_targetTriggerActive;
  }

  void deserializeImpl(DataStreamOutput& s) override
  {
    QByteArray cmd;

    s >> m_scenarioPath >> m_movingTnId >> m_destinationTnId
        >> m_serializedTimeNode >> cmd >> m_targetTrigger
        >> m_targetTriggerActive;

    m_moveCommand = new MoveEvent<GoodOldDisplacementPolicy>{};
    m_moveCommand->deserialize(cmd);
  }

private:
  Path<ProcessModel> m_scenarioPath;
  Id<TimeNodeModel> m_movingTnId;
  Id<TimeNodeModel> m_destinationTnId;

  QByteArray m_serializedTimeNode;
  MoveEvent<GoodOldDisplacementPolicy>* m_moveCommand{};
  State::Expression m_targetTrigger;
  bool m_targetTriggerActive;
};
}
}

ISCORE_COMMAND_DECL_T(
    Scenario::Command::MergeTimeNodes<Scenario::ProcessModel>)

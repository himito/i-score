#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/Algorithms/StandardCreationPolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <iscore/tools/RandomNameProvider.hpp>

#include <QDataStream>
#include <QList>
#include <QVector>
#include <QtGlobal>
#include <algorithm>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <iscore/tools/IdentifierGeneration.hpp>
#include <vector>

#include "CreateConstraint.hpp"
#include <Process/Process.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <iscore/model/ModelMetadata.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/EntityMap.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/model/path/ObjectPath.hpp>
#include <iscore/model/Identifier.hpp>

namespace Scenario
{
namespace Command
{
CreateConstraint::CreateConstraint(
    Path<Scenario::ProcessModel>&& scenarioPath,
    Id<StateModel>
        startState,
    Id<StateModel>
        endState)
    : m_path{std::move(scenarioPath)}
    , m_createdName{RandomNameProvider::generateRandomName()}
    , m_startStateId{std::move(startState)}
    , m_endStateId{std::move(endState)}
{
  auto& scenar = m_path.find();
  // ISCORE_ASSERT(!scenar.state(startState).nextConstraint());
  // ISCORE_ASSERT(!scenar.state(endState).previousConstraint());
  m_createdConstraintId = getStrongId(scenar.constraints);
}

void CreateConstraint::undo() const
{
  auto& scenar = m_path.find();

  ScenarioCreate<ConstraintModel>::undo(m_createdConstraintId, scenar);
}

void CreateConstraint::redo() const
{
  auto& scenar = m_path.find();
  auto& sst = scenar.states.at(m_startStateId);
  auto& est = scenar.states.at(m_endStateId);

  ScenarioCreate<ConstraintModel>::redo(
      m_createdConstraintId,
      sst,
      est,
      sst.heightPercentage(),
      scenar);

  scenar.constraints.at(m_createdConstraintId)
      .metadata()
      .setName(m_createdName);
}

void CreateConstraint::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_createdName << m_createdConstraintId << m_startStateId
    << m_endStateId ;
}

void CreateConstraint::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_createdName >> m_createdConstraintId >> m_startStateId
      >> m_endStateId;
}
}
}

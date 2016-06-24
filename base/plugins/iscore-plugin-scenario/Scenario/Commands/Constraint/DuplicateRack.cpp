#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/Rack/RackModel.hpp>
#include <Scenario/Document/Constraint/Rack/Slot/SlotModel.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <vector>

#include "DuplicateRack.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
namespace Scenario
{
namespace Command
{
DuplicateRack::DuplicateRack(const RackModel& rack) :
    m_rackPath {rack}
{
    const auto& constraint = rack.constraint();

    m_newRackId = getStrongId(constraint.racks);
}

void DuplicateRack::undo() const
{
    auto& rack = m_rackPath.find();
    auto& constraint = rack.constraint();

    constraint.racks.remove(m_newRackId);
}

void DuplicateRack::redo() const
{
    auto& rack = m_rackPath.find();
    auto& constraint = rack.constraint();
    constraint.racks.add(new RackModel {rack,
                                    m_newRackId,
                                    &SlotModel::copyViewModelsInSameConstraint,
                                    &constraint});
}

void DuplicateRack::serializeImpl(DataStreamInput& s) const
{
    s << m_rackPath << m_newRackId;
}

void DuplicateRack::deserializeImpl(DataStreamOutput& s)
{
    s >> m_rackPath >> m_newRackId;
}

}
}

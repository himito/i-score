#include <Scenario/Document/Constraint/Rack/RackModel.hpp>

#include <algorithm>

#include "SwapSlots.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>


namespace Scenario
{
namespace Command
{
SwapSlots::SwapSlots(
        Path<RackModel>&& rack,
        Id<SlotModel> first,
        Id<SlotModel> second):
    m_rackPath{std::move(rack)},
    m_first{std::move(first)},
    m_second{std::move(second)}
{

}


void SwapSlots::undo() const
{
    redo();
}


void SwapSlots::redo() const
{
    auto& rack = m_rackPath.find();
    rack.swapSlots(m_first, m_second);
}


void SwapSlots::serializeImpl(DataStreamInput& s) const
{
    s << m_rackPath << m_first << m_second;
}


void SwapSlots::deserializeImpl(DataStreamOutput& s)
{
    s >> m_rackPath >> m_first >> m_second;
}
}
}

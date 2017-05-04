#include <Automation/AutomationModel.hpp>
#include <Curve/CurveModel.hpp>
#include <algorithm>

#include "InitAutomation.hpp"
#include <Curve/Segment/CurveSegmentData.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Automation
{
InitAutomation::InitAutomation(
    Path<ProcessModel>&& path,
    const ::State::AddressAccessor& newaddr,
    double newmin,
    double newmax,
    std::vector<Curve::SegmentData>&& segments)
    : m_path{std::move(path)}
    , m_addr(newaddr)
    , m_newMin{newmin}
    , m_newMax{newmax}
    , m_segments{std::move(segments)}
{
}

InitAutomation::InitAutomation(
    Path<ProcessModel>&& path,
    State::AddressAccessor&& newaddr,
    double newmin,
    double newmax,
    std::vector<Curve::SegmentData>&& segments)
    : m_path{std::move(path)}
    , m_addr(std::move(newaddr))
    , m_newMin{newmin}
    , m_newMax{newmax}
    , m_segments{std::move(segments)}
{
}

InitAutomation::InitAutomation(
    Path<ProcessModel>&& path,
    const ::State::AddressAccessor& newaddr,
    double newmin,
    double newmax)
    : InitAutomation(std::move(path), newaddr, newmin, newmax, {})
{
}
void InitAutomation::undo() const
{
}

void InitAutomation::redo() const
{
  auto& autom = m_path.find();

  auto& curve = autom.curve();

  if (!m_segments.empty())
    curve.fromCurveData(m_segments);

  autom.setMin(m_newMin);
  autom.setMax(m_newMax);

  autom.setAddress(m_addr);
}

void InitAutomation::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_addr << m_newMin << m_newMax << m_segments;
}

void InitAutomation::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_addr >> m_newMin >> m_newMax >> m_segments;
}
}

#include <algorithm>

#include "UpdateCurve.hpp"
#include <Curve/CurveModel.hpp>
#include <Curve/Segment/CurveSegmentData.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Curve
{
UpdateCurve::UpdateCurve(
    Path<Model>&& model, std::vector<SegmentData>&& segments)
    : m_model{std::move(model)}, m_newCurveData{std::move(segments)}
{
  const auto& curve = m_model.find();
  m_oldCurveData = curve.toCurveData();
}

void UpdateCurve::undo() const
{
  auto& curve = m_model.find();
  curve.fromCurveData(m_oldCurveData);
}

void UpdateCurve::redo() const
{
  auto& curve = m_model.find();
  curve.fromCurveData(m_newCurveData);
}

void UpdateCurve::serializeImpl(DataStreamInput& s) const
{
  s << m_model << m_oldCurveData << m_newCurveData;
}

void UpdateCurve::deserializeImpl(DataStreamOutput& s)
{
  s >> m_model >> m_oldCurveData >> m_newCurveData;
}
}

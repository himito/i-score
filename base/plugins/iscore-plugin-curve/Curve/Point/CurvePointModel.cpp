#include "CurvePointModel.hpp"
#include <iscore/model/IdentifiedObject.hpp>

class QObject;

namespace Curve
{
PointModel::PointModel(const Id<PointModel>& id, QObject* parent)
    : IdentifiedObject<PointModel>{id, "CurvePointModel", parent}
{
}

const OptionalId<SegmentModel>& PointModel::following() const
{
  return m_following;
}

void PointModel::setFollowing(const OptionalId<SegmentModel>& following)
{
  m_following = following;
}

Curve::Point PointModel::pos() const
{
  return m_pos;
}

void PointModel::setPos(const Curve::Point& pos)
{
  m_pos = pos;
  emit posChanged();
}

const OptionalId<SegmentModel>& PointModel::previous() const
{
  return m_previous;
}

void PointModel::setPrevious(const OptionalId<SegmentModel>& previous)
{
  m_previous = previous;
}
}

#include "CurveSegmentModel.hpp"
#include <Curve/Palette/CurvePoint.hpp>
#include <Curve/Segment/CurveSegmentData.hpp>
#include <iscore/model/IdentifiedObject.hpp>

class QObject;
template class IdentifiedObject<Curve::SegmentModel>;
template class iscore::SerializableInterface<Curve::SegmentModel>;

namespace Curve
{
SegmentModel::SegmentModel(const Id<SegmentModel>& id, QObject* parent)
    : IdentifiedObject<SegmentModel>{
          id, Metadata<ObjectKey_k, SegmentModel>::get(), parent}

{
}

SegmentModel::SegmentModel(const SegmentData& data, QObject* parent)
    : IdentifiedObject<SegmentModel>{data.id,
                                     Metadata<ObjectKey_k, SegmentModel>::
                                         get(),
                                     parent}
    , m_start{data.start}
    , m_end{data.end}
    , m_previous{data.previous}
    , m_following{data.following}
{
}

SegmentModel::SegmentModel(
    Curve::Point s,
    Curve::Point e,
    const Id<SegmentModel>& id,
    QObject* parent)
    : IdentifiedObject<SegmentModel>{id, Metadata<ObjectKey_k, SegmentModel>::
                                             get(),
                                     parent}
    , m_start{s}
    , m_end{e}
{
}

SegmentModel::~SegmentModel()
{
}

void SegmentModel::setPrevious(const OptionalId<SegmentModel>& previous)
{
  if (previous != m_previous)
  {
    m_previous = previous;
    emit previousChanged();
  }
}

void SegmentModel::setVerticalParameter(double p)
{
}

void SegmentModel::setFollowing(const OptionalId<SegmentModel>& following)
{
  if (following != m_following)
  {
    m_following = following;
    emit followingChanged();
  }
}

void SegmentModel::setHorizontalParameter(double p)
{
}

optional<double> SegmentModel::verticalParameter() const
{
  return {};
}

optional<double> SegmentModel::horizontalParameter() const
{
  return {};
}

void SegmentModel::setStart(const Curve::Point& pt)
{
  if (pt != m_start)
  {
    m_start = pt;
    m_valid = false;
    on_startChanged();

    emit startChanged();
  }
}

void SegmentModel::setEnd(const Curve::Point& pt)
{
  if (pt != m_end)
  {
    m_end = pt;
    m_valid = false;
    on_endChanged();

    emit endChanged();
  }
}
}

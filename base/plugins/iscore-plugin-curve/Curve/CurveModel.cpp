#include <Curve/Point/CurvePointModel.hpp>
#include <Curve/Segment/CurveSegmentList.hpp>
#include <Curve/Segment/CurveSegmentModel.hpp>
#include <Curve/Segment/CurveSegmentModelSerialization.hpp>

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>

#include <algorithm>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/tools/IdentifierGeneration.hpp>
#include <ossia/network/domain/domain_base.hpp>

#include "CurveModel.hpp"
#include <ossia/detail/algorithms.hpp>
#include <Curve/Segment/CurveSegmentData.hpp>
#include <State/ValueConversion.hpp>
#include <iscore/application/ApplicationContext.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/selection/Selectable.hpp>
#include <iscore/selection/Selection.hpp>
#include <iscore/model/IdentifiedObject.hpp>
#include <iscore/model/IdentifiedObjectMap.hpp>
#include <iscore/tools/MapCopy.hpp>
#include <iscore/model/Identifier.hpp>

class QObject;
namespace Curve
{
Model::Model(const Id<Model>& id, QObject* parent)
    : IdentifiedObject<Model>(id, "CurveModel", parent)
{
}

Model* Model::clone(const Id<Model>& id, QObject* parent)
{
  auto cm = new Model{id, parent};
  for (const auto& segment : m_segments)
  {
    auto seg = segment.clone(segment.id(), cm);
    seg->setPrevious(segment.previous());
    seg->setFollowing(segment.following());
    cm->addSegment(seg);
  }
  return cm;
}

void Model::addSortedSegment(SegmentModel* m)
{
  insertSegment(m);

  // Add points if necessary
  // If there is an existing previous segment, its end point also exists
  auto createStartPoint = [&]() {
    auto pt = new PointModel{getStrongId(m_points), this};
    pt->setFollowing(m->id());
    pt->setPos(m->start());
    addPoint(pt);
    return pt;
  };
  auto createEndPoint = [&]() {
    auto pt = new PointModel{getStrongId(m_points), this};
    pt->setPrevious(m->id());
    pt->setPos(m->end());
    addPoint(pt);
    return pt;
  };

  if (!m->previous())
  {
    createStartPoint();
  }
  else
  {
    // The previous segment has already been inserted,
    // hence the previous point is present.
    m_points.back()->setFollowing(m->id());
  }

  createEndPoint();
}

void Model::addSegment(SegmentModel* m)
{
  insertSegment(m);

  // Add points if necessary
  // If there is an existing previous segment, its end point also exists
  auto createStartPoint = [&]() {
    auto pt = new PointModel{getStrongId(m_points), this};
    pt->setFollowing(m->id());
    pt->setPos(m->start());
    addPoint(pt);
    return pt;
  };
  auto createEndPoint = [&]() {
    auto pt = new PointModel{getStrongId(m_points), this};
    pt->setPrevious(m->id());
    pt->setPos(m->end());
    addPoint(pt);
    return pt;
  };

  if (m->previous())
  {
    auto previousSegment = std::find_if(
        m_segments.begin(), m_segments.end(),
        [&](const auto& seg) { return seg.following() == m->id(); });
    if (previousSegment != m_segments.end())
    {
      auto thePt = std::find_if(
          m_points.begin(), m_points.end(), [&](PointModel* pt) {
            return pt->previous() == (*previousSegment).id();
          });

      if (thePt != m_points.end())
      {
        // The previous segments and points both exist
        (*thePt)->setFollowing(m->id());
      }
      else
      {
        // The previous segment exists but not the end point.
        auto pt = createStartPoint();
        pt->setPrevious((*previousSegment).id());
      }
    }
    else // The previous segment has not yet been added.
    {
      createStartPoint();
    }
  }
  else if (std::none_of(m_points.begin(), m_points.end(), [&](PointModel* pt) {
             return pt->following() == m->id();
           }))
  {
    createStartPoint();
  }

  if (m->following())
  {
    auto followingSegment = std::find_if(
        m_segments.begin(), m_segments.end(),
        [&](const auto& seg) { return seg.previous() == m->id(); });
    if (followingSegment != m_segments.end())
    {
      auto thePt = std::find_if(
          m_points.begin(), m_points.end(), [&](PointModel* pt) {
            return pt->following() == (*followingSegment).id();
          });

      if (thePt != m_points.end())
      {
        (*thePt)->setPrevious(m->id());
      }
      else
      {
        auto pt = createEndPoint();
        pt->setFollowing((*followingSegment).id());
      }
    }
    else
    {
      createEndPoint();
    }
  }
  else if (std::none_of(m_points.begin(), m_points.end(), [&](PointModel* pt) {
             return pt->previous() == m->id();
           }))
  {
    // Note : if one day a buggy case happens here, check that set
    // following/previous
    // are correctly set after cloning the segment.
    createEndPoint();
  }
}

void Model::insertSegment(SegmentModel* m)
{
  m->setParent(this);
  m_segments.insert(m);

  // TODO have indexes on the points with the start and end
  // curve segments
  connect(m, &SegmentModel::startChanged, this, [=]() {
    for (PointModel* pt : m_points)
    {
      if (pt->following() == m->id())
      {
        pt->setPos(m->start());
        break;
      }
    }
  });
  connect(m, &SegmentModel::endChanged, this, [=]() {
    for (PointModel* pt : m_points)
    {
      if (pt->previous() == m->id())
      {
        pt->setPos(m->end());
        break;
      }
    }
  });

  emit segmentAdded(*m);
}

void Model::removeSegment(SegmentModel* m)
{
  m_segments.remove(m->id());

  emit segmentRemoved(m->id());

  for (PointModel* pt : m_points)
  {
    if (pt->previous() == m->id())
    {
      pt->setPrevious(OptionalId<SegmentModel>{});
    }

    if (pt->following() == m->id())
    {
      pt->setFollowing(OptionalId<SegmentModel>{});
    }

    if (!pt->previous() && !pt->following())
    {
      removePoint(pt);
    }
  }

  delete m;
}

std::vector<SegmentModel*> Model::sortedSegments() const
{
  std::vector<SegmentModel*> dat;
  dat.reserve(m_segments.size());
  for (auto& seg : m_segments)
  {
    dat.push_back(&seg);
  }

  ossia::sort(
      dat, [](auto s1, auto s2) { return s1->start().x() < s2->start().x(); });

  return dat;
}

std::vector<SegmentData> Model::toCurveData() const
{
  std::vector<SegmentData> dat;
  dat.reserve(m_segments.size());
  for (const auto& seg : m_segments)
  {
    dat.push_back(seg.toSegmentData());
  }

  return dat;
}

void Model::fromCurveData(const std::vector<SegmentData>& curve)
{
  this->blockSignals(true);
  clear();

  auto& context = iscore::IDocument::documentContext(*this).app;
  auto& csl = context.interfaces<SegmentList>();
  CurveSegmentOrderedMap map(curve.begin(), curve.end());
  for (const auto& elt : map.get<Segments::Ordered>())
  {
    addSortedSegment(createCurveSegment(csl, elt, this));
  }
  this->blockSignals(false);
  emit curveReset();
  emit changed();
}

Selection Model::selectedChildren() const
{
  Selection s;
  for (const auto& elt : m_segments)
  {
    if (elt.selection.get())
      s.append(&elt);
  }
  for (const auto& elt : m_points)
  {
    if (elt->selection.get())
      s.append(elt);
  }

  return s;
}

void Model::setSelection(const Selection& s)
{
  // OPTIMIZEME
  for (auto& elt : m_segments)
    elt.selection.set(s.contains(&elt));
  for (auto& elt : m_points)
    elt->selection.set(s.contains(elt));
}

void Model::clear()
{
  emit cleared();

  auto segs = shallow_copy(m_segments);
  m_segments.clear();
  for (auto seg : segs)
    seg->deleteLater();

  auto pts = m_points;
  m_points.clear();
  for (auto pt : pts)
    pt->deleteLater();
}

const std::vector<PointModel*>& Model::points() const
{
  return m_points;
}

void Model::addPoint(PointModel* pt)
{
  m_points.push_back(pt);

  emit pointAdded(*pt);
}

void Model::removePoint(PointModel* pt)
{
  auto it = ossia::find(m_points, pt);
  if (it != m_points.end())
  {
    m_points.erase(it);
  }

  emit pointRemoved(pt->id());
  delete pt;
}

std::vector<SegmentData> orderedSegments(const Model& curve)
{
  auto vec = curve.toCurveData();

  decltype(vec) vec2;
  vec2.reserve(vec.size());

  CurveSegmentOrderedMap map{vec.begin(), vec.end()};
  for (const auto& elt : map.get<Segments::Ordered>())
  {
    vec2.push_back(elt);
  }

  return vec2;
}

CurveDomain::CurveDomain(const ossia::domain& dom):
  min{ossia::convert<double>(dom.get_min())}
, max{ossia::convert<double>(dom.get_max())}
{
  if(min == 0. && max == 0.)
  {
    max = 1.;
  }

  start = std::min(min, max);
  end = std::max(min, max);
}

CurveDomain::CurveDomain(const ossia::domain& dom, const State::Value& v):
  min{ossia::convert<double>(dom.get_min())}
, max{ossia::convert<double>(dom.get_max())}
{
  if(min == 0. && max == 0.)
  {
    const auto val = State::convert::value<double>(v);
    if(val > 0.)
      max = val;
    else if(val < 0.)
      min = val;
    else
      max = 1.;
  }

  if(min == max)
    max += 1.;

  start = std::min(min, max);
  end = std::max(min, max);
}

CurveDomain::CurveDomain(const ossia::domain& dom, double start, double end):
  min{ossia::convert<double>(dom.get_min())}
, max{ossia::convert<double>(dom.get_max())}
, start{start}
, end{end}
{
  auto min_v = dom.get_min();
  auto max_v = dom.get_max();
  min = (min_v.valid())
            ? std::min(ossia::convert<double>(min_v), std::min(start, end))
            : std::min(start, end);
  max = (max_v.valid())
            ? std::max(ossia::convert<double>(max_v), std::max(start, end))
            : std::max(start, end);

  ensureValid();
}

void CurveDomain::refine(const ossia::domain& dom)
{
  auto min_v = dom.get_min();
  auto max_v = dom.get_max();

  if (min_v.valid())
    min = std::min(min, ossia::convert<double>(min_v));
  else
    min = std::min(start, end);

  if (max_v.valid())
    max = std::max(max, ossia::convert<double>(max_v));
  else
    max = std::max(start, end);

  ensureValid();
}

void CurveDomain::ensureValid()
{
  if(min == 0. && max == 0.)
  {
    max = 1.;
  }
  else if(min == max)
  {
    max += 1.;
  }

  if(start == end)
  {
    start = std::min(min, max);
    end = std::max(min, max);
  }
}

}

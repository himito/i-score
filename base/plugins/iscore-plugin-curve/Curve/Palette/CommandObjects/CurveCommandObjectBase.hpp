#pragma once
#include <QByteArray>
#include <QPoint>
#include <QVector>
#include <algorithm>
#include <iscore/command/Dispatchers/SingleOngoingCommandDispatcher.hpp>
#include <iscore/tools/IdentifierGeneration.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <vector>

#include <Curve/Segment/CurveSegmentData.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/Identifier.hpp>
#include <iscore_plugin_curve_export.h>

namespace iscore
{
class CommandStackFacade;
} // namespace iscore

/*
concept CommandObject
{
    public:
        void instantiate();
        void update();
        void commit();
        void rollback();
};
*/
// CreateSegment
// CreateSegmentBetweenPoints

// RemoveSegment -> easy peasy
// RemovePoint -> which segment do we merge ? At the left or at the right ?
// A point(view) has pointers to one or both of its curve segments.

namespace Curve
{
class UpdateCurve;
class Model;
class Presenter;
class StateBase;
class SegmentModel;

class ISCORE_PLUGIN_CURVE_EXPORT CommandObjectBase
{
public:
  CommandObjectBase(Presenter* pres, const iscore::CommandStackFacade&);
  virtual ~CommandObjectBase();

  void setCurveState(Curve::StateBase* stateBase)
  {
    m_state = stateBase;
  }
  void press();

  void handleLocking();

  // Creates and pushes an UpdateCurve command
  // from a vector of segments.
  // They are removed afterwards
  void submit(std::vector<SegmentData>&&);

protected:
  auto
  find(std::vector<SegmentData>& segments, const OptionalId<SegmentModel>& id)
  {
    return std::find_if(
        segments.begin(), segments.end(), [&](const auto& seg) {
          return seg.id == id;
        });
  }
  auto find(
      const std::vector<SegmentData>& segments,
      const OptionalId<SegmentModel>& id)
  {
    return std::find_if(
        segments.cbegin(), segments.cend(), [&](const auto& seg) {
          return seg.id == id;
        });
  }

  virtual void on_press() = 0;

  QVector<QByteArray> m_oldCurveData;
  QPointF m_originalPress; // Note : there should be only one per curve...

  Presenter* m_presenter{};

  Curve::StateBase* m_state{};

  SingleOngoingCommandDispatcher<UpdateCurve> m_dispatcher;
  Path<Model> m_modelPath;

  std::vector<SegmentData> m_startSegments;

  // To prevent behind locked at 0.000001 or 0.9999
  double m_xmin{-1}, m_xmax{2}, m_xLastPoint{2};
};
}

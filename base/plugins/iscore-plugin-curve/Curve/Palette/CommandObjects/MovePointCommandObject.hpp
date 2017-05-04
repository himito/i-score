#pragma once
#include "CurveCommandObjectBase.hpp"
#include <Curve/Segment/CurveSegmentData.hpp>

namespace iscore
{
class CommandStackFacade;
} // namespace iscore

namespace Curve
{
class Presenter;
class ISCORE_PLUGIN_CURVE_EXPORT MovePointCommandObject final
    : public CommandObjectBase
{
public:
  MovePointCommandObject(
      Presenter* presenter, const iscore::CommandStackFacade& stack);
  ~MovePointCommandObject();

  void on_press() override;

  void move();

  void release();

  void cancel();

private:
  void handlePointOverlap(CurveSegmentMap& segments);
  void handleSuppressOnOverlap(CurveSegmentMap& segments);
  void handleCrossOnOverlap(CurveSegmentMap& segments);
  void setCurrentPoint(CurveSegmentMap& segments);
};
}

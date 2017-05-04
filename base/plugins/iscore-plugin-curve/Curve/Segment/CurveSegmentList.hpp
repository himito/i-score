#pragma once
#include <Curve/Segment/CurveSegmentFactory.hpp>

#include <iscore/plugins/customfactory/FactoryFamily.hpp>
namespace Curve
{
class SegmentList final : public iscore::InterfaceList<SegmentFactory>
{
public:
  using object_type = Curve::SegmentModel;
  virtual ~SegmentList();

  object_type* loadMissing(const VisitorVariant& vis, QObject* parent) const;
};
}

#pragma once
#include <Curve/Segment/CurveSegmentFactory.hpp>

#include <iscore/plugins/customfactory/FactoryFamily.hpp>
namespace Curve
{
class SegmentList final :
        public iscore::ConcreteFactoryList<SegmentFactory>
{
    public:
        using object_type = Curve::SegmentModel;
};
}

#pragma once
#include <Engine/LocalTree/LocalTreeComponent.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

namespace Engine
{
namespace LocalTree
{
class TimeNode final : public CommonComponent
{
  COMMON_COMPONENT_METADATA("104e4446-b09f-4bf6-92ef-0fe360397066")
public:
  TimeNode(
      ossia::net::node_base& parent,
      const Id<iscore::Component>& id,
      Scenario::TimeNodeModel& event,
      DocumentPlugin& doc,
      QObject* parent_comp);
};
}
}

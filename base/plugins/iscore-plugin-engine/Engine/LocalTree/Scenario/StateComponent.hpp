#pragma once
#include <Engine/LocalTree/LocalTreeComponent.hpp>
#include <Scenario/Document/State/StateModel.hpp>

namespace Engine
{
namespace LocalTree
{
class ISCORE_PLUGIN_ENGINE_EXPORT State final : public CommonComponent
{
  COMMON_COMPONENT_METADATA("2e5fefa2-3442-4c08-9f3e-564ab65f7b22")
public:
  State(
      ossia::net::node_base& parent,
      const Id<iscore::Component>& id,
      Scenario::StateModel& event,
      DocumentPlugin& doc,
      QObject* parent_comp);
};
}
}

#pragma once
#include <Engine/Executor/Component.hpp>

#include <ossia/editor/state/state.hpp>

namespace ossia
{
class time_event;
}
namespace Scenario
{
class StateModel;
}

namespace Engine
{
namespace Execution
{
class ISCORE_PLUGIN_ENGINE_EXPORT StateComponent final :
    public Execution::Component
{
  COMMON_COMPONENT_METADATA("b3905e79-2bd0-48bd-8654-8666455ceedd")
public:
  StateComponent(
      const Scenario::StateModel& element,
      const Engine::Execution::Context& ctx,
      const Id<iscore::Component>& id,
      QObject* parent);

  void cleanup();

  //! To be called from the API edition queue
  void onSetup(
      const std::shared_ptr<ossia::time_event>& root);

  //! To be called from the GUI thread
  void onDelete() const;
private:
  std::shared_ptr<ossia::time_event> m_ev;
  ossia::state m_state;
};
}
}

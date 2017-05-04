#include "InspectorPanelFactory.hpp"
#include "InspectorPanelDelegate.hpp"

namespace InspectorPanel
{

std::unique_ptr<iscore::PanelDelegate>
PanelDelegateFactory::make(const iscore::GUIApplicationContext& ctx)
{
  return std::make_unique<PanelDelegate>(ctx);
}
}

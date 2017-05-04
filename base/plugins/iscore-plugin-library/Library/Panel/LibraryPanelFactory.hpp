#pragma once
#include <iscore/plugins/panel/PanelDelegateFactory.hpp>

namespace Library
{
class PanelDelegateFactory final : public iscore::PanelDelegateFactory
{
  ISCORE_CONCRETE("ddbc5169-1ca3-4a64-a805-40b8fc0e1e02")

  std::unique_ptr<iscore::PanelDelegate>
  make(const iscore::GUIApplicationContext& ctx) override;
};
}

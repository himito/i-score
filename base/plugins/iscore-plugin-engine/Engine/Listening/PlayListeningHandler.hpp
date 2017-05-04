#pragma once
#include <Explorer/Listening/ListeningHandler.hpp>

namespace Device
{
class DeviceList;
}
namespace Engine
{
namespace Execution
{
class DocumentPlugin;

class PlayListeningHandler final : public Explorer::ListeningHandler
{
  const Engine::Execution::DocumentPlugin& m_executor;

public:
  PlayListeningHandler(const Engine::Execution::DocumentPlugin& docpl);

private:
  void setListening(
      Device::DeviceInterface& dev,
      const State::Address& addr,
      bool b) override;

  void addToListening(
      Device::DeviceInterface& dev,
      const std::vector<State::Address>& v) override;
};
}
}

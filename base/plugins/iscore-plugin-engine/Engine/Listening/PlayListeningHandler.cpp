#include "PlayListeningHandler.hpp"
#include <Device/Protocol/DeviceList.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>

namespace Engine
{
namespace Execution
{
PlayListeningHandler::PlayListeningHandler(
    const Engine::Execution::DocumentPlugin& docpl)
    : m_executor{docpl}
{
}

void PlayListeningHandler::setListening(
    Device::DeviceInterface& dev, const State::Address& addr, bool b)
{
  if (!m_executor.isPlaying())
  {
    dev.setListening(addr, b);
  }
}

void PlayListeningHandler::addToListening(
    Device::DeviceInterface& dev, const std::vector<State::Address>& v)
{
  if (!m_executor.isPlaying())
  {
    dev.addToListening(v);
  }
}
}
}

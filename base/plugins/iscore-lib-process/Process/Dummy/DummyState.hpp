#pragma once
#include <Process/State/ProcessStateDataInterface.hpp>
#include <QString>
#include <vector>

#include <Process/State/MessageNode.hpp>
#include <State/Message.hpp>
#include <iscore_lib_process_export.h>

namespace Process
{
class ProcessModel;
}
class QObject;
namespace State
{
struct Address;
} // namespace iscore

namespace Dummy
{
class ISCORE_LIB_PROCESS_EXPORT DummyState final
    : public ProcessStateDataInterface
{
public:
  DummyState(Process::ProcessModel& model, QObject* parent);
  ProcessStateDataInterface* clone(QObject* parent) const override;

  std::vector<State::AddressAccessor> matchingAddresses() override;
  State::MessageList messages() const override;
  State::MessageList setMessages(
      const State::MessageList& newMessages,
      const Process::MessageNode& currentState) override;
};
}

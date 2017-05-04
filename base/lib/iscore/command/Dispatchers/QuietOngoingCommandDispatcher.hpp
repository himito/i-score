#pragma once
#include <iscore/command/Dispatchers/ICommandDispatcher.hpp>
#include <iscore/command/Dispatchers/SendStrategy.hpp>

#include <core/command/CommandStack.hpp>

namespace iscore
{
class QuietOngoingCommandDispatcher final : public ICommandDispatcher
{
public:
  QuietOngoingCommandDispatcher(const iscore::CommandStackFacade& stack)
      : ICommandDispatcher{stack}
  {
  }

  template <typename TheCommand, typename... Args>
  void submitCommand(Args&&... args)
  {
    if (!m_cmd)
    {
      stack().disableActions();
      m_cmd = std::make_unique<TheCommand>(std::forward<Args>(args)...);
    }
    else
    {
      ISCORE_ASSERT(m_cmd->key() == TheCommand::static_key());
      safe_cast<TheCommand*>(m_cmd.get())->update(std::forward<Args>(args)...);
    }
  }

  template<typename Strategy = SendStrategy::Simple> // TODO why ?
  void commit()
  {
    if (m_cmd)
    {
      Strategy::send(stack(), m_cmd.release());
      stack().enableActions();
    }
  }

  void rollback()
  {
    if (m_cmd)
    {
      stack().enableActions();
    }
    m_cmd.reset();
  }

private:
  std::unique_ptr<iscore::Command> m_cmd;
};
}

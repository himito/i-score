#pragma once
#include <iscore/command/AggregateCommand.hpp>
#include <iscore/command/Dispatchers/ICommandDispatcher.hpp>
#include <iscore/command/Dispatchers/SendStrategy.hpp>
#include <memory>

/**
 * @brief The MacroCommandDispatcher class
 *
 * Used to send multiple "one-shot" commands one after the other.
 * An aggregate command is required : it will put them under the same "command"
 * once in the stack.
 */

template <typename Command_T, typename RedoStrategy_T, typename SendStrategy_T>
class GenericMacroCommandDispatcher final : public ICommandDispatcher
{
public:
  template <typename... Args>
  GenericMacroCommandDispatcher(Args&&... args)
      : ICommandDispatcher{std::forward<Args&&>(args)...}
      , m_aggregateCommand{std::make_unique<Command_T>()}
  {
    static_assert(
        std::is_base_of<iscore::AggregateCommand, Command_T>::value,
        "MacroCommandDispatcher: Command_T must be AggregateCommand-derived");
  }

  void submitCommand(iscore::Command* cmd)
  {
    RedoStrategy_T::redo(*cmd);
    m_aggregateCommand->addCommand(cmd);
  }

  void commit()
  {
    if (m_aggregateCommand)
    {
      if (m_aggregateCommand->count() != 0)
      {
        SendStrategy_T::send(stack(), m_aggregateCommand.release());
      }

      m_aggregateCommand.reset();
    }
  }

  void rollback()
  {
    if (m_aggregateCommand)
    {
      m_aggregateCommand->undo();
      m_aggregateCommand.reset();
    }
  }

  auto command() const
  {
    return m_aggregateCommand.get();
  }

protected:
  std::unique_ptr<Command_T> m_aggregateCommand;
};

// Don't redo the individual commands, and redo() the aggregate command.
template <typename Command_T>
using MacroCommandDispatcher
    = GenericMacroCommandDispatcher<Command_T, RedoStrategy::Quiet, SendStrategy::Simple>;

// Redo the individual commands, don't redo the aggregate command
template <typename Command_T>
using RedoMacroCommandDispatcher
    = GenericMacroCommandDispatcher<Command_T, RedoStrategy::Redo, SendStrategy::Quiet>;

// Don't redo anything, just push
template <typename Command_T>
using QuietMacroCommandDispatcher
    = GenericMacroCommandDispatcher<Command_T, RedoStrategy::Quiet, SendStrategy::Quiet>;

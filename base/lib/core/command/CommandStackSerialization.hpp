#pragma once
#include <core/command/CommandStack.hpp>
#include <iscore/command/CommandData.hpp>
#include <iscore/plugins/customfactory/StringFactoryKeySerialization.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
namespace iscore
{
template <typename RedoFun>
void loadCommandStack(
    const iscore::ApplicationComponents& components,
    DataStreamWriter& writer,
    iscore::CommandStack& stack,
    RedoFun redo_fun)
{
  std::vector<iscore::CommandData> undoStack, redoStack;
  writer.writeTo(undoStack);
  writer.writeTo(redoStack);

  writer.checkDelimiter();

  stack.undoable().clear();
  stack.redoable().clear();

  stack.updateStack([&]() {
    stack.setSavedIndex(-1);

    for (const auto& elt : undoStack)
    {
      auto cmd = components.instantiateUndoCommand(elt);

      redo_fun(cmd);
      stack.undoable().push(cmd);
    }

    for (const auto& elt : redoStack)
    {
      auto cmd = components.instantiateUndoCommand(elt);

      stack.redoable().push(cmd);
    }
  });
}
}

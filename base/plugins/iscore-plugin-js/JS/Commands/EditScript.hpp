#pragma once
#include <JS/Commands/JSCommandFactory.hpp>
#include <QString>
#include <iscore/command/Command.hpp>

#include <iscore/model/path/Path.hpp>

struct DataStreamInput;
struct DataStreamOutput;
namespace JS
{
class ProcessModel;
class StateProcess;

class EditScript final : public iscore::Command
{
  ISCORE_COMMAND_DECL(JS::CommandFactoryName(), EditScript, "Edit a JS script")
public:
  EditScript(Path<ProcessModel>&& model, const QString& text);

  void undo() const override;
  void redo() const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<ProcessModel> m_model;
  QString m_old, m_new;
};

class EditStateScript final : public iscore::Command
{
  ISCORE_COMMAND_DECL(
      JS::CommandFactoryName(), EditStateScript, "Edit a JS state script")
public:
  EditStateScript(Path<StateProcess>&& model, const QString& text);

  void undo() const override;
  void redo() const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<StateProcess> m_model;
  QString m_old, m_new;
};
}

#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/Command.hpp>

#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{
template <class T>
class ChangeElementComments final : public iscore::Command
{
  // No ISCORE_COMMAND here since it's a template.
public:
  const CommandGroupKey& parentKey() const noexcept override
  {
    return ScenarioCommandFactoryName();
  }
  static const CommandKey& static_key() noexcept
  {
    auto name
        = QString("ChangeElementComments_") + Metadata<ObjectKey_k, T>::get();
    static const CommandKey kagi{std::move(name)};
    return kagi;
  }
  const CommandKey& key() const noexcept override
  {
    return static_key();
  }
  QString description() const override
  {
    return QObject::tr("Change %1 comments")
        .arg(Metadata<Description_k, T>::get());
  }

  ChangeElementComments() = default;

  ChangeElementComments(Path<T>&& path, QString newComments)
      : m_path{std::move(path)}, m_newComments{std::move(newComments)}
  {
    auto& obj = m_path.find();
    m_oldComments = obj.metadata().getComment();
  }

  void undo() const override
  {
    auto& obj = m_path.find();
    obj.metadata().setComment(m_oldComments);
  }

  void redo() const override
  {
    auto& obj = m_path.find();
    obj.metadata().setComment(m_newComments);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_oldComments << m_newComments;
  }

  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_oldComments >> m_newComments;
  }

private:
  Path<T> m_path;
  QString m_oldComments;
  QString m_newComments;
};
}
}

ISCORE_COMMAND_DECL_T(ChangeElementComments<ConstraintModel>)
ISCORE_COMMAND_DECL_T(ChangeElementComments<EventModel>)
ISCORE_COMMAND_DECL_T(ChangeElementComments<TimeNodeModel>)
ISCORE_COMMAND_DECL_T(ChangeElementComments<StateModel>)
ISCORE_COMMAND_DECL_T(ChangeElementComments<Process::ProcessModel>)

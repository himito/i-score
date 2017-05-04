#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/Command.hpp>

#include <iscore/model/ColorReference.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{
template <class T>
class ChangeElementColor final : public iscore::Command
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
        = QString("ChangeElementColor_") + Metadata<ObjectKey_k, T>::get();
    static const CommandKey kagi{std::move(name)};
    return kagi;
  }
  const CommandKey& key() const noexcept override
  {
    return static_key();
  }
  QString description() const override
  {
    return QObject::tr("Change %1 color")
        .arg(Metadata<Description_k, T>::get());
  }

  ChangeElementColor() = default;
  ChangeElementColor(Path<T>&& path, iscore::ColorRef newColor)
      : m_path{std::move(path)}, m_newColor{newColor}
  {
    auto& obj = m_path.find();
    m_oldColor = obj.metadata().getColor();
  }

  void undo() const override
  {
    auto& obj = m_path.find();
    obj.metadata().setColor(m_oldColor);
  }

  void redo() const override
  {
    auto& obj = m_path.find();
    obj.metadata().setColor(m_newColor);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_oldColor << m_newColor;
  }

  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_oldColor >> m_newColor;
  }

private:
  Path<T> m_path;
  iscore::ColorRef m_newColor;
  iscore::ColorRef m_oldColor;
};
}
}

ISCORE_COMMAND_DECL_T(ChangeElementColor<ConstraintModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<EventModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<TimeNodeModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<StateModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<Process::ProcessModel>)

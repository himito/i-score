#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/SerializableCommand.hpp>

#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>

namespace Scenario
{
namespace Command
{
template<class T>
class ChangeElementName final : public iscore::SerializableCommand
{
        // No ISCORE_COMMAND here since it's a template.
    public:
        const CommandParentFactoryKey& parentKey() const override
        {
            return ScenarioCommandFactoryName();
        }
        static const CommandFactoryKey& static_key()
        {
            auto name = QString("ChangeElementName_")  + Metadata<ObjectKey_k, T>::get();
            static const CommandFactoryKey kagi{std::move(name)};
            return kagi;
        }
        const CommandFactoryKey& key() const override
        {
            return static_key();
        }
        QString description() const override
        {
            return QObject::tr("Change %1 name").arg(Metadata<Description_k, T>::get());
        }

        ChangeElementName() = default;

        ChangeElementName(Path<T>&& path, QString newName) :
            m_path {std::move(path) },
            m_newName {std::move(newName)}
        {
            auto& obj = m_path.find();
            m_oldName = obj.metadata.name();
        }

        void undo() const override
        {
            auto& obj = m_path.find();
            obj.metadata.setName(m_oldName);
        }

        void redo() const override
        {
            auto& obj = m_path.find();
            obj.metadata.setName(m_newName);
        }

    protected:
        void serializeImpl(DataStreamInput& s) const override
        {
            s << m_path << m_oldName << m_newName;
        }

        void deserializeImpl(DataStreamOutput& s) override
        {
            s >> m_path >> m_oldName >> m_newName;
        }

    private:
        Path<T> m_path;
        QString m_newName;
        QString m_oldName;
};
}
}

ISCORE_COMMAND_DECL_T(ChangeElementName<ConstraintModel>)
ISCORE_COMMAND_DECL_T(ChangeElementName<EventModel>)
ISCORE_COMMAND_DECL_T(ChangeElementName<TimeNodeModel>)
ISCORE_COMMAND_DECL_T(ChangeElementName<StateModel>)
ISCORE_COMMAND_DECL_T(ChangeElementName<Process::ProcessModel>)

#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ObjectPath.hpp>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QVariant>

namespace iscore
{
/**
 * @brief The PropertyCommand class
 *
 * This generic command allows for a very simple operation when
 * changing a property specified with Q_PROPERTY.
 *
 * It will save the current state and switch between the current and new
 * state upon undo / redo.
 */
class ISCORE_LIB_BASE_EXPORT PropertyCommand : public SerializableCommand
{
    public:
        using SerializableCommand::SerializableCommand;
        PropertyCommand() = default;

        template<typename Path_T, typename... Args>
        PropertyCommand(Path_T&& path,
                        QString property,
                        QVariant newval):
            m_path{std::move(path).unsafePath()},
            m_property{std::move(property)},
            m_new{std::move(newval)}
        {
            m_old = m_path.find<QObject>().property(m_property.toUtf8().constData());
        }

        virtual ~PropertyCommand();

        void undo() const final override;
        void redo() const final override;

        template<typename Path_T>
        void update(const Path_T&, QVariant newval)
        {
            m_new = std::move(newval);
        }

    protected:
        void serializeImpl(DataStreamInput &) const final override;
        void deserializeImpl(DataStreamOutput &) final override;

    private:
        ObjectPath m_path;
        QString m_property;
        QVariant m_old, m_new;
};
}

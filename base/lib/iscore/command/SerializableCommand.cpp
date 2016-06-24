#include <iscore/command/SerializableCommand.hpp>
#include <QDataStream>
#include <QtGlobal>
#include <QIODevice>

#include <iscore/serialization/DataStreamVisitor.hpp>

namespace iscore
{
SerializableCommand::~SerializableCommand() = default;

QByteArray SerializableCommand::serialize() const
{
    QByteArray arr;
    {
        QDataStream s(&arr, QIODevice::Append);
        s.setVersion(QDataStream::Qt_5_3);

        s << timestamp();
        DataStreamInput inp{s};
        serializeImpl(inp);
    }

    return arr;
}

void SerializableCommand::deserialize(const QByteArray& arr)
{
    QDataStream s(arr);
    s.setVersion(QDataStream::Qt_5_3);

    quint32 stmp;
    s >> stmp;

    setTimestamp(stmp);

    DataStreamOutput outp{s};
    deserializeImpl(outp);
}
}

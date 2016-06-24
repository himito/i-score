#pragma once
#include <QMetaType>
#include <QJsonObject>

namespace Ossia
{
struct MIDISpecificSettings
{
        enum class IO { In, Out } io;
        QString endpoint;
        int port{};
};
}
Q_DECLARE_METATYPE(Ossia::MIDISpecificSettings)


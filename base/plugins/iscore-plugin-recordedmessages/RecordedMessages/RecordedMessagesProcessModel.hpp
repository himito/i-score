#pragma once
#include <RecordedMessages/RecordedMessagesProcessMetadata.hpp>
#include <Process/Process.hpp>
#include <QByteArray>
#include <QString>
#include <memory>


#include <Process/TimeValue.hpp>
#include <State/Message.hpp>
#include <iscore/selection/Selection.hpp>
#include <iscore/serialization/VisitorInterface.hpp>

class DataStream;
class JSONObject;
namespace Process { class LayerModel; }
class ProcessStateDataInterface;
class QObject;
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_recordedmessages_export.h>
namespace RecordedMessages
{
struct RecordedMessage
{
    double percentage;
    State::Message message;
};
using RecordedMessagesList = QList<RecordedMessage>;

class ISCORE_PLUGIN_RECORDEDMESSAGES_EXPORT ProcessModel final : public Process::ProcessModel
{
        ISCORE_SERIALIZE_FRIENDS(ProcessModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(ProcessModel, JSONObject)
        PROCESS_METADATA_IMPL(RecordedMessages::ProcessModel)
    Q_OBJECT
    public:
        explicit ProcessModel(
                const TimeValue& duration,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        explicit ProcessModel(
                const ProcessModel& source,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        template<typename Impl>
        explicit ProcessModel(
                Deserializer<Impl>& vis,
                QObject* parent) :
            Process::ProcessModel{vis, parent}
        {
            vis.writeTo(*this);
        }

        virtual ~ProcessModel();
        void setMessages(const QList<RecordedMessage>& script);
        const auto& messages() const
        { return m_messages; }

        // Process interface
        void setDurationAndScale(const TimeValue& newDuration) override;
        void setDurationAndGrow(const TimeValue& newDuration) override;
        void setDurationAndShrink(const TimeValue& newDuration) override;

    signals:
        void messagesChanged();

    protected:

    private:
        RecordedMessagesList m_messages;
};


}

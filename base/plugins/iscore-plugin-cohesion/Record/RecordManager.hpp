#pragma once
#include <Record/RecordTools.hpp>
#include <Record/RecordData.hpp>

namespace Recording
{
// TODO for some reason we have to undo redo
// to be able to send the curve at execution. Investigate why.
class RecordManager final : public QObject
{
    public:
        RecordManager(const iscore::DocumentContext& ctx);

        void recordInNewBox(Scenario::ScenarioModel& scenar, Scenario::Point pt);
        // TODO : recordInExstingBox; recordFromState.
        void stopRecording();

        void commit();

    private:
        const iscore::DocumentContext& m_ctx;
        std::unique_ptr<RecordCommandDispatcher> m_dispatcher;
        Explorer::ListeningState m_savedListening;
        std::vector<QMetaObject::Connection> m_recordCallbackConnections;

        Explorer::DeviceExplorerModel* m_explorer{};

        QTimer m_recordTimer;
        bool m_firstValueReceived{};
        std::chrono::steady_clock::time_point start_time_pt;

        std::unordered_map<
            Device::FullAddressSettings,
            RecordData> records;
};
}

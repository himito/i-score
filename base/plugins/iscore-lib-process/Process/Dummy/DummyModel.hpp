#pragma once
#include <Process/Dummy/DummyState.hpp>
#include <Process/Process.hpp>
#include <Process/ProcessMetadata.hpp>
#include <QByteArray>
#include <QString>


#include <Process/TimeValue.hpp>
#include <iscore/selection/Selection.hpp>
#include <iscore/serialization/VisitorInterface.hpp>
#include <iscore_lib_process_export.h>


class DataStream;
class JSONObject;
namespace Process { class LayerModel; }
class ProcessStateDataInterface;
class QObject;
#include <iscore/tools/SettableIdentifier.hpp>

namespace Dummy
{
class DummyModel;
}
PROCESS_METADATA(
        ISCORE_LIB_PROCESS_EXPORT,
        Dummy::DummyModel,
        "7db45400-6033-425e-9ded-d60a35d4c4b2",
        "Dummy",
        "Dummy"
        )


namespace Dummy
{
class ISCORE_LIB_PROCESS_EXPORT DummyModel final :
        public Process::ProcessModel
{
        ISCORE_SERIALIZE_FRIENDS(DummyModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(DummyModel, JSONObject)
        PROCESS_METADATA_IMPL(Dummy::DummyModel)

    public:
        explicit DummyModel(
                const TimeValue& duration,
                const Id<ProcessModel>& id,
                QObject* parent);

        explicit DummyModel(
                const DummyModel& source,
                const Id<ProcessModel>& id,
                QObject* parent);

        template<typename Impl>
        explicit DummyModel(
                Deserializer<Impl>& vis,
                QObject* parent) :
            ProcessModel{vis, parent}
        {
            vis.writeTo(*this);
        }

    private:
        ProcessStateDataInterface* startStateData() const override;
        ProcessStateDataInterface* endStateData() const override;

        mutable DummyState m_startState{*this, nullptr};
        mutable DummyState m_endState{*this, nullptr};
};
}

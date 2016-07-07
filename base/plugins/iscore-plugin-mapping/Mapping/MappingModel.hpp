#pragma once

#include <Curve/Process/CurveProcessModel.hpp>
#include <State/Address.hpp>
#include <QByteArray>
#include <QString>


#include <Process/TimeValue.hpp>
#include <iscore/serialization/VisitorInterface.hpp>
#include <iscore_plugin_mapping_export.h>
class DataStream;
class JSONObject;
namespace Process {
class LayerModel;
}
class ProcessStateDataInterface;
class QObject;
#include <iscore/tools/SettableIdentifier.hpp>

namespace Mapping
{
class ISCORE_PLUGIN_MAPPING_EXPORT ProcessModel final : public Curve::CurveProcessModel
{
        ISCORE_SERIALIZE_FRIENDS(Mapping::ProcessModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Mapping::ProcessModel, JSONObject)

        Q_OBJECT

        Q_PROPERTY(State::Address sourceAddress READ sourceAddress WRITE setSourceAddress NOTIFY sourceAddressChanged)
        Q_PROPERTY(double sourceMin READ sourceMin WRITE setSourceMin NOTIFY sourceMinChanged)
        Q_PROPERTY(double sourceMax READ sourceMax WRITE setSourceMax NOTIFY sourceMaxChanged)

        Q_PROPERTY(State::Address targetAddress READ targetAddress WRITE setTargetAddress NOTIFY targetAddressChanged)
        Q_PROPERTY(double targetMin READ targetMin WRITE setTargetMin NOTIFY targetMinChanged)
        Q_PROPERTY(double targetMax READ targetMax WRITE setTargetMax NOTIFY targetMaxChanged)
    public:
        ProcessModel(
                const TimeValue& duration,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        template<typename Impl>
        ProcessModel(Deserializer<Impl>& vis, QObject* parent) :
            CurveProcessModel{vis, parent}
        {
            vis.writeTo(*this);
        }

        //// MappingModel specifics ////
        State::Address sourceAddress() const;
        double sourceMin() const;
        double sourceMax() const;

        void setSourceAddress(const State::Address& arg);
        void setSourceMin(double arg);
        void setSourceMax(double arg);

        State::Address targetAddress() const;
        double targetMin() const;
        double targetMax() const;

        void setTargetAddress(const State::Address& arg);
        void setTargetMin(double arg);
        void setTargetMax(double arg);

        QString prettyName() const override;

        ~ProcessModel();
    signals:
        void sourceAddressChanged(const State::Address& arg);
        void sourceMinChanged(double arg);
        void sourceMaxChanged(double arg);

        void targetAddressChanged(const State::Address& arg);
        void targetMinChanged(double arg);
        void targetMaxChanged(double arg);

    private:
        ProcessModel(const ProcessModel& source,
                        const Id<Process::ProcessModel>& id,
                        QObject* parent);

        Process::ProcessModel* clone(
                const Id<Process::ProcessModel>& newId,
                QObject* newParent) const override;

        //// ProcessModel ////
        UuidKey<Process::ProcessFactory> concreteFactoryKey() const override;

        void setDurationAndScale(const TimeValue& newDuration) override;
        void setDurationAndGrow(const TimeValue& newDuration) override;
        void setDurationAndShrink(const TimeValue& newDuration) override;

        void serialize_impl(const VisitorVariant& vis) const override;

        State::Address m_sourceAddress;
        State::Address m_targetAddress;

        double m_sourceMin{};
        double m_sourceMax{};

        double m_targetMin{};
        double m_targetMax{};
};
}

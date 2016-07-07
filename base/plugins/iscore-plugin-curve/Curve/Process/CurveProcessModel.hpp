#pragma once
#include <Curve/CurveModel.hpp>
#include <Curve/Point/CurvePointModel.hpp>
#include <Process/Process.hpp>
#include <QString>

#include <Curve/Segment/CurveSegmentModel.hpp>
#include <iscore/selection/Selection.hpp>
#include <iscore_plugin_curve_export.h>

namespace Curve
{
class ISCORE_PLUGIN_CURVE_EXPORT CurveProcessModel :
        public Process::ProcessModel
{
        Q_OBJECT
    public:
        CurveProcessModel(
            TimeValue duration,
            const Id<ProcessModel>& id,
            const QString& name,
            QObject* parent) :
            Process::ProcessModel( duration, id, name, parent )
        {

        }

        CurveProcessModel(Deserializer<DataStream>& vis, QObject* p) :
            Process::ProcessModel(vis, p)
        {
            // Nothing to do
        }

        CurveProcessModel(Deserializer<JSONObject>& vis, QObject* p) :
            Process::ProcessModel(vis, p)
        {
            // Nothing to do
        }

        // Clone
        CurveProcessModel(
            const CurveProcessModel& other,
            const Id<ProcessModel>& id,
            const QString& name,
            QObject* parent):
            Process::ProcessModel(other, id, name, parent)
        {

        }


        Model& curve() const
        { return *m_curve; }

        virtual ~CurveProcessModel();


        void startExecution() final override
        {
            emit execution(true);
        }

        void stopExecution() final override
        {
            emit execution(false);
        }

        void reset() final override
        {

        }


        Selection selectableChildren() const final override
        {
            Selection s;
            for(auto& segment : m_curve->segments())
                s.append(&segment);
            for(auto& point : m_curve->points())
                s.append(point);
            return s;
        }

        Selection selectedChildren() const final override
        {
            return m_curve->selectedChildren();
        }

        void setSelection(const Selection & s) const final override
        {
            m_curve->setSelection(s);
        }


    signals:
        void curveChanged();

    protected:
        void setCurve(Model* newCurve)
        {
            delete m_curve;
            m_curve = newCurve;

            setCurve_impl();

            emit m_curve->changed();
        }

        virtual void setCurve_impl() { }

        Model* m_curve{};
};
}

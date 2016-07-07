#pragma once
#include <Process/ProcessFactory.hpp>
#include <Process/Process.hpp>
#include <Process/LayerModel.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <iscore_plugin_curve_export.h>

namespace Curve
{
class EditionSettings;
template<
        typename Model_T,
        typename LayerModel_T,
        typename LayerPresenter_T,
        typename LayerView_T,
        typename CurveColors_T>
class CurveProcessFactory_T :
        public Process::ProcessFactory
{
    public:
        virtual ~CurveProcessFactory_T() = default;

        Model_T* makeModel(
                const TimeValue& duration,
                const Id<Process::ProcessModel>& id,
                QObject* parent) final override
        {
            return new Model_T{duration, id, parent};
        }

        Model_T* load(
                const VisitorVariant& vis,
                QObject* parent) final override
        {
            return deserialize_dyn(vis, [&] (auto&& deserializer)
            { return new Model_T{deserializer, parent}; });
        }

        Process::LayerModel* makeLayer_impl(
                Process::ProcessModel& proc,
                const Id<Process::LayerModel>& viewModelId,
                const QByteArray& constructionData,
                QObject* parent) final override
        {
            auto layer = new LayerModel_T{
                         static_cast<Model_T&>(proc),
                         viewModelId, parent};
            return layer;
        }

        Process::LayerModel* cloneLayer_impl(
                Process::ProcessModel& proc,
                const Id<Process::LayerModel>& newId,
                const Process::LayerModel& source,
                QObject* parent) final override
        {
            auto layer = new LayerModel_T {
                      static_cast<const LayerModel_T&>(source),
                      static_cast<Model_T&>(proc),
                      newId, parent};
            return layer;
        }

        Process::LayerModel* loadLayer_impl(
                Process::ProcessModel& proc,
                const VisitorVariant& vis,
                QObject* parent) final override
        {
            return deserialize_dyn(vis, [&] (auto&& deserializer)
            {
                auto layer = new LayerModel_T{
                                deserializer,
                                static_cast<Model_T&>(proc),
                                parent};

                return layer;
            });
        }

        LayerView_T* makeLayerView(
                const Process::LayerModel& viewmodel,
                QGraphicsItem* parent) final override
        {
            return new LayerView_T{parent};
        }

        LayerPresenter_T* makeLayerPresenter(
                const Process::LayerModel& lm,
                Process::LayerView* v,
                const Process::ProcessPresenterContext& context,
                QObject* parent) final override
        {
            return new LayerPresenter_T {
                m_colors.style(),
                safe_cast<const LayerModel_T&>(lm),
                safe_cast<LayerView_T*>(v),
                context,
                parent};
        }

        const UuidKey<Process::ProcessFactory>& concreteFactoryKey() const override \
        { return Metadata<ConcreteFactoryKey_k, Model_T>::get(); } \
        \
        QString prettyName() const override \
        { return Metadata<PrettyName_k, Model_T>::get(); } \

    private:
        CurveColors_T m_colors;
};

}

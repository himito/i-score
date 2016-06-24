#pragma once
#include <iscore/tools/Metadata.hpp>
#include <Process/LayerModel.hpp>
#include <Process/ModelMetadata.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <QtGlobal>
#include <QObject>
#include <nano_signal_slot.hpp>

#include <QString>
#include <functional>

#include <iscore/serialization/VisitorInterface.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_scenario_export.h>
class DataStream;
class JSONObject;
namespace Process { class ProcessModel; }

namespace Scenario
{
class ConstraintModel;
class RackModel;
// Note : the SlotModel is assumed to be in a Rack, itself in a Constraint.
class ISCORE_PLUGIN_SCENARIO_EXPORT SlotModel final :
        public IdentifiedObject<SlotModel>,
        public Nano::Observer
{
        Q_OBJECT
        ISCORE_SERIALIZE_FRIENDS(SlotModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(SlotModel, JSONObject)

        Q_PROPERTY(qreal getHeight
                   READ getHeight
                   WRITE setHeight
                   NOTIFY HeightChanged)

        Q_PROPERTY(bool focus
                   READ focus
                   WRITE setFocus
                   NOTIFY focusChanged)

    public:
        ModelMetadata metadata;
        SlotModel(const Id<SlotModel>& id,
                  const qreal slotHeight,
                  RackModel* parent);

        // Copy
        SlotModel(std::function<void(const SlotModel&, SlotModel&)> lmCopyMethod,
                  const SlotModel& source,
                  const Id<SlotModel>& id,
                  RackModel* parent);

        RackModel& rack() const;

        static void copyViewModelsInSameConstraint(const SlotModel&, SlotModel&);

        template<typename Impl>
        SlotModel(Deserializer<Impl>& vis, QObject* parent) :
            IdentifiedObject{vis, parent}
        {
            initConnections();
            vis.writeTo(*this);
        }

        virtual ~SlotModel() = default;

         // A process is selected for edition when it is
         // the edited process when the interface is clicked.
        void putToFront(
                const Id<Process::LayerModel>& layerId);
        const Process::LayerModel* frontLayerModel() const;

        // A slot is always in a constraint
        ConstraintModel& parentConstraint() const;

        qreal getHeight() const;
        bool focus() const;

        NotifyingMap<Process::LayerModel> layers;

        void on_deleteSharedProcessModel(const Process::ProcessModel& sharedProcessId);

        void setHeight(qreal arg);
        void setFocus(bool arg);
    signals:
        void layerModelPutToFront(const Process::LayerModel& layerModelId);

        void HeightChanged(qreal arg);
        void focusChanged(bool arg);

    private:
        void initConnections();

        void on_addLayer(const Process::LayerModel& viewmodel);
        void on_removeLayer(const Process::LayerModel&);

        Id<Process::LayerModel> m_frontLayerModelId;

        qreal m_height {200};
        bool m_focus{false};
};
ISCORE_PARAMETER_TYPE(SlotModel, Height)

/**
 * @brief parentConstraint Utility function to get the parent constraint of a process view model
 * @param lm Process view model pointer
 *
 * @return A pointer to the parent constraint if there is one, or nullptr.
 */
ConstraintModel* parentConstraint(Process::LayerModel* lm);
}

DEFAULT_MODEL_METADATA(Scenario::SlotModel, "Slot")


#pragma once
#include <iscore/tools/Metadata.hpp>
#include <Process/ModelMetadata.hpp>
#include <Process/TimeValue.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/serialization/VisitorInterface.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <QList>
#include <QObject>
#include <nano_signal_slot.hpp>

#include <QString>
#include <functional>

#include "Slot/SlotModel.hpp"
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore_plugin_scenario_export.h>
namespace Process { class ProcessModel; }


namespace Scenario
{
class ConstraintModel;
/**
 * @brief The RackModel class
 *
 * A Rack is a slot container.
 * A Rack is always found in a Constraint.
 */
class ISCORE_PLUGIN_SCENARIO_EXPORT RackModel final :
        public IdentifiedObject<RackModel>,
        public Nano::Observer
{
        Q_OBJECT

    public:
        ModelMetadata metadata;
        RackModel(const Id<RackModel>& id, QObject* parent);

        // Copy
        RackModel(const RackModel& source,
                 const Id<RackModel>& id,
                 std::function<void(const SlotModel&, SlotModel&)> lmCopyMethod,
                 QObject* parent);

        template<typename Impl>
        RackModel(Deserializer<Impl>& vis, QObject* parent) :
            IdentifiedObject{vis, parent}
        {
            initConnections();
            vis.writeTo(*this);
        }

        // A rack is necessarily child of a constraint.
        ConstraintModel& constraint() const;

        void addSlot(SlotModel* m, int position);
        void addSlot(SlotModel* m);  // No position : at the end

        void swapSlots(const Id<SlotModel>& firstslot,
                       const Id<SlotModel>& secondslot);

        int slotPosition(const Id<SlotModel>& slotId) const
        {
            return m_positions.indexOf(slotId);
        }

        const QList<Id<SlotModel>>& slotsPositions() const
        { return m_positions; }

        NotifyingMap<SlotModel> slotmodels;
    signals:
        void slotPositionsChanged();

        void on_deleteSharedProcessModel(const Process::ProcessModel&);
        void on_durationChanged(const TimeValue&);

    private:
        void initConnections();
        void on_slotRemoved(const SlotModel&);

        // Positions of the slots. First is topmost.
        QList<Id<SlotModel>> m_positions;
};
}
DEFAULT_MODEL_METADATA(Scenario::RackModel, "Rack")
Q_DECLARE_METATYPE(Id<Scenario::RackModel>)

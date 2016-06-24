#pragma once
#include <Process/TimeValue.hpp>
#include <Process/ExpandMode.hpp>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>

#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Process/Algorithms/StandardDisplacementPolicy.hpp>
#include <Scenario/Process/Algorithms/ProcessPolicy.hpp>

#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>

/*
 * Command to change a constraint duration by moving event. Vertical move is not allowed.
 */
namespace Scenario
{
class BaseScenario;
namespace Command
{
template<typename SimpleScenario_T>
class MoveBaseEvent final : public iscore::SerializableCommand
{
    private:
        template<typename ScaleFun>
        static void updateDuration(
                SimpleScenario_T& scenar,
                const TimeValue& newDuration,
                ScaleFun&& scaleMethod)
        {
            scenar.endEvent().setDate(newDuration);
            scenar.endTimeNode().setDate(newDuration);

            auto& constraint = scenar.constraint();
            ConstraintDurations::Algorithms::changeAllDurations(constraint, newDuration);
            for(auto& process : constraint.processes)
            {
                scaleMethod(process, newDuration);
            }
        }

    public:
        const CommandParentFactoryKey& parentKey() const override
        { return CommandFactoryName<SimpleScenario_T>(); }
        const CommandFactoryKey& key() const override
        { return static_key(); }
        QString description() const override
        { return QObject::tr("Move a %1 event").arg(Metadata<UndoName_k, SimpleScenario_T>::get()); }
        static const CommandFactoryKey& static_key()
        {
            static const CommandFactoryKey kagi{QString("MoveBaseEvent_") + Metadata<ObjectKey_k, SimpleScenario_T>::get()};
            return kagi;
        }

        MoveBaseEvent() = default;

        MoveBaseEvent(
                Path<SimpleScenario_T>&& path,
                const Id<EventModel>& event,
                const TimeValue& date,
                double y,
                ExpandMode mode) :
            m_path {std::move(path)},
            m_newDate {date},
            m_mode{mode}
        {
            auto& scenar = m_path.find();
            const auto& constraint = scenar.constraint();
            m_oldDate = constraint.duration.defaultDuration();

            // Save the constraint data
            QByteArray arr;
            Visitor<Reader<DataStream>> jr{&arr};
            jr.readFrom(constraint);

            // Save for each view model of this constraint
            // the identifier of the rack that was displayed
            QMap<Id<ConstraintViewModel>, Id<RackModel>> map;
            for(const ConstraintViewModel* vm : constraint.viewModels())
            {
                map.insert(vm->id(), vm->shownRack());
            }

            m_savedConstraint = {arr, map};
        }

        void undo() const override
        {
            auto& scenar = m_path.find();

            updateDuration(scenar,
                           m_oldDate,
                           [&] (Process::ProcessModel& p , const TimeValue& v) {
                // Nothing is needed since the processes will be replaced anyway.
            });


            // TODO do this only if we shrink.

            // Now we have to restore the state of each constraint that might have been modified
            // during this command.

            // 1. Clear the constraint
            ClearConstraint clearCmd{scenar.constraint()};
            clearCmd.redo();

            auto& constraint = scenar.constraint();
            // 2. Restore the rackes & processes.

            // TODO if possible refactor this with ReplaceConstraintContent and ConstraintModel::clone
            // Be careful however, the code differs in subtle ways
            ConstraintModel src_constraint{
                Deserializer<DataStream>{m_savedConstraint.first},
                &constraint}; // Temporary parent

            std::map<const Process::ProcessModel*, Process::ProcessModel*> processPairs;

            // Clone the processes
            for(const auto& sourceproc : src_constraint.processes)
            {
                auto newproc = sourceproc.clone(sourceproc.id(), &constraint);

                processPairs.insert(std::make_pair(&sourceproc, newproc));
                AddProcess(constraint, newproc);
            }

            // Clone the rackes
            for(const auto& sourcerack : src_constraint.racks)
            {
                // A note about what happens here :
                // Since we want to duplicate our process view models using
                // the target constraint's cloned shared processes (they might setup some specific data),
                // we maintain a pair mapping each original process to their cloned counterpart.
                // We can then use the correct cloned process to clone the process view model.
                auto newrack = new RackModel{
                        sourcerack,
                        sourcerack.id(),
                        [&] (const SlotModel& source, SlotModel& target)
                {
                    for(const auto& lm : source.layers)
                    {
                        // We can safely reuse the same id since it's in a different slot.
                        Process::ProcessModel* proc = processPairs[&lm.processModel()];
                        // TODO harmonize the order of parameters (source first, then new id)
                        target.layers.add(proc->cloneLayer(lm.id(), lm, &target));
                    }
                },
                &constraint};
                constraint.racks.add(newrack);
            }


            // 3. Restore the correct rackes in the constraint view models
            for(auto& viewmodel : constraint.viewModels())
            {
                viewmodel->showRack(m_savedConstraint.second[viewmodel->id()]);
            }
        }

        void redo() const override
        {
            auto& scenar = m_path.find();

            updateDuration(scenar,
                           m_newDate,
                           [&] (Process::ProcessModel& p , const TimeValue& v) {
                p.setParentDuration(m_mode, v);
            });
        }

        void update(
                unused_t,
                unused_t,
                const TimeValue& date,
                double,
                ExpandMode)
        {
            m_newDate = date;
        }

        const Path<SimpleScenario_T>& path() const
        { return m_path; }

    protected:
        void serializeImpl(DataStreamInput& s) const override
        {
            s << m_path
              << m_oldDate
              << m_newDate
              << (int)m_mode
              << m_savedConstraint;
        }
        void deserializeImpl(DataStreamOutput& s) override
        {
            int mode;
            s >> m_path
                    >> m_oldDate
                    >> m_newDate
                    >> mode
                    >> m_savedConstraint;
            m_mode = static_cast<ExpandMode>(mode);
        }

    private:
        Path<SimpleScenario_T> m_path;

        TimeValue m_oldDate {};
        TimeValue m_newDate {};

        ExpandMode m_mode{ExpandMode::Scale};

        QPair<
        QByteArray, // The constraint data
        QMap< // Mapping for the view models of this constraint
        Id<ConstraintViewModel>,
        Id<RackModel>
        >
        > m_savedConstraint;
};

}
}

ISCORE_COMMAND_DECL_T(MoveBaseEvent<Scenario::BaseScenario>)


#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <QByteArray>
#include <QPair>
#include <QVector>

#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModelIdMap.hpp>
#include <iscore/selection/Selection.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class ConstraintModel;
class EventModel;
class StateModel;
class TimeNodeModel;
class CommentBlockModel;
class ProcessModel;
namespace Command
{
/**
        * @brief The RemoveSelection class
        *
        * Tries to remove what is selected in a scenario.
        */
class RemoveSelection final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), RemoveSelection, "Remove selected elements")
        public:
            RemoveSelection(Path<Scenario::ProcessModel>&& scenarioPath, Selection sel);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput&) const override;
        void deserializeImpl(DataStreamOutput&) override;

    private:
        Path<Scenario::ProcessModel> m_path;

        // For timenodes that may be removed when there is only a single event
        QVector<QPair<Id<TimeNodeModel>, QByteArray>> m_maybeRemovedTimeNodes;


        QVector<QPair<Id<CommentBlockModel>, QByteArray>> m_removedComments;
        QVector<QPair<Id<StateModel>, QByteArray>> m_removedStates;
        QVector<QPair<Id<EventModel>, QByteArray>> m_removedEvents;
        QVector<QPair<Id<TimeNodeModel>, QByteArray>> m_removedTimeNodes;
        QVector<
        QPair<
        QPair<
        Id<ConstraintModel>,
        QByteArray
        >, SerializedConstraintViewModels
        >
        > m_removedConstraints; // The handlers inside the events are IN the constraints / Rackes / etc.

};
}
}

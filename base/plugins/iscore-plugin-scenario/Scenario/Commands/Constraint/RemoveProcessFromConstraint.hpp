#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <QByteArray>
#include <QPair>
#include <QVector>

#include <iscore/tools/SettableIdentifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;
namespace Process { class LayerModel; }
namespace Process { class ProcessModel; }

namespace Scenario
{
class ConstraintModel;
namespace Command
{
class RemoveProcessFromConstraint final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), RemoveProcessFromConstraint, "Remove a process")
        public:
            RemoveProcessFromConstraint(
                Path<ConstraintModel>&& constraintPath,
                Id<Process::ProcessModel> processId);
        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput&) const override;
        void deserializeImpl(DataStreamOutput&) override;

    private:
        Path<ConstraintModel> m_path;
        Id<Process::ProcessModel> m_processId;

        QByteArray m_serializedProcessData;
        QVector<QPair<Path<Process::LayerModel>, QByteArray>> m_serializedViewModels;
};
}
}

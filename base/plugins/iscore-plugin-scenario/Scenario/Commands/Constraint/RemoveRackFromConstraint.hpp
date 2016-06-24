#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <QByteArray>
#include <QMap>

#include <iscore/tools/SettableIdentifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class RackModel;
class ConstraintModel;
class ConstraintViewModel;
namespace Command
{
/**
         * @brief The RemoveRackFromConstraint class
         *
         * Removes a rack : all the slots and function views will be removed.
         */
class RemoveRackFromConstraint final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), RemoveRackFromConstraint, "Remove a rack")
        public:
            RemoveRackFromConstraint(
                Path<RackModel>&& rackPath);
        RemoveRackFromConstraint(
                Path<ConstraintModel>&& constraintPath,
                Id<RackModel> rackId);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput&) const override;
        void deserializeImpl(DataStreamOutput&) override;

    private:
        Path<ConstraintModel> m_path;
        Id<RackModel> m_rackId {};

        QByteArray m_serializedRackData; // Should be done in the constructor

        QMap<Id<ConstraintViewModel>, bool> m_rackMappings;
};
}
}

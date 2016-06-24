#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>

#include <tests/helpers/ForwardDeclaration.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>


namespace Scenario
{
class ConstraintModel;
namespace Command
{
/**
 * @brief The SetMaxDuration class
 *
 * Sets the Max duration of a Constraint
*/
class SetMaxDuration final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), SetMaxDuration, "Set constraint maximum")
    public:
            static const constexpr auto corresponding_member = &ConstraintDurations::maxDuration; // used by state machine (MoveState.hpp)

        SetMaxDuration(const ConstraintModel& cst, TimeValue newval, bool isInfinite):
        m_path{cst},
        m_oldVal{cst.duration.maxDuration()},
        m_newVal{std::move(newval)},
        m_newInfinite{isInfinite},
        m_oldInfinite{cst.duration.isMaxInfinite()}
        {

        }

        void update(const ConstraintModel& cst, const TimeValue &newval, bool isInfinite)
        {
            m_newVal = newval;
            auto& cstrDuration = cst.duration;
            if(m_newVal < cstrDuration.defaultDuration())
                m_newVal = cstrDuration.defaultDuration();
        }

        void undo() const override
        {
            m_path.find().duration.setMaxInfinite(m_oldInfinite);
            m_path.find().duration.setMaxDuration(m_oldVal);
        }

        void redo() const override
        {
            m_path.find().duration.setMaxInfinite(m_newInfinite);
            m_path.find().duration.setMaxDuration(m_newVal);
        }

    protected:
        void serializeImpl(DataStreamInput& s) const override
        {
            s << m_path << m_oldVal << m_newVal << m_newInfinite;
        }
        void deserializeImpl(DataStreamOutput& s) override
        {
            s >> m_path >> m_oldVal >> m_newVal >> m_newInfinite;
        }

    private:
        Path<ConstraintModel> m_path;

        TimeValue m_oldVal;
        TimeValue m_newVal;
        bool m_newInfinite;
        bool m_oldInfinite;
};
}
}

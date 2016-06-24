#pragma once
#include "ScenarioCreationState.hpp"

#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

#include <Scenario/Commands/Scenario/Creations/CreateState.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveNewEvent.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveNewState.hpp>

#include <Scenario/Commands/Scenario/Creations/CreateConstraint.hpp>

#include <Scenario/Palette/Transitions/NothingTransitions.hpp>
#include <Scenario/Palette/Transitions/AnythingTransitions.hpp>
#include <Scenario/Palette/Transitions/EventTransitions.hpp>
#include <Scenario/Palette/Transitions/ConstraintTransitions.hpp>
#include <Scenario/Palette/Transitions/TimeNodeTransitions.hpp>
#include <Scenario/Palette/Transitions/StateTransitions.hpp>

#include <Scenario/Palette/Tools/ScenarioRollbackStrategy.hpp>
#include <QApplication>
#include <QFinalState>


namespace Scenario
{
template<typename Scenario_T, typename ToolPalette_T>
class Creation_FromState final : public CreationState<Scenario_T, ToolPalette_T>
{
    public:
        Creation_FromState(
                const ToolPalette_T& stateMachine,
                const Path<Scenario_T>& scenarioPath,
                const iscore::CommandStackFacade& stack,
                QState* parent):
            CreationState<Scenario_T, ToolPalette_T>{stateMachine, stack, std::move(scenarioPath), parent}
        {
            using namespace Scenario::Command;
            auto finalState = new QFinalState{this};
            QObject::connect(finalState, &QState::entered, [&] ()
            {
                this->clearCreatedIds();
            });

            auto mainState = new QState{this};
            {
                auto pressed = new QState{mainState};
                auto released = new QState{mainState};
                auto move_nothing = new StrongQState<MoveOnNothing>{mainState};
                auto move_state = new StrongQState<MoveOnState>{mainState};
                auto move_event = new StrongQState<MoveOnEvent>{mainState};
                auto move_timenode = new StrongQState<MoveOnTimeNode>{mainState};

                // General setup
                mainState->setInitialState(pressed);
                released->addTransition(finalState);

                // Release
                iscore::make_transition<ReleaseOnAnything_Transition>(mainState, released);

                // Pressed -> ...
                iscore::make_transition<MoveOnNothing_Transition<Scenario_T>>(pressed, move_state, *this);
                iscore::make_transition<MoveOnNothing_Transition<Scenario_T>>(pressed, move_nothing, *this);

                /// MoveOnNothing -> ...
                // MoveOnNothing -> MoveOnNothing.
                iscore::make_transition<MoveOnNothing_Transition<Scenario_T>>(move_nothing, move_nothing, *this);

                // MoveOnNothing -> MoveOnState.
                this->add_transition(move_nothing, move_state,
                               [&] () { this->rollback(); createToState(); });

                // MoveOnNothing -> MoveOnEvent.
                this->add_transition(move_nothing, move_event,
                               [&] () { this->rollback(); createToEvent(); });

                // MoveOnNothing -> MoveOnTimeNode
                this->add_transition(move_nothing, move_timenode,
                               [&] () { this->rollback(); createToTimeNode(); });


                /// MoveOnState -> ...
                // MoveOnState -> MoveOnNothing
                this->add_transition(move_state, move_nothing,
                               [&] () { this->rollback(); createToNothing(); });

                // MoveOnState -> MoveOnState
                // We don't do anything, the constraint should not move.

                // MoveOnState -> MoveOnEvent
                this->add_transition(move_state, move_event,
                               [&] () { this->rollback(); createToEvent(); });

                // MoveOnState -> MoveOnTimeNode
                this->add_transition(move_state, move_timenode,
                               [&] () { this->rollback(); createToTimeNode(); });

                /// MoveOnEvent -> ...
                // MoveOnEvent -> MoveOnNothing
                this->add_transition(move_event, move_nothing,
                               [&] () { this->rollback(); createToNothing(); });

                // MoveOnEvent -> MoveOnState
                this->add_transition(move_event, move_state,
                               [&] () {
                    if(this->m_parentSM.model().state(this->clickedState).eventId() != this->m_parentSM.model().state(this->hoveredState).eventId())
                    {
                        this->rollback();
                        createToState();
                    }
                });

                // MoveOnEvent -> MoveOnEvent
                iscore::make_transition<MoveOnEvent_Transition<Scenario_T>>(move_event, move_event, *this);

                // MoveOnEvent -> MoveOnTimeNode
                this->add_transition(move_event, move_timenode,
                               [&] () { this->rollback(); createToTimeNode(); });


                /// MoveOnTimeNode -> ...
                // MoveOnTimeNode -> MoveOnNothing
                this->add_transition(move_timenode, move_nothing,
                               [&] () { this->rollback(); createToNothing(); });

                // MoveOnTimeNode -> MoveOnState
                this->add_transition(move_timenode, move_state,
                               [&] () { this->rollback(); createToState(); });

                // MoveOnTimeNode -> MoveOnEvent
                this->add_transition(move_timenode, move_event,
                               [&] () { this->rollback(); createToEvent(); });

                // MoveOnTimeNode -> MoveOnTimeNode
                iscore::make_transition<MoveOnTimeNode_Transition<Scenario_T>>(move_timenode , move_timenode , *this);



                // What happens in each state.
                QObject::connect(pressed, &QState::entered,
                                 [&] ()
                {
                    this->m_clickedPoint = this->currentPoint;
                    createToNothing();
                });

                QObject::connect(move_nothing, &QState::entered, [&] ()
                {
                    if(this->createdConstraints.empty() || this->createdEvents.empty())
                    {
                        this->rollback();
                        return;
                    }

                    // Magnetism handling :
                    // If we press "sequence"... we're always in sequence.
                    //
                    // Else, if we're < 0.005, we switch to "sequence"
                    // Else, we keep the normal state.
                    bool manual_sequence = qApp->keyboardModifiers() & Qt::ShiftModifier;
                    if(!manual_sequence)
                    {
                        auto sequence = this->m_parentSM.editionSettings().sequence();
                        auto magnetism_distance = std::abs(this->currentPoint.y - this->m_clickedPoint.y) < 0.005;
                        if(!sequence && magnetism_distance)
                        {
                            this->m_parentSM.editionSettings().setSequence(true);
                            this->rollback();
                            createToNothing();
                            return;
                        }
                        else if(sequence && !magnetism_distance)
                        {
                            this->m_parentSM.editionSettings().setSequence(false);
                            this->rollback();
                            createToNothing();
                            return;
                        }
                    }

                    auto sequence = this->m_parentSM.editionSettings().sequence();
                    if(sequence)
                    {
                        const auto& st = this->m_parentSM.model().state(this->clickedState);
                        this->currentPoint.y = st.heightPercentage();
                    }

                    if(this->currentPoint.date <= this->m_clickedPoint.date)
                    {
                        this->currentPoint.date = this->m_clickedPoint.date + TimeValue::fromMsecs(10);;
                    }

                    this->m_dispatcher.template submitCommand<MoveNewEvent>(
                                Path<Scenario_T>{this->m_scenarioPath},
                                this->createdConstraints.last(),
                                this->createdEvents.last(),
                                this->currentPoint.date,
                                this->currentPoint.y,
                                sequence);

                });

                QObject::connect(move_event, &QState::entered, [&] ()
                {
                    if(this->createdStates.empty())
                    {
                        this->rollback();
                        return;
                    }

                    if(this->currentPoint.date <= this->m_clickedPoint.date)
                    {
                        return;
                    }

                    this->m_dispatcher.template submitCommand<MoveNewState>(
                                Path<Scenario_T>{this->m_scenarioPath},
                                this->createdStates.last(),
                                this->currentPoint.y);
                });

                QObject::connect(move_timenode, &QState::entered, [&] ()
                {
                    if(this->createdStates.empty())
                    {
                        this->rollback();
                        return;
                    }

                    if(this->currentPoint.date <= this->m_clickedPoint.date)
                    {
                        return;
                    }

                    this->m_dispatcher.template submitCommand<MoveNewState>(
                                Path<Scenario_T>{this->m_scenarioPath},
                                this->createdStates.last(),
                                this->currentPoint.y);
                });

                QObject::connect(released, &QState::entered, [&] ()
                {
                    this->makeSnapshot();
                    this->m_dispatcher.template commit<Scenario::Command::CreationMetaCommand>();
                });
            }

            auto rollbackState = new QState{this};
            iscore::make_transition<iscore::Cancel_Transition>(mainState, rollbackState);
            rollbackState->addTransition(finalState);
            QObject::connect(rollbackState, &QState::entered, [&] ()
            {
                this->rollback();
            });

            this->setInitialState(mainState);
        }

    private:
        template<typename Fun>
        void creationCheck(Fun&& fun)
        {
            const auto& scenar = this->m_parentSM.model();
            if(!this->m_parentSM.editionSettings().sequence())
            {
                // Create new state at the beginning
                auto cmd = new Scenario::Command::CreateState{
                        this->m_scenarioPath,
                        scenar.state(this->clickedState).eventId(),
                        this->currentPoint.y};
                this->m_dispatcher.submitCommand(cmd);

                this->createdStates.append(cmd->createdState());
                fun(this->createdStates.first());
            }
            else
            {
                const auto& st = scenar.states.at(this->clickedState);
                if(!st.nextConstraint()) // TODO & deltaY < deltaX
                {
                    this->currentPoint.y = st.heightPercentage();
                    fun(this->clickedState);
                }
                else
                {
                    ISCORE_TODO;
                    // create a single state on the same event (deltaY > deltaX)
                }
            }
        }

        // Note : clickedEvent is set at startEvent if clicking in the background.
        void createToNothing()
        {
            creationCheck([&] (const Id<StateModel>& id) { this->createToNothing_base(id); });
        }

        void createToTimeNode()
        {
            creationCheck([&] (const Id<StateModel>& id) { this->createToTimeNode_base(id); });
        }

        void createToEvent()
        {
            if(this->hoveredEvent == this->m_parentSM.model().state(this->clickedState).eventId())
            {
                creationCheck([&] (const Id<StateModel>& id) { });
            }
            else
            {
                creationCheck([&] (const Id<StateModel>& id) { this->createToEvent_base(id); });
            }
        }

        void createToState()
        {
            if(!this->m_parentSM.model().states.at(this->hoveredState).previousConstraint())
            {
                // No previous constraint -> we create a new constraint and link it to this state
                creationCheck([&] (const Id<StateModel>& id) { this->createToState_base(id); });
            }
            else
            {
                // Previous constraint -> we add a new state to the event and link to it.
                this->hoveredEvent = this->m_parentSM.model().states.at(this->hoveredState).eventId();
                creationCheck([&] (const Id<StateModel>& id) { this->createToEvent_base(id); });
            }
        }


};
}

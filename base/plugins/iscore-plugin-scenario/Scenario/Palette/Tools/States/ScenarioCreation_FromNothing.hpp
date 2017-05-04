#pragma once
#include "ScenarioCreationState.hpp"
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

#include <Scenario/Commands/Scenario/Displacement/MoveNewEvent.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveNewState.hpp>

#include <Scenario/Commands/Scenario/Creations/CreateEvent_State.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateState.hpp>

#include <Scenario/Palette/Transitions/AnythingTransitions.hpp>
#include <Scenario/Palette/Transitions/ConstraintTransitions.hpp>
#include <Scenario/Palette/Transitions/EventTransitions.hpp>
#include <Scenario/Palette/Transitions/NothingTransitions.hpp>
#include <Scenario/Palette/Transitions/StateTransitions.hpp>
#include <Scenario/Palette/Transitions/TimeNodeTransitions.hpp>

#include <QFinalState>

namespace Scenario
{
template <typename Scenario_T, typename ToolPalette_T>
class Creation_FromNothing final
    : public CreationState<Scenario_T, ToolPalette_T>
{
public:
  Creation_FromNothing(
      const ToolPalette_T& stateMachine,
      const Path<Scenario_T>& scenarioPath,
      const iscore::CommandStackFacade& stack,
      QState* parent)
      : CreationState<Scenario_T, ToolPalette_T>{stateMachine, stack,
                                                 scenarioPath, parent}
  {
    this->setObjectName("ScenarioCreation_FromNothing");
    using namespace Scenario::Command;
    auto finalState = new QFinalState{this};
    QObject::connect(
        finalState, &QState::entered, [&]() { this->clearCreatedIds(); });

    auto mainState = new QState{this};
    mainState->setObjectName("Main state");
    {
      auto pressed = new QState{mainState};
      auto released = new QState{mainState};
      auto move_nothing = new StrongQState<MoveOnNothing>{mainState};
      auto move_state = new StrongQState<MoveOnState>{mainState};
      auto move_event = new StrongQState<MoveOnEvent>{mainState};
      auto move_timenode = new StrongQState<MoveOnTimeNode>{mainState};

      pressed->setObjectName("Pressed");
      released->setObjectName("Released");
      move_nothing->setObjectName("Move on Nothing");
      move_state->setObjectName("Move on State");
      move_event->setObjectName("Move on Event");
      move_timenode->setObjectName("Move on TimeNode");

      // General setup
      mainState->setInitialState(pressed);
      released->addTransition(finalState);

      // Release
      iscore::make_transition<ReleaseOnAnything_Transition>(
          mainState, released);

      // Pressed -> ...
      iscore::make_transition<MoveOnNothing_Transition<Scenario_T>>(
          pressed, move_nothing, *this);

      /// MoveOnNothing -> ...
      // MoveOnNothing -> MoveOnNothing.
      iscore::make_transition<MoveOnNothing_Transition<Scenario_T>>(
          move_nothing, move_nothing, *this);

      // MoveOnNothing -> MoveOnState.
      this->add_transition(move_nothing, move_state, [&]() {
        this->rollback();
        createToState();
      });

      // MoveOnNothing -> MoveOnEvent.
      this->add_transition(move_nothing, move_event, [&]() {
        this->rollback();
        createToEvent();
      });

      // MoveOnNothing -> MoveOnTimeNode
      this->add_transition(move_nothing, move_timenode, [&]() {
        this->rollback();
        createToTimeNode();
      });

      /// MoveOnState -> ...
      // MoveOnState -> MoveOnNothing
      this->add_transition(move_state, move_nothing, [&]() {
        this->rollback();
        createToNothing();
      });

      // MoveOnState -> MoveOnState
      // We don't do anything, the constraint should not move.

      // MoveOnState -> MoveOnEvent
      this->add_transition(move_state, move_event, [&]() {
        this->rollback();
        createToEvent();
      });

      // MoveOnState -> MoveOnTimeNode
      this->add_transition(move_state, move_timenode, [&]() {
        this->rollback();
        createToTimeNode();
      });

      /// MoveOnEvent -> ...
      // MoveOnEvent -> MoveOnNothing
      this->add_transition(move_event, move_nothing, [&]() {
        this->rollback();
        createToNothing();
      });

      // MoveOnEvent -> MoveOnState
      this->add_transition(move_event, move_state, [&]() {
        this->rollback();
        createToState();
      });

      // MoveOnEvent -> MoveOnEvent
      iscore::make_transition<MoveOnEvent_Transition<Scenario_T>>(
          move_event, move_event, *this);

      // MoveOnEvent -> MoveOnTimeNode
      this->add_transition(move_event, move_timenode, [&]() {
        this->rollback();
        createToTimeNode();
      });

      /// MoveOnTimeNode -> ...
      // MoveOnTimeNode -> MoveOnNothing
      this->add_transition(move_timenode, move_nothing, [&]() {
        this->rollback();
        createToNothing();
      });

      // MoveOnTimeNode -> MoveOnState
      this->add_transition(move_timenode, move_state, [&]() {
        this->rollback();
        createToState();
      });

      // MoveOnTimeNode -> MoveOnEvent
      this->add_transition(move_timenode, move_event, [&]() {
        this->rollback();
        createToEvent();
      });

      // MoveOnTimeNode -> MoveOnTimeNode
      iscore::make_transition<MoveOnTimeNode_Transition<Scenario_T>>(
          move_timenode, move_timenode, *this);

      // What happens in each state.
      QObject::connect(pressed, &QState::entered, [&]() {
        this->m_clickedPoint = this->currentPoint;
        this->clickedEvent = this->m_parentSM.model().startEvent().id();
        createToNothing();
      });

      QObject::connect(move_nothing, &QState::entered, [&]() {
        if (this->createdConstraints.empty() || this->createdEvents.empty())
        {
          this->rollback();
          return;
        }

        if (this->clickedEvent != this->m_parentSM.model().startEvent().id()
         && this->currentPoint.date <= this->m_clickedPoint.date)
        {
          this->currentPoint.date
              = this->m_clickedPoint.date + TimeVal::fromMsecs(10);
        }
        else if (this->clickedEvent == this->m_parentSM.model().startEvent().id()
              && this->currentPoint.date <= TimeVal::fromMsecs(10))
        {
          this->currentPoint.date = TimeVal::fromMsecs(10);
        }

        this->m_dispatcher.template submitCommand<MoveNewEvent>(
            Path<Scenario_T>{this->m_scenarioPath},
            this->createdConstraints.last(),
            this->createdEvents.last(),
            this->currentPoint.date,
            this->currentPoint.y,
            stateMachine.editionSettings().sequence());
      });

      QObject::connect(move_timenode, &QState::entered, [&]() {
        if (this->createdStates.empty())
        {
          this->rollback();
          return;
        }

        if (this->currentPoint.date <= this->m_clickedPoint.date)
        {
          return;
        }

        this->m_dispatcher.template submitCommand<MoveNewState>(
            Path<Scenario_T>{this->m_scenarioPath},
            this->createdStates.last(),
            this->currentPoint.y);
      });

      QObject::connect(move_event, &QState::entered, [&]() {
        if (this->createdStates.empty())
        {
          this->rollback();
          return;
        }

        if (this->currentPoint.date <= this->m_clickedPoint.date)
        {
          return;
        }

        this->m_dispatcher.template submitCommand<MoveNewState>(
            Path<Scenario_T>{this->m_scenarioPath},
            this->createdStates.last(),
            this->currentPoint.y);
      });

      QObject::connect(
          released, &QState::entered, this, &Creation_FromNothing::commit);
    }

    auto rollbackState = new QState{this};
    rollbackState->setObjectName("Rollback");
    iscore::make_transition<iscore::Cancel_Transition>(
        mainState, rollbackState);
    rollbackState->addTransition(finalState);

    QObject::connect(
        rollbackState, &QState::entered, this,
        &Creation_FromNothing::rollback);

    this->setInitialState(mainState);
  }

private:
  void createInitialState()
  {
    if (this->clickedEvent)
    {
      auto cmd = new Scenario::Command::CreateState{
          this->m_scenarioPath, *this->clickedEvent, this->currentPoint.y};
      this->m_dispatcher.submitCommand(cmd);

      this->createdStates.append(cmd->createdState());
    }
  }

  void createToNothing()
  {
    createInitialState();
    this->createToNothing_base(this->createdStates.first());
  }
  void createToState()
  {
    ISCORE_ASSERT(bool(this->hoveredState));

    const auto& state
        = this->m_parentSM.model().states.at(*this->hoveredState);
    if (!bool(state.previousConstraint()))
    {
      createInitialState();
      this->createToState_base(this->createdStates.first());
    }
  }
  void createToEvent()
  {
    if (this->hoveredEvent != this->clickedEvent)
    {
      createInitialState();
      this->createToEvent_base(this->createdStates.first());
    }
  }
  void createToTimeNode()
  {
    createInitialState();
    this->createToTimeNode_base(this->createdStates.first());
  }
};
}

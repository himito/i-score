#pragma once
#include "ScenarioToolState.hpp"
#include <Scenario/Process/Temporal/StateMachines/ScenarioStateMachineBaseStates.hpp>
class EventPresenter;
class TimeNodePresenter;
class ScenarioCreation_FromNothing;
class ScenarioCreation_FromEvent;
class ScenarioCreation_FromState;
class ScenarioCreation_FromTimeNode;
class CreationToolState final : public ScenarioTool
{
    public:
        CreationToolState(ScenarioStateMachine& sm);

        void on_pressed(QPointF scene, ScenarioPoint sp);
        void on_moved(QPointF scene, ScenarioPoint sp);
        void on_released(QPointF scene, ScenarioPoint sp);
    private:
        // Return the colliding elements that were not created in the current commands
        QList<Id<StateModel>> getCollidingStates(QPointF, const QVector<Id<StateModel>>& createdStates);
        QList<Id<EventModel>> getCollidingEvents(QPointF, const QVector<Id<EventModel>>& createdEvents);
        QList<Id<TimeNodeModel>> getCollidingTimeNodes(QPointF, const QVector<Id<TimeNodeModel>>& createdTimeNodes);

        CreationState* currentState() const;

        template<typename StateFun,
                 typename EventFun,
                 typename TimeNodeFun,
                 typename NothingFun>
        void mapWithCollision(
                QPointF point,
                StateFun st_fun,
                EventFun ev_fun,
                TimeNodeFun tn_fun,
                NothingFun nothing_fun,
                const QVector<Id<StateModel>>& createdStates,
                const QVector<Id<EventModel>>& createdEvents,
                const QVector<Id<TimeNodeModel>>& createdTimeNodes)
        {
            auto collidingStates = getCollidingStates(point, createdStates);
            if(!collidingStates.empty())
            {
                st_fun(collidingStates.first());
                return;
            }

            auto collidingEvents = getCollidingEvents(point, createdEvents);
            if(!collidingEvents.empty())
            {
                ev_fun(collidingEvents.first());
                return;
            }

            auto collidingTimeNodes = getCollidingTimeNodes(point, createdTimeNodes);
            if(!collidingTimeNodes.empty())
            {
                tn_fun(collidingTimeNodes.first());
                return;
            }

            nothing_fun();
        }

        ScenarioCreation_FromNothing* m_createFromNothingState{};
        ScenarioCreation_FromEvent* m_createFromEventState{};
        ScenarioCreation_FromTimeNode* m_createFromTimeNodeState{};
        ScenarioCreation_FromState* m_createFromStateState{};
        QState* m_waitState{};
};

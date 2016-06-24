#include <Editor/TimeConstraint.h>
#include <Editor/TimeEvent.h>
#include <Editor/TimeNode.h>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <OSSIA/iscore2OSSIA.hpp>
#include <algorithm>
#include <vector>

#include "BaseScenarioElement.hpp"
#include "Editor/State.h"
#include "Editor/StateElement.h"
#include "Editor/TimeValue.h"
#include <OSSIA/Executor/ConstraintElement.hpp>
#include <OSSIA/Executor/EventElement.hpp>
#include <OSSIA/Executor/StateElement.hpp>
#include <OSSIA/Executor/TimeNodeElement.hpp>
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

#include <OSSIA/Executor/ExecutorContext.hpp>

#include <OSSIA/OSSIA2iscore.hpp>


namespace RecreateOnPlay
{
BaseScenarioElement::BaseScenarioElement(
        BaseScenarioRefContainer element,
        const Context& ctx,
        QObject *parent):
    QObject{parent},
    m_ctx{ctx}
{
    auto main_start_node = OSSIA::TimeNode::create();
    auto main_end_node = OSSIA::TimeNode::create();

    auto main_start_event_it = main_start_node->emplace(main_start_node->timeEvents().begin(), [this] (auto&&...) { });
    auto main_end_event_it = main_end_node->emplace(main_end_node->timeEvents().begin(), [this] (auto&&...) { });
    auto main_start_state = OSSIA::State::create();
    auto main_end_state = OSSIA::State::create();

    (*main_start_event_it)->addState(main_start_state);
    (*main_end_event_it)->addState(main_end_state);

    // TODO PlayDuration of base constraint.
    // TODO PlayDuration of FullView
    auto main_constraint = OSSIA::TimeConstraint::create(
                                [] (auto&&...) {},
                               *main_start_event_it,
                               *main_end_event_it,
                               iscore::convert::time(element.constraint().duration.defaultDuration()),
                               iscore::convert::time(element.constraint().duration.minDuration()),
                               iscore::convert::time(element.constraint().duration.maxDuration()));

    m_ossia_startTimeNode = new TimeNodeElement{main_start_node, element.startTimeNode(),  m_ctx.devices.list(), this};
    m_ossia_endTimeNode = new TimeNodeElement{main_end_node, element.endTimeNode(), m_ctx.devices.list(), this};

    m_ossia_startEvent = new EventElement{*main_start_event_it, element.startEvent(), m_ctx.devices.list(), this};
    m_ossia_endEvent = new EventElement{*main_end_event_it, element.endEvent(), m_ctx.devices.list(), this};

    m_ossia_startState = new StateElement{element.startState(), main_start_state, m_ctx, this};
    m_ossia_endState = new StateElement{element.endState(), main_end_state, m_ctx, this};

    m_ossia_constraint = new ConstraintElement{main_constraint, element.constraint(), m_ctx, this};

    main_constraint->setExecutionStatusCallback(
                [=] (OSSIA::Clock::ClockExecutionStatus c)
    {
        if(c == OSSIA::Clock::ClockExecutionStatus::STOPPED)
        {
            auto accumulator = OSSIA::State::create();
            flattenAndFilter(main_end_state, accumulator);
            accumulator->launch();

            emit finished();
        }
    });
}

ConstraintElement *BaseScenarioElement::baseConstraint() const
{
    return m_ossia_constraint;
}

TimeNodeElement *BaseScenarioElement::startTimeNode() const
{
    return m_ossia_startTimeNode;
}

TimeNodeElement *BaseScenarioElement::endTimeNode() const
{
    return m_ossia_endTimeNode;
}

EventElement *BaseScenarioElement::startEvent() const
{
    return m_ossia_startEvent;
}

EventElement *BaseScenarioElement::endEvent() const
{
    return m_ossia_endEvent;
}

StateElement *BaseScenarioElement::startState() const
{
    return m_ossia_startState;
}

StateElement *BaseScenarioElement::endState() const
{
    return m_ossia_endState;
}

}

BaseScenarioRefContainer::BaseScenarioRefContainer(
        Scenario::ConstraintModel& constraint,
        Scenario::ScenarioInterface& s):
    m_constraint{constraint},
    m_startState{s.state(constraint.startState())},
    m_endState{s.state(constraint.endState())},
    m_startEvent{s.event(m_startState.eventId())},
    m_endEvent{s.event(m_endState.eventId())},
    m_startNode{s.timeNode(m_startEvent.timeNode())},
    m_endNode{s.timeNode(m_endEvent.timeNode())}
{

}

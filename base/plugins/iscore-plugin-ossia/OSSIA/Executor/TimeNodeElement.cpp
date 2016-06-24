#include <Editor/TimeNode.h>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
#include <OSSIA/iscore2OSSIA.hpp>
#include <QDebug>
#include <exception>

#include "Editor/Expression.h"
#include "TimeNodeElement.hpp"
#include "ConstraintElement.hpp"

namespace RecreateOnPlay
{
TimeNodeElement::TimeNodeElement(
        std::shared_ptr<OSSIA::TimeNode> ossia_tn,
        const Scenario::TimeNodeModel& element,
        const Device::DeviceList& devlist,
        QObject* parent):
    QObject{parent},
    m_ossia_node{ossia_tn},
    m_iscore_node{element},
    m_deviceList{devlist}
{
    if(element.trigger() && element.trigger()->active())
    {
        try
        {
            auto expr = iscore::convert::expression(
                            element.trigger()->expression(),
                            m_deviceList);

            m_ossia_node->setExpression(expr);
        }
        catch(std::exception& e)
        {
            qDebug() << e.what();
            m_ossia_node->setExpression(OSSIA::Expression::create(true));
        }
    }
    connect(m_iscore_node.trigger(), &Scenario::TriggerModel::triggeredByGui,
            this, [&] () {
        try {

            // Also launch the relevant states when triggerring by hand.
            // Note : this will also launch the states of false conditions.
            auto accumulator = OSSIA::State::create();
            for(auto& ev : m_ossia_node->timeEvents())
            {
                OSSIA::TimeEvent& e = *ev;
                flattenAndFilter(e.getState(), accumulator);
            }
            accumulator->launch();

            m_ossia_node->trigger();
        }
        catch(...)
        {

        }
    });
}

std::shared_ptr<OSSIA::TimeNode> TimeNodeElement::OSSIATimeNode() const
{
    return m_ossia_node;
}

const Scenario::TimeNodeModel&TimeNodeElement::iscoreTimeNode() const
{
    return m_iscore_node;
}

}

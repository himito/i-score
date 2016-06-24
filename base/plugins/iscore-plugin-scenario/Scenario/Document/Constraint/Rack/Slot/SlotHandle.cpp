#include <Process/Style/ScenarioStyle.hpp>
#include <qnamespace.h>
#include <QPainter>
#include <QPoint>
#include <QCursor>

#include "SlotHandle.hpp"
#include "SlotView.hpp"

class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
SlotHandle::SlotHandle(const SlotView &slotView, QGraphicsItem *parent):
    QGraphicsItem{parent},
    m_slotView{slotView},
    m_width{slotView.boundingRect().width()}
{
    this->setCursor(Qt::SizeVerCursor);

    m_pen.setColor(ScenarioStyle::instance().SlotHandle.getColor());
    m_pen.setCosmetic(true);
    m_pen.setWidth(0);
}

QRectF SlotHandle::boundingRect() const
{
    return {0, 0, m_width, handleHeight()};
}

void SlotHandle::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(m_pen);
    painter->drawLine(QPointF{0., 0.}, QPointF{m_width, 0.});
}

void SlotHandle::setWidth(qreal width)
{
    m_width = width;
    prepareGeometryChange();
}
}

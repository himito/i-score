#pragma once
#include <QGraphicsItem>
#include <Scenario/Document/Constraint/ConstraintView.hpp>
#include <QPainter>
#include <Process/Style/ScenarioStyle.hpp>
#include <QPen>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>

namespace Scenario
{

class ConstraintMenuOverlay final :
    public QGraphicsItem
{

public:
  ConstraintMenuOverlay(ConstraintView* parent):
    QGraphicsItem{parent}
  {

  }

  QRectF boundingRect() const override
  {
    return {0, -10, 20, 20};
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
  {

    auto& skin = ScenarioStyle::instance();
    auto cst = static_cast<ConstraintView*>(parentItem());
    auto col = cst->constraintColor(skin);
    painter->setPen(col.color());
    painter->setBrush(col);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawChord(0, 0, 20, 20, 5760 / 2, -5760 / 2);

    painter->setBrush(skin.EventPending.getColor());
    const auto bright = skin.EventPending.getColor().color();
    QPen p{bright.darker(300)};
    p.setWidth(2);
    painter->setPen(p);

    const QLineF l1{QPointF{10, 2}, QPointF{10, 8}};
    const QLineF l2{QPointF{7, 5}, QPointF{13, 5}};
    painter->drawLine(l1.translated(1,1));
    painter->drawLine(l2.translated(1,1));
    p.setColor(bright);
    painter->setPen(p);
    painter->drawLine(l1);
    painter->drawLine(l2);
    painter->setBrush(QBrush());
  }

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override
  {
    auto cst = static_cast<ConstraintView*>(parentItem());
    emit cst->requestOverlayMenu(event->pos());
    event->accept();
  }
};

}

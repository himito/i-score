#include "TextItem.hpp"

#include <QGraphicsSceneMouseEvent>

namespace Scenario
{

TextItem::TextItem(QString text, QGraphicsItem* parent)
    : QGraphicsTextItem{text, parent}
{
  this->setFlag(QGraphicsItem::ItemIsFocusable);
  this->setDefaultTextColor(Qt::white);
}

void TextItem::focusOutEvent(QFocusEvent* event)
{
  emit focusOut();
}

void SimpleTextItem::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  //    setPen(m_color.getColor()); -> if enabled, there will be undesirable
  //    antialiasing
  setBrush(m_color.getColor());
  QGraphicsSimpleTextItem::paint(painter, option, widget);
}
}

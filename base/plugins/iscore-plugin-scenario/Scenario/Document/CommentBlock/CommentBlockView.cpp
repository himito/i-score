#include "CommentBlockView.hpp"
#include "TextItem.hpp"

#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTextCursor>
#include <QTextDocument>
#include <QWidget>

#include <Scenario/Document/CommentBlock/CommentBlockPresenter.hpp>
#include <cmath>
namespace Scenario
{
CommentBlockView::CommentBlockView(
    CommentBlockPresenter& presenter, QGraphicsItem* parent)
    : QGraphicsItem{parent}, m_presenter{presenter}
{
  this->setParentItem(parent);
  this->setZValue(ZPos::Comment);
  this->setAcceptHoverEvents(true);

  m_textItem = new TextItem{"", this};

  connect(
      m_textItem->document(), &QTextDocument::contentsChanged, this,
      [&]() { this->prepareGeometryChange(); });
  connect(m_textItem, &TextItem::focusOut, this, &CommentBlockView::focusOut);
  focusOut();
}

void CommentBlockView::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  auto p = QPen{Qt::white};
  p.setWidth(1.);
  painter->setPen(p);
  painter->drawRoundedRect(boundingRect(), 5, 5);
}

QRectF CommentBlockView::boundingRect() const
{
  if (m_textItem)
  {
    auto rect = m_textItem->boundingRect();
    rect.translate(-3, -3);
    rect.setWidth(rect.width() + 6);
    rect.setHeight(rect.height() + 6);
    return rect;
  }
  else
    return {-1., -1., 2., 2.};
}

void CommentBlockView::setSelected(bool b)
{
  if (m_selected == b)
    return;

  m_selected = b;
}

void CommentBlockView::setHtmlContent(QString htmlText)
{
  m_textItem->setHtml(htmlText);
}

void CommentBlockView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::MouseButton::LeftButton)
  {
    m_clickedPoint = event->scenePos() - this->pos();
    m_clickedScenePoint = event->scenePos();
  }
}

void CommentBlockView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  emit m_presenter.moved(event->scenePos() - m_clickedPoint);
}

void CommentBlockView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  auto p = event->scenePos();
  auto d = (m_clickedScenePoint - p).manhattanLength();
  if (std::abs(d) < 5)
    emit m_presenter.selected();

  emit m_presenter.released(event->scenePos());
}

void CommentBlockView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* evt)
{
  focusOnText();
}

void CommentBlockView::focusOnText()
{
  if (m_textItem->textInteractionFlags() == Qt::NoTextInteraction)
  {
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_textItem->setFocus(Qt::MouseFocusReason);
    QTextCursor c = m_textItem->textCursor();
    c.select(QTextCursor::Document);
    m_textItem->setTextCursor(c);
  }
}

void CommentBlockView::focusOut()
{
  m_textItem->setTextInteractionFlags(Qt::NoTextInteraction);
  QTextCursor c = m_textItem->textCursor();
  c.clearSelection();
  m_textItem->setTextCursor(c);
  clearFocus();
  emit m_presenter.editFinished(m_textItem->toHtml());
}
}

#pragma once
#include <Midi/MidiProcess.hpp>
#include <Process/LayerView.hpp>
#include <QPainterPath>

namespace Midi
{
class NoteView;
class View final : public Process::LayerView
{
  Q_OBJECT
public:
  View(QGraphicsItem* parent);

  ~View();

signals:
  void askContextMenu(const QPoint&, const QPointF&);
  void pressed();
  void doubleClicked(QPointF);
  void deleteRequested();

private:
  void paint_impl(QPainter*) const override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent*) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) override;
  void keyPressEvent(QKeyEvent*) override;

  QPainterPath m_selectArea;
};

NoteData noteAtPos(QPointF point, const QRectF& rect);
}

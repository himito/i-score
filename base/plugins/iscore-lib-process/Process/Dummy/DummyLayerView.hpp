#pragma once
#include <Process/LayerView.hpp>
#include <QGraphicsSimpleTextItem>
#include <QString>
#include <iscore_lib_process_export.h>

class QGraphicsItem;
class QPainter;
class QQuickView;
class QQuickItem;

namespace Dummy
{
class DummyTextItem;
class ISCORE_LIB_PROCESS_EXPORT DummyLayerView final
    : public Process::LayerView
{
  Q_OBJECT
public:
  explicit DummyLayerView(QGraphicsItem* parent);

  void setText(const QString& text);

signals:
  void pressed();

private:
  void updateText();
  void paint_impl(QPainter*) const override;
  void mousePressEvent(QGraphicsSceneMouseEvent*) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
  DummyTextItem* m_text{};
  /*
  QQuickView* m_view{};
  QQuickItem* m_item{};
  */
};
}

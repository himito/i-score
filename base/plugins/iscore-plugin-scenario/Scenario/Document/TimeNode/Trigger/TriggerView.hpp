#pragma once
#include <QGraphicsSvgItem>
#include <QRect>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>

class QGraphicsSceneMouseEvent;
class QGraphicsSvgItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
class TriggerView final : public QGraphicsSvgItem
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsItem)

public:
  TriggerView(QGraphicsItem* parent);

  static constexpr int static_type()
  {
    return QGraphicsItem::UserType + ItemType::Trigger;
  }
  int type() const override
  {
    return static_type();
  }

signals:
  void pressed();

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent*) override;

private:
  QGraphicsSvgItem* m_item{};
};
}

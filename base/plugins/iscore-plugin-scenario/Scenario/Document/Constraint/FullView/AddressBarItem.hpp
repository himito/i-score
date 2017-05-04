#pragma once
#include <QGraphicsItem>
#include <QList>
#include <QRect>

#include <iscore/model/path/ObjectPath.hpp>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
class ConstraintModel;
class ClickableLabelItem;
class AddressBarItem final : public QObject, public QGraphicsItem
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsItem)
public:
  AddressBarItem(QGraphicsItem* parent);

  double width() const;
  void setTargetObject(ObjectPath&&);

  QRectF boundingRect() const override;
  void paint(
      QPainter* painter, const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

signals:
  void needRedraw();
  void constraintSelected(ConstraintModel& cst);

private:
  void redraw();
  QList<QGraphicsItem*> m_items;
  ObjectPath m_currentPath;

  double m_width{};
};
}

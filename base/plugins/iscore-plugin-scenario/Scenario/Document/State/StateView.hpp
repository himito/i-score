#pragma once
#include <QColor>
#include <QGraphicsItem>
#include <QRect>
#include <QtGlobal>
#include <iscore/model/ColorReference.hpp>

#include <Scenario/Document/Event/ExecutionStatus.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>
#include <iscore_plugin_scenario_export.h>

class QGraphicsSceneDragDropEvent;
class QGraphicsSceneMouseEvent;
class QMimeData;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
class StateMenuOverlay;
class StatePresenter;

class ISCORE_PLUGIN_SCENARIO_EXPORT StateView final : public QObject,
                                                      public QGraphicsItem
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsItem)
public:
  StateView(StatePresenter& presenter, QGraphicsItem* parent = nullptr);
  virtual ~StateView() = default;

  static constexpr int static_type()
  {
    return QGraphicsItem::UserType + ItemType::State;
  }
  int type() const override
  {
    return static_type();
  }

  const StatePresenter& presenter() const
  {
    return m_presenter;
  }

  QRectF boundingRect() const override
  {
    auto radius = m_radiusFull * m_dilatationFactor;
    return {-radius, -radius, 2 * radius, 2 * radius};
  }

  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void setContainMessage(bool);
  void setSelected(bool arg);

  void changeColor(iscore::ColorRef);
  void setStatus(ExecutionStatus);

signals:
  void dropReceived(const QMimeData*);
  void startCreateMode();

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;

  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
  void setDilatation(double);
  StatePresenter& m_presenter;
  StateMenuOverlay* m_overlay{};

  bool m_containMessage{false};
  bool m_selected{false};

  iscore::ColorRef m_color;

  ExecutionStatusProperty m_status{};

  static const constexpr qreal m_radiusFull = 6.;
  static const constexpr qreal m_radiusPoint = 3.5;
  qreal m_dilatationFactor = 1;
};
}

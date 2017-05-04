#include <Process/Style/ScenarioStyle.hpp>
#include <QColor>
#include <QCursor>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QtGlobal>
#include <qnamespace.h>

#include "FullViewConstraintPresenter.hpp"
#include "FullViewConstraintView.hpp"
#include <Scenario/Document/Constraint/ConstraintView.hpp>

class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
FullViewConstraintView::FullViewConstraintView(
    FullViewConstraintPresenter& presenter, QGraphicsItem* parent)
    : ConstraintView{presenter, parent}
{
  this->setCacheMode(QGraphicsItem::NoCache);
  this->setParentItem(parent);
  this->setFlag(ItemIsSelectable);

  this->setZValue(1);
  this->setY(2 * constraintAndRackHeight());
}

void FullViewConstraintView::updatePaths()
{
  update();
}

QRectF FullViewConstraintView::boundingRect() const
{
  return {0, 0, qreal(std::max(defaultWidth(), maxWidth())) + 3, qreal(constraintAndRackHeight()) + 3};
}

void FullViewConstraintView::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  auto& skin = ScenarioStyle::instance();
  const qreal min_w = minWidth();
  const qreal max_w = maxWidth();
  const qreal def_w = defaultWidth();
  const qreal play_w = playWidth();

  auto& p = *painter;

  p.setRenderHint(QPainter::Antialiasing, false);

  QBrush c;
  if (isSelected())
  {
    c = skin.ConstraintSelected.getColor();
  }
  else if (parentItem()->isSelected())
  {
    c = skin.ConstraintFullViewParentSelected.getColor();
  }
  else
  {
    c = skin.ConstraintBase.getColor();
  }

  skin.ConstraintSolidPen.setBrush(c);

  if (min_w == max_w)
  {
    p.setPen(skin.ConstraintSolidPen);
    p.drawLine(0, 0, def_w, 0);
  }
  else
  {
    // First the line going from 0 to the min
    p.setPen(skin.ConstraintSolidPen);
    p.drawLine(0, 0, min_w, 0);

    // Then the dashed line
    skin.ConstraintDashPen.setBrush(c);
    p.setPen(skin.ConstraintDashPen);
    if(!infinite())
      p.drawLine(min_w, 0, max_w, 0);
    else
      p.drawLine(min_w, 0, def_w, 0);
  }

  auto pw = playWidth();
  if (pw != 0.)
  {
    skin.ConstraintPlayPen.setBrush(skin.ConstraintPlayFill.getColor());
    p.setPen(skin.ConstraintPlayPen);
    p.drawLine(0, 0, std::min(play_w, std::max(def_w, max_w)), 0);
  }

#if defined(ISCORE_SCENARIO_DEBUG_RECTS)
  p.setPen(Qt::red);
  p.drawRect(boundingRect());
#endif
}
}

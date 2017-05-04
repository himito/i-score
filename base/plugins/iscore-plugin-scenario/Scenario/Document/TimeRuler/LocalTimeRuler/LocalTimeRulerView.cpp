#include <Process/Style/ScenarioStyle.hpp>
#include <QColor>
#include <QString>

#include "LocalTimeRulerView.hpp"
#include <Scenario/Document/TimeRuler/AbstractTimeRulerView.hpp>
#include <iscore/document/DocumentContext.hpp>

namespace Scenario
{
LocalTimeRulerView::LocalTimeRulerView(QGraphicsView* v)
  : AbstractTimeRulerView(v)
{
  this->setCacheMode(QGraphicsItem::NoCache);
  m_graduationHeight = 10;
  m_textPosition = 1.75 * m_graduationHeight;
  m_height = 3 * m_graduationHeight;
  m_timeFormat = "ss''''z";
  setZValue(1);
}

LocalTimeRulerView::~LocalTimeRulerView() = default;

QRectF LocalTimeRulerView::boundingRect() const
{
  return QRectF{0, 0, m_width * 2, m_height};
}
}

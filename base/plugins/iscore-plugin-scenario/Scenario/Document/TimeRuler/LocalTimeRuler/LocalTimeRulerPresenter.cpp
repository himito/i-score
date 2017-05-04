#include "LocalTimeRulerPresenter.hpp"
#include <Process/TimeValue.hpp>
#include <Scenario/Document/TimeRuler/AbstractTimeRuler.hpp>
#include <Scenario/Document/TimeRuler/AbstractTimeRulerView.hpp>
#include <Scenario/Document/TimeRuler/LocalTimeRuler/LocalTimeRulerView.hpp>

class QObject;
namespace Scenario
{
LocalTimeRulerPresenter::LocalTimeRulerPresenter(
    LocalTimeRulerView* view, QObject* parent)
    : AbstractTimeRuler{view, parent}
{
}

LocalTimeRulerPresenter::LocalTimeRulerPresenter(
    LocalTimeRulerView* view,
    TimeVal startDate,
    TimeVal duration,
    double pixPerMillis,
    QObject* parent)
    : LocalTimeRulerPresenter{view, parent}
{
  m_startPoint = startDate;
  m_pixelPerMillis = pixPerMillis;
}

LocalTimeRulerPresenter::~LocalTimeRulerPresenter() = default;

void LocalTimeRulerPresenter::setStartPoint(TimeVal dur)
{
  if (m_startPoint != dur)
  {
    m_startPoint = dur;
    m_view->setX((m_startPoint).msec() * m_pixelPerMillis + m_totalScroll);
  }
}
}

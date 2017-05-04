#pragma once

#include <Process/TimeValue.hpp>
#include <Scenario/Document/TimeRuler/AbstractTimeRuler.hpp>

class QObject;

// TODO : total refactor => unused now
namespace Scenario
{
class LocalTimeRulerView;
class LocalTimeRulerPresenter final : public AbstractTimeRuler
{
public:
  LocalTimeRulerPresenter(LocalTimeRulerView* view, QObject* parent);
  LocalTimeRulerPresenter(
      LocalTimeRulerView* view,
      TimeVal startDate,
      TimeVal duration,
      double pixPerMillis,
      QObject* parent);
  ~LocalTimeRulerPresenter();

  void setStartPoint(TimeVal dur) override;
};
}

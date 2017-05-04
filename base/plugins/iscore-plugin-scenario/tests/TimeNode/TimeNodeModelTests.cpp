#include <QtTest/QtTest>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

#include <Process/TimeValue.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <iscore/model/path/Path.hpp>

#include <chrono>

class TimeNodeModelTests : public QObject
{
  Q_OBJECT

public:
private slots:
  void AddEventTest()
  {
    TimeNodeModel model{Id<TimeNodeModel>(1),
                        TimeValue{std::chrono::milliseconds(1)}, 0.5, this};
  }
};

QTEST_MAIN(TimeNodeModelTests)
#include "TimeNodeModelTests.moc"

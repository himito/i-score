#include <QtTest/QtTest>

#include <Commands/Scenario/CreateEvent.hpp>

#include <Document/Event/EventModel.hpp>
#include <Document/Event/EventData.hpp>

#include <Process/ScenarioModel.hpp>

using namespace iscore;
using namespace Scenario::Command;


class CreateEventTest: public QObject
{
        Q_OBJECT
    public:

    private slots:

        void CreateTest()
        {
            ScenarioModel* scenar = new ScenarioModel(id_type<ProcessSharedModelInterface> {0}, qApp);
            EventData data {};
            // data.id = 0; unused here
            data.dDate.setMSecs(10);
            data.relativeY = 0.2;

            CreateEvent cmd(
            {
                {"ScenarioModel", {0}},
            }, data);

            cmd.redo();
            QCOMPARE((int) scenar->events().size(), 2);   // TODO 3 if endEvent
            QCOMPARE(scenar->event(cmd.m_cmd->m_createdEventId)->heightPercentage(), 0.2);

            cmd.undo();
            QCOMPARE((int) scenar->events().size(), 1);   // TODO 2 if endEvent

            cmd.redo();
            QCOMPARE((int) scenar->events().size(), 2);
            QCOMPARE(scenar->event(cmd.m_cmd->m_createdEventId)->heightPercentage(), 0.2);


            // Delete them else they stay in qApp !

            delete scenar;
        }
};

QTEST_MAIN(CreateEventTest)
#include "CreateEventTest.moc"


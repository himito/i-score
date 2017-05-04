#include <ossia/editor/dataspace/dataspace.hpp>
#include <QtTest/QtTest>
#include <State/Unit.hpp>
#include <core/application/MockApplication.hpp>
#include <iscore/serialization/VisitorCommon.hpp>

class UnitTests : public QObject
{
  Q_OBJECT
  iscore::testing::MockApplication m;

private slots:

  void test_deserialize()
  {

    ossia::unit_t u = ossia::rgba_u{};
    QVERIFY(u == u);

    auto v = unmarshall<ossia::unit_t>(marshall<DataStream>(u));

    QVERIFY(u == v);
  }
};

QTEST_APPLESS_MAIN(UnitTests)
#include "UnitTests.moc"

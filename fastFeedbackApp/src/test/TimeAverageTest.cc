#include "TimeAverageTest.h"
#include <epicsTime.h>
#include "TimeUtil.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::TimeAverageTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

        /** 10 microseconds in nanos */
        const long TEN_MICROS = 10000;

/**
 * @author L.Piccoli
 */
void FF::TimeAverageTest::testAverage() {
    TimeAverage avg(4, "Test");

    avg._start._time.secPastEpoch = 0;
    avg._start._time.nsec = 0;

    avg._elapsedTime = TEN_MICROS;
    avg.update();
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS, avg.getMax());
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS, avg.getMin());
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS, avg.getAverage());

    for (int i = 0; i < 100; ++i) {
        avg._elapsedTime = TEN_MICROS;
        avg.update();
        CPPUNIT_ASSERT_EQUAL(TEN_MICROS, avg.getMax());
        CPPUNIT_ASSERT_EQUAL(TEN_MICROS, avg.getMin());
        CPPUNIT_ASSERT_EQUAL(TEN_MICROS, avg.getAverage());
    }

    avg._elapsedTime = TEN_MICROS * 3;
    avg.update();
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS * 3, avg.getMax());
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS, avg.getMin());
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS * 6 / 4, avg.getAverage());

    avg._elapsedTime = TEN_MICROS / 2;
    avg.update();
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS * 3, avg.getMax());
    CPPUNIT_ASSERT_EQUAL(TEN_MICROS / 2, avg.getMin());
    CPPUNIT_ASSERT_EQUAL((TEN_MICROS * 3 + TEN_MICROS * 2 + TEN_MICROS / 2) / 4, avg.getAverage());

}

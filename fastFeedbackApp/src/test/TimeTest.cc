#include "TimeTest.h"
#include <epicsTime.h>
#include "TimeUtil.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::TimeTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

/**
 * @author L.Piccoli
 */
void FF::TimeTest::testDifference() {
    Time start;
    sleep(2);
    Time end;
    Time difference = end - start;
    CPPUNIT_ASSERT(difference._time.secPastEpoch < 5);
    CPPUNIT_ASSERT(difference._time.secPastEpoch > 1);

    start._time.secPastEpoch = 0;
    start._time.nsec = 100000;
    end._time.secPastEpoch = 0;
    epicsUInt32 nanodiff = 1e9;
    end._time.nsec = start._time.nsec + 1e9;

    difference = start - end;
    CPPUNIT_ASSERT_EQUAL(nanodiff, difference._time.nsec);

    long expectedMillisdiff = nanodiff * 1e-6;
    long millisdiff = difference.toMillis();
    CPPUNIT_ASSERT_EQUAL(expectedMillisdiff, millisdiff);
}

void FF::TimeTest::test1HzDifference() {
    Time start;
    Time end;

    start._time.secPastEpoch = 654469936;
    start._time.nsec = 663257751;

    end._time.secPastEpoch = 654469937;
    end._time.nsec = 663444985;

    Time difference;
    difference = end - start;

    long millis = difference.toMillis();
    CPPUNIT_ASSERT(millis > 990);
    CPPUNIT_ASSERT(millis < 1100);

    start._time.secPastEpoch = 654469939;
    start._time.nsec = 663814143;

    end._time.secPastEpoch = 654469939;
    end._time.nsec = 672158529;

    difference = end - start;

    millis = difference.toMillis();
    std::cout << "Diff: " << millis << std::endl;
    CPPUNIT_ASSERT(millis > 990);
    CPPUNIT_ASSERT(millis < 1100);
}
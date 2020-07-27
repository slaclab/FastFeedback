#include "LoopThreadTest.h"
#include "LoopThread.h"
#include "SineWave.h"
#include "Event.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::LoopThreadTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

#ifdef RTEMS
#define TEST_DIR "/boot/g/lcls/vol8/epics/iocTop/FFController/Development/test/"
#else
#define TEST_DIR "/afs/slac/g/lcls/epics/iocTop/FFController/Development/test/"
#endif

void FF::LoopThreadTest::setUp() {
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = i;
        p2._inclusionMask[i] = i + 10;
        p3._inclusionMask[i] = i + 50;
    }
    config = new LoopConfiguration("TESTSLOT");
}

void FF::LoopThreadTest::tearDown() {
    delete config;
}

/**
 * Test if the LoopThread is responding correctly to PATTERN_EVENTS. Configure
 * a LoopThread to process two patterns, and verify that it goes into
 * WAITING_MEASUREMENT state. Check if an unknown and an unexpected pattern is
 * received.
 *
 * @author L.Piccoli
 */
void FF::LoopThreadTest::testProcessPattern() {
    LoopThread loopThread("MyLoop", config);

    Event event;
    event._type = PATTERN_EVENT;
    event._pattern = p1;

    loopThread._state = LoopThread::WAITING_PATTERN;

    // At this point the LoopThread is in WAITING_PATTERN state, so sending
    // a PATTERN_EVENT should work
    CPPUNIT_ASSERT_EQUAL(0, loopThread.processEvent(event));

    // After receiving the PATTERN_EVENT, the state must be WAITING_MEASUREMENT
    CPPUNIT_ASSERT_EQUAL(LoopThread::WAITING_MEASUREMENT, loopThread._state);

    // Check that the pattern event counter was increased
    decltype(loopThread._patternCount) expectedValue = 1;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._patternCount);

    // Try processing a PATTERN_EVENT while in WAITING_MEASUREMENT state
    CPPUNIT_ASSERT_EQUAL(-1, loopThread.processEvent(event));

    // State must remain as WAITING_MEASUREMENT
    CPPUNIT_ASSERT_EQUAL(LoopThread::WAITING_MEASUREMENT, loopThread._state);

    // Check that the pattern event counter is still the same
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._patternCount);

    // The pattern is on the list of patterns, so it must have increased
    // the number of unexpected patterns
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedPatternCount);
    expectedValue = 0;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unknownPatternCount);

    // Try to process a pattern that is not know, while in WAITING_MEASUREMENT
    // state
    event._pattern = p3;
    CPPUNIT_ASSERT_EQUAL(-1, loopThread.processEvent(event));

    // Both counters for unexpected and unknown pattern should have increased
    expectedValue = 2;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedPatternCount);
    expectedValue = 1;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unknownPatternCount);

    // Force state to be WAITING_PATTERN and send unknow pattern again
    loopThread._state = LoopThread::WAITING_PATTERN;
    CPPUNIT_ASSERT_EQUAL(-1, loopThread.processEvent(event));

    // Only the unknown pattern counter should have increased
    expectedValue = 2;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedPatternCount);
    expectedValue = 2;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unknownPatternCount);

    // Finally send a good pattern again and check the numbers
    event._pattern = p2;
    CPPUNIT_ASSERT_EQUAL(0, loopThread.processEvent(event));

    // After receiving the PATTERN_EVENT, the state must be WAITING_MEASUREMENT
    CPPUNIT_ASSERT_EQUAL(LoopThread::WAITING_MEASUREMENT, loopThread._state);

    // Check that the pattern event counter was increased
    expectedValue = 2;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._patternCount);
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedPatternCount);
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unknownPatternCount);

}

/**
 * Test if MEASUREMENT_EVENTS are processed correctly. Those events are expected
 * only after a PATTERN_EVENT has been received. The patterns of both events
 * must match.
 * 
 * @author L.Piccoli
 */
void FF::LoopThreadTest::testProcessMeasurement() {
    LoopThread loopThread("MyLoop", config);

    Event event;
    event._type = PATTERN_EVENT;
    event._pattern = p1;

    // Send a PATTERN_EVENT with p1, so LoopThread is waiting for a
    // MEASUREMENT_EVENT with p1
    loopThread._state = LoopThread::WAITING_PATTERN;
    loopThread.processEvent(event);

    // Send a MEASUREMENT_EVENT for p1
    event._type = MEASUREMENT_EVENT;
    CPPUNIT_ASSERT_EQUAL(0, loopThread.processEvent(event));

    decltype(loopThread._measurementCount) expectedValue = 1;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._measurementCount);

    expectedValue = 0;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedMeasurementCount);
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedMeasurementPatternCount);
    CPPUNIT_ASSERT_EQUAL(LoopThread::WAITING_PATTERN, loopThread._state);

    // Send a MEASUREMENT_EVENT for p2 when LoopThread is waiting for p1.
    // First reset state to WAITING_MEASUREMENT
    loopThread._state = LoopThread::WAITING_MEASUREMENT;
    event._pattern = p2;
    CPPUNIT_ASSERT_EQUAL(-1, loopThread.processEvent(event));

    expectedValue = 1;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._measurementCount);
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedMeasurementPatternCount);

    expectedValue = 0;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedMeasurementCount);

    // The state should go back to WAITING_PATTERN after an unexpected 
    // measurement is received.
    CPPUNIT_ASSERT_EQUAL(LoopThread::WAITING_PATTERN, loopThread._state);
    CPPUNIT_ASSERT_EQUAL(-1, loopThread.processEvent(event));

    expectedValue = 1;
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._measurementCount);
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedMeasurementPatternCount);
    CPPUNIT_ASSERT_EQUAL(expectedValue, loopThread._unexpectedMeasurementCount);

}

/**
 * Test that Loops are being correctly created when the LoopThread is
 * instantiated. There is one loop per pattern, and the algorithm is
 * specified in the LoopConfiguration.
 *
 * @author L.Piccoli
 */
void FF::LoopThreadTest::testCreateLoops() {
    LoopThread loopThread("MyLoop", config);

    CPPUNIT_ASSERT_EQUAL(0, loopThread.createLoops());
}

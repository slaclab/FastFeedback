#include "CollectorThreadTest.h"
#include "CollectorThread.h"
#include "PatternManager.h"

#include <iostream>

#include <unistd.h>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::CollectorThreadTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::CollectorThreadTest::setUp() {
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = i;
        p2._inclusionMask[i] = i + 10;
        p3._inclusionMask[i] = i + 50;
    }

    int size = 100;
    m1 = new MeasurementDevice("XX00", "M1", size);
    m2 = new MeasurementDevice("XX00", "M2", size);
    m3 = new MeasurementDevice("XX00", "M3", size);
    m4 = new MeasurementDevice("XX00", "M4", size);

    m1->setPatternMask(p1);
    m2->setPatternMask(p2);
    m3->setPatternMask(p3);
    m4->setPatternMask(p1);

    nc1 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc2 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc3 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc4 = new NullChannel(CommunicationChannel::READ_ONLY, true);

    m1->setCommunicationChannel(nc1);
    m2->setCommunicationChannel(nc2);
    m3->setCommunicationChannel(nc3);
    m4->setCommunicationChannel(nc4);

    // Add devices to the MeasurementCollector singleton.
    MeasurementCollector::getInstance().add(m1);
    MeasurementCollector::getInstance().add(m2);
    MeasurementCollector::getInstance().add(m3);
    MeasurementCollector::getInstance().add(m4);
}

void FF::CollectorThreadTest::tearDown() {
    // First remove devices from MeasurementCollector singlenton
    MeasurementCollector::getInstance().remove(m1);
    MeasurementCollector::getInstance().remove(m2);
    MeasurementCollector::getInstance().remove(m3);
    MeasurementCollector::getInstance().remove(m4);
    /*
    delete m1;
    delete m2;
    delete m3;
    delete m4;

    delete nc1;
    delete nc2;
    delete nc3;
    delete nc4;
    */
}

/**
 * When a PATTERN_EVENT is received by the CollectorThread it must store the
 * pattern used later when updating the measurements.
 * 
 * @author L.Piccoli
 */
void FF::CollectorThreadTest::testProcessPattern() {
    Pattern p1;
    for (int i = 0; i < 6; ++i) {
        p1._value[i] = i;
    }
    CollectorThread collector;

    // Start collector thread
    collector.start();
    sleep(1);

    // Set up PATTERN_EVENT
    Event patternEvent(PATTERN_EVENT);
    patternEvent._pattern = p1;

    // Send PATTERN_EVENT
    collector.send(patternEvent);
    sleep(2);
    // wait for PATTERN_EVENT to be consumed
    while (collector.getPendingEvents() != 0);

    // Check if pattern stored with collector is the same one sent before
    if (!(patternEvent._pattern == collector._pattern)) {
        CPPUNIT_ASSERT_MESSAGE(false, "Patterns are different!");
        // NOTE: If this test fails, the CollectorThread exits without
        // calling epicsThreadExit, which does generate some complaints by EPICS
    }

    // Quit collector thread
    Event e(QUIT_EVENT);
    collector.send(e);
    sleep(3);
    while (collector.getPendingEvents() != 0);
    sleep(2);
    while (collector._done == false);
    sleep(2);
    collector.join();
}

/**
 * Check whether the CollectorThread updates all devices for a given pattern
 * and broadcasts a MEASUREMENT_EVENT to registered receivers.
 * 
 * @author L.Piccoli
 */
void FF::CollectorThreadTest::testProcessMeasurement() {
    CollectorThread collector;
    EventReceiver r1;
    EventReceiver r2;

    // Add event receivers (these represent the LoopThreads)
    collector.add(r1);
    collector.add(r2);

    PatternManager::getInstance().enable();

    int numMeasurements = 10;
    for (int i = 0; i < numMeasurements; ++i) {
        // Update devices for pattern p1
        Event measurementEvent(MEASUREMENT_EVENT);
        Pattern pattern(p1);
        measurementEvent._pattern = pattern;
	collector._state = CollectorThread::WAITING_MEASUREMENT;
        CPPUNIT_ASSERT_EQUAL(0, collector.processMeasurementEvent(measurementEvent));

        // Check if devices were updated for p1
        double value;
        double expectedValue = i;
        epicsTimeStamp timestamp;
        CPPUNIT_ASSERT_EQUAL(0, m1->get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
        CPPUNIT_ASSERT_EQUAL(-1, m2->get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(-1, m3->get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(0, m4->get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);

        // Update devices for pattern p2
        Pattern pattern2(p2);
        measurementEvent._pattern = pattern2;
	collector._state = CollectorThread::WAITING_MEASUREMENT;
        collector.processMeasurementEvent(measurementEvent);

        // Check if devices were updated for p2
        CPPUNIT_ASSERT_EQUAL(-1, m1->get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(0, m2->get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
        CPPUNIT_ASSERT_EQUAL(-1, m3->get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(-1, m4->get(value, timestamp));
    }

    // Check if event receivers did get notice about new measurements,
    // which add up to the number of loop iterations (numMeasurements)
    // times 2 (one for each pattern - p1 and p2)
    CPPUNIT_ASSERT_EQUAL(r1.getPendingEvents(), numMeasurements * 2);
    CPPUNIT_ASSERT_EQUAL(r2.getPendingEvents(), numMeasurements * 2);
}

#include "MeasurementCollectorTest.h"
#include "MeasurementCollector.h"
#include "Log.h"

#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::MeasurementCollectorTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::MeasurementCollectorTest::setUp() {
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = i;
        p2._inclusionMask[i] = i + 10;
        p3._inclusionMask[i] = i + 50;
    }
    p4 = p1;
    p4._exclusionMask[2] = 0x8000;

    int size = 100;
    m1 = new MeasurementDevice("XX00", "M1", size);
    m1Clone = new MeasurementDevice("XX00", "M1", size);
    m2 = new MeasurementDevice("XX00", "M2", size);
    m3 = new MeasurementDevice("XX00", "M3", size);
    m4 = new MeasurementDevice("XX00", "M4", size);
    m5 = new MeasurementDevice("XX00", "M5", size);

    m1->setPatternMask(p1);
    m1Clone->setPatternMask(p1);
    m2->setPatternMask(p2);
    m3->setPatternMask(p3);
    m4->setPatternMask(p1);
    m5->setPatternMask(p4); // Matches P1

    nc1 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc2 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc3 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc4 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc5 = new NullChannel(CommunicationChannel::READ_ONLY, true);

    m1->setCommunicationChannel(nc1);
    m2->setCommunicationChannel(nc2);
    m3->setCommunicationChannel(nc3);
    m4->setCommunicationChannel(nc4);
    m5->setCommunicationChannel(nc5);
}

void FF::MeasurementCollectorTest::tearDown() {
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
 * Check if the hasPatternMask() returns true if a pattern has already been
 * added and false if pattern has not yet been added.
 * 
 * @author L.Piccoli
 */
void FF::MeasurementCollectorTest::testHasPattern() {
    MeasurementCollector collector;

    CPPUNIT_ASSERT_EQUAL(false, collector.hasPatternMask(p1));
    CPPUNIT_ASSERT_EQUAL(false, collector.hasPatternMask(p2));
    CPPUNIT_ASSERT_EQUAL(false, collector.hasPatternMask(p3));

    // Add a patterns by hand
    CollectorMeasurementSet *set = 0;
    collector._measurements.insert(std::pair<PatternMask, CollectorMeasurementSet *>(p1, set));
    CPPUNIT_ASSERT_EQUAL(true, collector.hasPatternMask(p1));
    CPPUNIT_ASSERT_EQUAL(false, collector.hasPatternMask(p2));
    CPPUNIT_ASSERT_EQUAL(false, collector.hasPatternMask(p3));

    collector._measurements.insert(std::pair<PatternMask, CollectorMeasurementSet *>(p3, set));
    CPPUNIT_ASSERT_EQUAL(true, collector.hasPatternMask(p1));
    CPPUNIT_ASSERT_EQUAL(false, collector.hasPatternMask(p2));
    CPPUNIT_ASSERT_EQUAL(true, collector.hasPatternMask(p3));
}

/**
 * Test adding devices to the collector. Devices are added to the map of
 * measurements based on their pattern:
 *
 *   m1 -> p1
 *   m2 -> p2
 *   m3 -> p3
 *   m4 -> p1
 *
 * Also try to add a 'clone' of m1, which has the same name and pattern, but
 * it is a different MeasurementDevice instance. The use of the DeviceCompare
 * defined in the file Device.h prevents that insertion to happen.
 *
 * @author L.Piccoli
 */
void FF::MeasurementCollectorTest::testAddDevice() {
    MeasurementCollector collector;
    //    Log::getInstance().logToConsole();

    // Try adding same device twice
    CPPUNIT_ASSERT_EQUAL(0, collector.add(m1));
    CPPUNIT_ASSERT_EQUAL(0, collector.add(m1));

    // Adding device with same name/pattern should not be allowed
    CPPUNIT_ASSERT_EQUAL(0, collector.add(m1Clone));

    // The set returned should contain m1 only. Later m4 is
    // added to the same set (because both m1 and m4 use p1)
    CollectorMeasurementMap::iterator iterator;
    iterator = collector._measurements.find(p1);
    CollectorMeasurementSet *set = iterator->second;
    CPPUNIT_ASSERT_EQUAL(1, (int) set->size());

    // Add other devices
    CPPUNIT_ASSERT_EQUAL(0, collector.add(m2));
    CPPUNIT_ASSERT_EQUAL(0, collector.add(m3));
    CPPUNIT_ASSERT_EQUAL(0, collector.add(m4));

    CPPUNIT_ASSERT_EQUAL(2, (int) set->size());
}

/**
 * Test if MeasurementDevices are updated only when their pattern is
 * received.
 *
 * @author L.Piccoli
 */
void FF::MeasurementCollectorTest::testUpdate() {
    MeasurementCollector collector;
    //    Log::getInstance().logToConsole();
    // First add the devices
    collector.add(m1);
    collector.add(m2);
    collector.add(m3);
    collector.add(m4);
    collector.add(m5); // Matches P1

    // Call update several times
    int updateP1 = 64;
    int updateP2 = 32;
    int updateP3 = 98;

    for (int i = 0; i < updateP1; ++i) {
        collector.update(p1);
    }

    for (int i = 0; i < updateP2; ++i) {
        collector.update(p2);
    }

    for (int i = 0; i < updateP3; ++i) {
        collector.update(p3);
    }

    // Check if measurements from devices are what we expect (a counting value)
    // The call to get() returns the latest value read by the
    // measurement device
    double value = -1;
    epicsTimeStamp timestamp;

    CPPUNIT_ASSERT_EQUAL(0, m1->get(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(updateP1 - 1.0, value);

    CPPUNIT_ASSERT_EQUAL(0, m2->get(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(updateP2 - 1.0, value);

    CPPUNIT_ASSERT_EQUAL(0, m3->get(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(updateP3 - 1.0, value);

    CPPUNIT_ASSERT_EQUAL(0, m4->get(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(updateP1 - 1.0, value);

    CPPUNIT_ASSERT_EQUAL(0, m5->get(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(updateP1 - 1.0, value);
}

/**
 * Test if removing a device from the MeasurementCollector list does work.
 * This functionality in needed monstly for testing the system, where devices
 * will be added/removed many times.
 * 
 * @author L.Piccoli
 */
void FF::MeasurementCollectorTest::testRemoveDevice() {
    MeasurementCollector collector;

    // Add device for pattern p1
    collector.add(m1);

    CollectorMeasurementMap::iterator iterator;
    iterator = collector._measurements.find(p1);
    CollectorMeasurementSet *set = iterator->second;
    CPPUNIT_ASSERT_EQUAL(1, (int) set->size());

    // Add another device for same pattern p1
    collector.add(m4);
    CPPUNIT_ASSERT_EQUAL(2, (int) set->size());

    collector.remove(m1);
    CPPUNIT_ASSERT_EQUAL(1, (int) set->size());

    collector.remove(m4);
    CPPUNIT_ASSERT_EQUAL(0, (int) set->size());
}

/**
 * Test that all measurements can be removed using the clear() method.
 * 
 * @author L.Piccoli
 */
void FF::MeasurementCollectorTest::testClear() {
    MeasurementCollector *collector = &MeasurementCollector::getInstance();

    collector->clear();
    CPPUNIT_ASSERT_EQUAL(0, (int) collector->_measurements.size());

    collector->add(m1);
    collector->add(m2);
    collector->add(m3);
    collector->add(m4);

    CPPUNIT_ASSERT_EQUAL(3, (int) collector->_measurements.size());
    collector->clear();
    CPPUNIT_ASSERT_EQUAL(0, (int) collector->_measurements.size());
}

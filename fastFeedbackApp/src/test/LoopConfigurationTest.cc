#include "LoopConfigurationTest.h"
#include "LoopConfiguration.h"
#include "MeasurementCollector.h"
#include "Log.h"

#include <string>
#include <fstream>
#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::LoopConfigurationTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

#ifdef RTEMS
#define TEST_DIR "/boot/g/lcls/vol8/epics/iocTop/FFController/Development/test/"
#else
#define TEST_DIR "/afs/slac/g/lcls/epics/iocTop/FFController/Development/test/"
#endif

void FF::LoopConfigurationTest::testInitializeDevices() {
    PatternMask fakePatternMask;
    fakePatternMask = PatternMask::ZERO_PATTERN_MASK;
    fakePatternMask._timeSlot = 100;

    MeasurementSet *measurementSet = new MeasurementSet;
    LoopConfiguration lc;
    lc.initializeDevices<MeasurementDevice, MeasurementSet >
            ("M", 12, "", 1, fakePatternMask, 1, measurementSet);

    std::vector<std::string> expectedDeviceNames;
    expectedDeviceNames.push_back("M1");
    expectedDeviceNames.push_back("M2");
    expectedDeviceNames.push_back("M3");
    expectedDeviceNames.push_back("M4");
    expectedDeviceNames.push_back("M5");
    expectedDeviceNames.push_back("M6");
    expectedDeviceNames.push_back("M7");
    expectedDeviceNames.push_back("M8");
    expectedDeviceNames.push_back("M9");
    expectedDeviceNames.push_back("M10");
    expectedDeviceNames.push_back("M11");
    expectedDeviceNames.push_back("M12");

    MeasurementSet::iterator it;
    int i = 0;
    for (it = measurementSet->begin(); it != measurementSet->end(); ++it, ++i) {
        CPPUNIT_ASSERT_EQUAL(expectedDeviceNames[i], (*it)->getName());
    }
}

void FF::LoopConfigurationTest::testReconfigureDevices() {
    LoopConfiguration lc;
    MeasurementSet set;

    PatternMask p1;
    p1 = PatternMask::ZERO_PATTERN_MASK;
    p1._timeSlot = 999;
    PatternMask p2(p1);
    p2._timeSlot = 333;

    CPPUNIT_ASSERT(p1 != p2);
    CPPUNIT_ASSERT(p2 < p1);

    // First test insert/remove from a map
    MeasurementMap map;
    MeasurementMap::iterator it;
    map.insert(std::pair<PatternMask, MeasurementSet *>(p1, &set));
    it = map.find(p1);
    CPPUNIT_ASSERT(it != map.end());
    map.erase(it);
    CPPUNIT_ASSERT_EQUAL(0, (int) map.size());
    std::pair<MeasurementMap::iterator, bool> ret;
    ret = map.insert(std::pair<PatternMask, MeasurementSet *>(p2, &set));
    CPPUNIT_ASSERT_EQUAL(true, ret.second);
    CPPUNIT_ASSERT_EQUAL(1, (int) map.count(ret.first->first));
    it = map.find(p2);
    CPPUNIT_ASSERT(it != map.end());
    CPPUNIT_ASSERT_EQUAL(1, (int) map.size());
    it = map.find(p1);
    CPPUNIT_ASSERT(it == map.end());

    // Now test the real thing
    lc._measurements.insert(std::pair<PatternMask, MeasurementSet *>(p1, &set));

    lc.initializeDevices<MeasurementDevice, MeasurementSet >
            ("M", 40, "", 1, p1, 1, &set);

    // Add more patterns
    PatternMask p1a(p1);
    p1a._timeSlot = 99;
    lc._measurements.insert(std::pair<PatternMask, MeasurementSet *>(p1a, &set));
    lc.initializeDevices<MeasurementDevice, MeasurementSet >
            ("A", 10, "", 1, p1a, 1, &set);

    PatternMask p1b(p1);
    p1b._timeSlot = 199;
    lc._measurements.insert(std::pair<PatternMask, MeasurementSet *>(p1b, &set));
    lc.initializeDevices<MeasurementDevice, MeasurementSet >
            ("S", 10, "", 1, p1b, 1, &set);

    PatternMask p1c(p1);
    p1c._timeSlot = 299;
    lc._measurements.insert(std::pair<PatternMask, MeasurementSet *>(p1c, &set));
    lc.initializeDevices<MeasurementDevice, MeasurementSet >
            ("S", 10, "DES", 1, p1c, 1, &set);

    for (it = lc._measurements.begin(); it != lc._measurements.end(); ++it) {
        PatternMask p = it->first;
        std::cout << &p << std::endl;
    }

    int status = lc.reconfigureDevices<MeasurementMap,
            MeasurementMap::iterator, MeasurementDevice, MeasurementSet,
            MeasurementSet::iterator > (lc._measurements, p1, p2);
    CPPUNIT_ASSERT_EQUAL(0, status);

    for (it = lc._measurements.begin(); it != lc._measurements.end(); ++it) {
        PatternMask p = it->first;
        std::cout << &p << std::endl;
    }
    CPPUNIT_ASSERT_EQUAL(4, (int) lc._measurements.size());

    // Old pattern p1 must have been removed from the map
    it = lc._measurements.find(p1);
    CPPUNIT_ASSERT(it == lc._measurements.end());

    // New pattern must be in there 
    it = lc._measurements.find(p2);
    CPPUNIT_ASSERT(it != lc._measurements.end());

    // Test with a newly created pattern
    PatternMask p3;
    p3 = PatternMask::ZERO_PATTERN_MASK;
    p3._timeSlot = 333;
    it = lc._measurements.find(p3);
    CPPUNIT_ASSERT(p2 == p3);
    CPPUNIT_ASSERT(p3 == p2);
    CPPUNIT_ASSERT(it != lc._measurements.end());

    p3 = p2;
    it = lc._measurements.find(p3);
    CPPUNIT_ASSERT(it != lc._measurements.end());

    PatternMask p4 = PatternMask::ZERO_PATTERN_MASK;
    p4._timeSlot = 333;
    CPPUNIT_ASSERT(p3 == p4);
    
    it = lc._measurements.find(p4);
    CPPUNIT_ASSERT(it != lc._measurements.end());

    MeasurementSet *s = map[p2];
    CPPUNIT_ASSERT_EQUAL(&set, s);

    s = map[p1];
    CPPUNIT_ASSERT(s == NULL);

    CPPUNIT_ASSERT_EQUAL(4, (int) lc._measurements  .size());
}

void FF::LoopConfigurationTest::testConfigurePatterns() {
    LoopConfiguration lc;

    CPPUNIT_ASSERT_EQUAL(0, lc.initialize());

    std::vector<unsigned short> v;
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(333);
    v.push_back(0);
    v.push_back(0);
    lc._poi1Pv.write(&v);

    PatternMask p1;
    p1 = PatternMask::ZERO_PATTERN_MASK;
    p1._timeSlot = 333;

    lc._totalPoiPv = 1;

    CPPUNIT_ASSERT_EQUAL(0, lc.configurePatterns());
    CPPUNIT_ASSERT_EQUAL(4, (int) lc._measurements.size());
    MeasurementMap::iterator it = lc._measurements.find(p1);
    CPPUNIT_ASSERT(it != lc._measurements.end());
}

void FF::LoopConfigurationTest::testConfigureFMatrix() {
    LoopConfiguration lc;
    lc._logger.logToConsole();

    CPPUNIT_ASSERT_EQUAL(0, lc.initialize());

    int rows = 4;
    int cols = 4;

    lc._measurementsUsedPv = rows;
    lc._statesUsedPv = cols;

    // Need to build a 4x4 f matrix is the _fMatrixPv attribute
    // ... actually we need an extra column with for the energies.
    // ... the extra column in not used for configuring the F Matrix
    std::vector<double> testFMatrix;
    int index = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols+1; ++j) {
	  if (j < cols) {
            testFMatrix.push_back(index);
            ++index;
	  }
	  else{
	    testFMatrix.push_back(0);
	  }
        }
        index = 100 * i;
    }

    lc._fMatrixPv.write(&testFMatrix);

    CPPUNIT_ASSERT_EQUAL(0, lc.configureFMatrix());

    // Check if the FMatrix got the correct values
    index = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols+1; ++j) {
	  if (j < cols) {
            CPPUNIT_ASSERT_EQUAL(testFMatrix[index], lc._fMatrix(i, j));
	  }
	  ++index;
        }
    }

}

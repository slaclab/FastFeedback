
#include "MeasurementDeviceTest.h"
#include "MeasurementDevice.h"
#include "NullChannel.h"
#include "FileChannel.h"
#include <string>
#include <fstream>
#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::MeasurementDeviceTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

/**
 * Make sure the MeasurementDevice class does not provide methods to write data.
 *
 * @author L.Piccoli
 */
void FF::MeasurementDeviceTest::testFailSetWrite() {
    MeasurementDevice m("XX00", "MyBPM", 1);
    CPPUNIT_ASSERT_EQUAL(-1, m.set(0.0));
    CPPUNIT_ASSERT_EQUAL(-1, m.write());
}

/**
 * Make sure MeasurementDevice works with a FileChannel, i.e data read()/get()
 * comes from a text file. This test writes the contents of the file and
 * make sure the MeasurementDevice reads them in the correct order.
 *
 * @author L.Piccoli
 */
void FF::MeasurementDeviceTest::testFileChannel() {
    std::string name = "MyBPM";
    std::string fileName = "/tmp/" + name + ".dat";

    // Write known values to input file first
    int numMeas = 234;
    std::ofstream testFile;
    testFile.open(fileName.c_str(), std::ifstream::trunc);
    for (int i = 0; i < numMeas; ++i) {
        testFile << i + 1000 << std::endl;
    }
    testFile.close();

    FileChannel *fr = new FileChannel(CommunicationChannel::READ_ONLY, fileName);
    double value;
    epicsTimeStamp timestamp;

    int size = 10;
    MeasurementDevice m("XX00", "MyBPM", size);
    m.setCommunicationChannel(fr);

    // Reads measurements in, checking values
    for (int i = 0; i < numMeas; ++i) {
        double expectedValue = i + 1000;
        CPPUNIT_ASSERT_EQUAL(0, m.read());
        CPPUNIT_ASSERT_EQUAL(0, m.get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }

    // Try reading one more from the file should fail because
    // EOF will be reached
    CPPUNIT_ASSERT_EQUAL(-1, m.read());
}

/**
 * Test whether measurement average is calculated correctly. Also try to change
 * the number of point used in the averaging.
 *
 * @author L.Piccoli
 */
void FF::MeasurementDeviceTest::testAverage() {
    int size = 1000;
    int iterations = 10;
    MeasurementDevice m("XX00", "MyBPM", size);

    NullChannel *nc = new NullChannel(CommunicationChannel::READ_ONLY, true);
    m.setCommunicationChannel(nc);

    // Fill up the internal buffer first
    for (int i = 0; i < size * iterations; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.read());
    }

    // Average the last 100 points 
    int averageSize = 100;
    m.setAverageCount(averageSize);
    double actualAverage = m.getAverage();

    double expectedAverage = 0;
    double value = size * iterations - 1;
    for (int i = 0; i < averageSize; ++i) {
      expectedAverage += value;
      value = value - 1;
    }
    expectedAverage /= averageSize;

    CPPUNIT_ASSERT_EQUAL(expectedAverage, actualAverage);

    // Average all points in the buffer (size elements)
    m.setAverageCount(size * 2);

    expectedAverage = 0;
    value = size * iterations - 1;
    // Even though there are 1000 elements in the buffer, only 999 are used.
    for (int i = 0; i < size - 1; ++i) {
      expectedAverage += value;
      value = value - 1;
    }
    expectedAverage /= (size - 1);

    CPPUNIT_ASSERT_EQUAL(expectedAverage, m.getAverage());

    // Try to average half-empty buffer - average count is still of the size
    // of the buffer
    m.clear();
    for (int i = 0; i < size / 2; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.read());
    }
    double last = (size * iterations) + size / 2 - 1;
    double first = (size * iterations);
    expectedAverage = (first + last) / 2;
    CPPUNIT_ASSERT_EQUAL(expectedAverage, m.getAverage());
}

/**
 * Check if latest read value can be retrieved using peek(). Peek() should be
 * used to get a measurement without moving the buffer cursor. Get() does move
 * the cursor to the next available measurement.
 *
 * @author L.Piccoli
 */
void FF::MeasurementDeviceTest::testPeek() {
    int size = 100;
    MeasurementDevice m("XX00", "MyBPM", size);

    NullChannel *nc = new NullChannel(CommunicationChannel::READ_ONLY, true);
    m.setCommunicationChannel(nc);

    // Fill up the internal buffer first, reading from NullChannel
    // and then retrieving with get() for the first 50 elements
    double value = 0;
    epicsTimeStamp timestamp;
    for (int i = 0; i < size; ++i) {
      CPPUNIT_ASSERT_EQUAL(0, m.read());
      if (i < size / 2) {
	CPPUNIT_ASSERT_EQUAL(0, m.get(value, timestamp));
      }
      value = i;
    }

    // Get half of the values out using get(), which moves the cursor
    /*
    for (int i = 0; i < size / 2; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.get(value, timestamp));
    }
    */
    // Peek a few times at the last measurement returned by get()
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());

    // Get another value and peek a few more
    CPPUNIT_ASSERT_EQUAL(0, m.read());
    CPPUNIT_ASSERT_EQUAL(0, m.get(value, timestamp));

    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
}


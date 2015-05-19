
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
 * Verify that if a MeasurementDevice is read times (i.e. values are retrieved
 * from the actual device) there will be measurement values dropped
 * once the internal buffer wraps. Some of the values will *not* be seen by
 * user when calling get() because they were overridden by multiple calls
 * to read().
 *
 * @author L.Piccoli
 */
void FF::MeasurementDeviceTest::testDropPoints() {
    int size = 10;
    MeasurementDevice m("XX00", "MyBPM", size);
    NullChannel nc(CommunicationChannel::READ_ONLY, true);
    m.setCommunicationChannel(&nc);

    CPPUNIT_ASSERT_EQUAL(0, m._droppedPoints);
    for (int i = 0; i < size; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.read());
    }
    CPPUNIT_ASSERT_EQUAL(0, m._droppedPoints);

    // Next buffer entry read using get() should have the value 1? 0?
    CPPUNIT_ASSERT_EQUAL(0, m._nextRead);
    CPPUNIT_ASSERT_EQUAL(0.0, m._buffer[m._nextRead]._value);

    // Now add one more so we have a dropped point
    CPPUNIT_ASSERT_EQUAL(0, m.read());
    CPPUNIT_ASSERT_EQUAL(1, m._droppedPoints);

    // The next value waiting to be read over should be 1,
    // because 0 was dropped
    CPPUNIT_ASSERT_EQUAL(1.0, m._buffer[m._nextRead]._value);
}

/**
 * Test when a user tries to get() more data from the internal buffer than it
 * was retrieved using read(). Once all read() values are get() then get()
 * should return errors.
 *
 * @author L.Piccoli
 */
void FF::MeasurementDeviceTest::testGetTooMuch() {
    int size = 10;
    MeasurementDevice m("XX00", "MyBPM", size);

    NullChannel nc(CommunicationChannel::READ_ONLY, true);
    m.setCommunicationChannel(&nc);

    // First read() values from the NullChannel (counting mode)
    for (int i = 0; i < size; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.read());
    }

    // Call get() for all values read() above
    double value;
    epicsTimeStamp timestamp;
    for (int i = 0; i < size; ++i) {
        double expectedValue = i;
        CPPUNIT_ASSERT_EQUAL(expectedValue, m._buffer[m._nextRead]._value);
        CPPUNIT_ASSERT_EQUAL(0, m.get(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }

    // Getting an extra value should fail!
    CPPUNIT_ASSERT_EQUAL(-1, m.get(value, timestamp));
}

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

    FileChannel fr(CommunicationChannel::READ_ONLY, fileName);
    double value;
    epicsTimeStamp timestamp;

    int size = 10;
    MeasurementDevice m("XX00", "MyBPM", size);
    m.setCommunicationChannel(&fr);

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

    NullChannel nc(CommunicationChannel::READ_ONLY, true);
    m.setCommunicationChannel(&nc);

    // Fill up the internal buffer first
    for (int i = 0; i < size * iterations; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.read());
    }

    // Average the last 100 points 
    int averageSize = 100;
    double last = (size * iterations) - 1;
    double first = (size * iterations) - averageSize;
    double expectedAverage = (first + last) / 2;

    m.setAverageCount(averageSize);
    CPPUNIT_ASSERT_EQUAL(expectedAverage, m.getAverage());

    // Average all points
    first = (size * iterations) - size;
    expectedAverage = (first + last) / 2;
    m.setAverageCount(size * 2);
    CPPUNIT_ASSERT_EQUAL(expectedAverage, m.getAverage());

    // Try to average half-empty buffer - average count is still of the size
    // of the buffer
    m.clear();
    for (int i = 0; i < size / 2; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.read());
    }
    last = (size * iterations) + size / 2 - 1;
    first = (size * iterations);
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

    NullChannel nc(CommunicationChannel::READ_ONLY, true);
    m.setCommunicationChannel(&nc);

    // Fill up the internal buffer first
    for (int i = 0; i < size; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.read());
    }

    // Get half of the values out using get(), which moves the cursor
    double value;
    epicsTimeStamp timestamp;
    for (int i = 0; i < size / 2; ++i) {
        CPPUNIT_ASSERT_EQUAL(0, m.get(value, timestamp));
    }

    // Peek a few times at the last measurement returned by get()
    value = value;
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());

    // Get another value and peek a few more
    CPPUNIT_ASSERT_EQUAL(0, m.get(value, timestamp));

    value = value;
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
    CPPUNIT_ASSERT_EQUAL(value, m.peek());
}


#include "ActuatorDeviceTest.h"
#include "ActuatorDevice.h"
#include "NullChannel.h"
#include "FileChannel.h"
#include <string>
#include <fstream>
#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::ActuatorDeviceTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

/**
 * Verify that if an Actuator is set multiple times there will be points dropped
 * once the internal buffer wraps. Some of the values will *not* be sent to
 * the actuator using write() because they were overridden by multiple calls
 * to set().
 *
 * @author L.Piccoli
 */
void FF::ActuatorDeviceTest::testDropPoints() {
    int size = 10;
    ActuatorDevice a("XX00", "MyMagnet", size);
    NullChannel *nc = new NullChannel(CommunicationChannel::WRITE_ONLY);
    a.setCommunicationChannel(nc);

    CPPUNIT_ASSERT_EQUAL(0, a._droppedPoints);
    for (int i = 0; i < size; ++i) {
        double value = i;
        CPPUNIT_ASSERT_EQUAL(0, a.set(value));
    }
    CPPUNIT_ASSERT_EQUAL(0, a._droppedPoints);

    // Next value to be written is 0
    CPPUNIT_ASSERT_EQUAL(0.0, a._buffer[a._nextWrite]._value);

    // Now add one more so we have a dropped point
    CPPUNIT_ASSERT_EQUAL(0, a.set(0.1));
    CPPUNIT_ASSERT_EQUAL(1, a._droppedPoints);

    // The next value waiting to be written should be 1,
    // because 0 was dropped
    CPPUNIT_ASSERT_EQUAL(1.0, a._buffer[a._nextWrite]._value);
}

/**
 * Test invoking write() more times than set(), which means the user is trying
 * to send data that was not previously added to the internal buffer using
 * set().
 *
 * REMOVED: this test has been removed because the write() function writes out
 * the latest set value, if there are queue set values they are skipped.
 * 
 * @author L.Piccoli
 */
void FF::ActuatorDeviceTest::testWriteTooMuch() {
  /*
    int size = 10;
    ActuatorDevice a("XX00", "MyMagnet", size);

    NullChannel *nc = new NullChannel(CommunicationChannel::WRITE_ONLY);
    a.setCommunicationChannel(nc);

    a._hihiPv = 1000;
    a._loloPv = -1000;

    for (int i = 0; i < size; ++i) {
        double value = i;
        CPPUNIT_ASSERT_EQUAL(0, a.set(value));
    }

    for (int i = 0; i < size; ++i) {
        double value = i;

        CPPUNIT_ASSERT_EQUAL(value, a._buffer[a._nextWrite]._value);
	int ret = a.write();
        CPPUNIT_ASSERT_EQUAL(0, ret);
    }
    CPPUNIT_ASSERT_EQUAL(-1, a.write());
  */
}

/**
 * Make sure the ActuatorDevice class does not provide methods to get data.
 *
 * @author L.Piccoli
 */
void FF::ActuatorDeviceTest::testFailGetRead() {
    ActuatorDevice a("XX00", "MyMagnet", 1);
    double value;
    epicsTimeStamp timestamp;
    CPPUNIT_ASSERT_EQUAL(-1, a.get(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(-1, a.read());
}

/**
 * Make sure ActuatorDevice works with a FileChannel, i.e data set()/write()
 * will end up in a text file. This test reads back contents of the file to
 * make sure values were set in the correct order.
 *
 * @author L.Piccoli
 */
void FF::ActuatorDeviceTest::testFileChannel() {
    std::string name = "MyMagnet";
    std::string fileName = "/tmp/" + name + ".dat";
    int size = 10;
    ActuatorDevice a("XX00", name, size);

    FileChannel *fc = new FileChannel(CommunicationChannel::WRITE_ONLY, fileName);
    a.setCommunicationChannel(fc);

    for (int i = 0; i < size * 4; ++i) {
        double value = i + 100;
        CPPUNIT_ASSERT_EQUAL(0, a.set(value));
        CPPUNIT_ASSERT_EQUAL(0, a.write());
    }

    // Check if file did get the correct values written
    std::ifstream testFile;
    testFile.open(fileName.c_str(), std::ifstream::in);
    for (int i = 0; i < size * 4; ++i) {
        double expectedValue = i + 100;
        double value;
        testFile >> value;
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }
    testFile.close();
}

/**
 * Checking if peek() does return the last value written.
 *
 * @author L.Piccoli
 */
void FF::ActuatorDeviceTest::testPeek() {
    int size = 10;
    ActuatorDevice a("XX00", "MyMagnet", size);

    NullChannel *nc = new NullChannel(CommunicationChannel::WRITE_ONLY);
    a.setCommunicationChannel(nc);

    for (int i = 0; i < size; ++i) {
        double value = i;
        a.set(value);
        a.write();
        double expectedValue = i;
        CPPUNIT_ASSERT_EQUAL(expectedValue, a.peek());
    }
}

/**
 * Test the comparison between two ActuatorDevices.
 *
 * @author L.Piccoli
 */
void FF::ActuatorDeviceTest::testActuatorCompare() {
    std::string name = "Lula";
    ActuatorDevice device1("XX00", name, 10);
    ActuatorDevice device2("XX00", name, 20);

    PatternMask p1;
    PatternMask p2;
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = 12 + i;
        p2._inclusionMask[i] = p1._inclusionMask[i];
    }
    p2._inclusionMask[3]++; // Make pattern a bit different
    device1.setPatternMask(p1);
    device2.setPatternMask(p1);

    std::set<ActuatorDevice *, DeviceCompare> deviceSet;

    deviceSet.insert(&device1);
    CPPUNIT_ASSERT_EQUAL(1, (int) deviceSet.size());

    // Inserting the device with same name and pattern should fail
    deviceSet.insert(&device2);
    CPPUNIT_ASSERT_EQUAL(1, (int) deviceSet.size());

    // Now, change device2's pattern to p2 and the insert should work
    device2.setPatternMask(p2);
    deviceSet.insert(&device2);
    CPPUNIT_ASSERT_EQUAL(2, (int) deviceSet.size());
}

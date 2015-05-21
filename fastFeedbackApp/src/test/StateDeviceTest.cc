#include "StateDeviceTest.h"
#include "StateDevice.h"
#include "NullChannel.h"
#include "FileChannel.h"
#include "SetpointDevice.h"
#include <string>
#include <fstream>
#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::StateDeviceTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::StateDeviceTest::setUp() {
    setpoint = new SetpointDevice("XX00", "MySetPoint", 10);
}

void FF::StateDeviceTest::tearDown() {
    delete setpoint;
}

/**
 * Make sure when set() is called the error (difference between state value
 * and setpoint value) is calculated correctly.
 * 
 * @author L.Piccoli
 */
void FF::StateDeviceTest::testError() {
    StateDevice state("XX00", "MyState", 10);
    NullChannel stateNc(CommunicationChannel::WRITE_ONLY);
    state.setCommunicationChannel(&stateNc);

    // Set up the setpoint
    NullChannel setpointNc(CommunicationChannel::READ_ONLY, true);
    setpoint->setCommunicationChannel(&setpointNc);
    state.setSetpointDevice(setpoint);

    // Read setpoint a few times
    double setpointValue;
    epicsTimeStamp timestamp;
    setpoint->read(); setpoint->get(setpointValue, timestamp); // 0
    setpoint->read(); setpoint->get(setpointValue, timestamp); // 1
    setpoint->read(); setpoint->get(setpointValue, timestamp); // 2
    setpoint->read(); setpoint->get(setpointValue, timestamp); // 3
    setpoint->read(); setpoint->get(setpointValue, timestamp); // 4

    setpointValue = setpoint->peek();
    CPPUNIT_ASSERT_EQUAL(4.0, setpointValue);

    // Set state to the same setpoint value
    CPPUNIT_ASSERT_EQUAL(0, state.set(setpointValue));
    CPPUNIT_ASSERT_EQUAL(0.0, state.getError());

    // Set a value different than the setpoint value
    double stateValue = setpointValue + 1;
    CPPUNIT_ASSERT_EQUAL(0, state.set(stateValue));
    CPPUNIT_ASSERT_EQUAL(stateValue - setpointValue, state.getError());

    // Set state to value less than setpoint value
    stateValue = setpointValue - 1;
    CPPUNIT_ASSERT_EQUAL(0, state.set(stateValue));
    CPPUNIT_ASSERT_EQUAL(stateValue - setpointValue, state.getError());
}

/**
 * Testing whether the value written out to a state does include the 
 * offset. Values are written to a file (using the FileChannel), read
 * back and verified.
 *
 * @author L.Piccoli
 */
void FF::StateDeviceTest::testWrite() {
    std::string name = "MyState";
    std::string fileName = "/tmp/" + name + ".dat";
    int size = 10;
    StateDevice state("XX00", name, size);

    FileChannel fc(CommunicationChannel::WRITE_ONLY, fileName);
    state.setCommunicationChannel(&fc);

    // Set up the setpoint
    NullChannel setpointNc(CommunicationChannel::READ_ONLY, true);
    setpoint->setCommunicationChannel(&setpointNc);
    state.setSetpointDevice(setpoint);
    double value;
    epicsTimeStamp timestamp;
    setpoint->read();
    setpoint->get(value, timestamp);

    // Set the offset
    double offset = 0.01;
    state.setOffset(offset);

    for (int i = 0; i < size * 4; ++i) {
        double value = i + 100;
        CPPUNIT_ASSERT_EQUAL(0, state.set(value));
        CPPUNIT_ASSERT_EQUAL(0, state.write());
    }

    // Check if file did get the correct values written
    std::ifstream testFile;
    testFile.open(fileName.c_str(), std::ifstream::in);
    for (int i = 0; i < size * 4; ++i) {
        double expectedValue = i + 100 + state.getOffset();
        double value;
        testFile >> value;
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }
    testFile.close();
}

#include "DeviceViewTest.h"
#include "DeviceView.h"
#include "Log.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::DeviceViewTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::DeviceViewTest::setUp() {
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = i;
        p2._inclusionMask[i] = i + 10;
        p3._inclusionMask[i] = i + 50;
    }

    size = 100;

    a1 = new ActuatorDevice("XX00", "A1", size);
    a2 = new ActuatorDevice("XX00", "A2", size);
    s1 = new StateDevice("XX00", "S1", size);
    s2 = new StateDevice("XX00", "S2", size);

    m1 = new MeasurementDevice("XX00", "M1", size);
    m2 = new MeasurementDevice("XX00", "M2", size);
    sp1 = new MeasurementDevice("XX00", "SP1", size);
    sp2 = new MeasurementDevice("XX00", "SP2", size);

    fc1 = new FileChannel(CommunicationChannel::WRITE_ONLY, "/tmp/Act1.dat");
    fc2 = new FileChannel(CommunicationChannel::WRITE_ONLY, "/tmp/Act2.dat");
    fc3 = new FileChannel(CommunicationChannel::WRITE_ONLY, "/tmp/Sta1.dat");
    fc4 = new FileChannel(CommunicationChannel::WRITE_ONLY, "/tmp/Sta2.dat");

    a1->setCommunicationChannel(fc1);
    a2->setCommunicationChannel(fc2);
    s1->setCommunicationChannel(fc3);
    s2->setCommunicationChannel(fc4);

    nc1 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc2 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc3 = new NullChannel(CommunicationChannel::READ_ONLY, true);
    nc4 = new NullChannel(CommunicationChannel::READ_ONLY, true);

    m1->setCommunicationChannel(nc1);
    m2->setCommunicationChannel(nc2);
    sp1->setCommunicationChannel(nc3);
    sp2->setCommunicationChannel(nc4);

    m1->setPatternMask(p1);
    m2->setPatternMask(p2);
    a1->setPatternMask(p1);
    a2->setPatternMask(p2);
    s1->setPatternMask(p1);
    s2->setPatternMask(p2);
}

void FF::DeviceViewTest::tearDown() {
}

/**
 * Test if adding devices to a view works. The add operation does not
 * verify if a device is unique or if there is only one device per pattern.
 *
 * @author L.Piccoli
 */
void FF::DeviceViewTest::testAddDevice() {
    int index = 5;
    char type = 'M';
    DeviceView view(size, index, type);

    CPPUNIT_ASSERT_EQUAL(0, view.add(m1));
    CPPUNIT_ASSERT_EQUAL(1, (int) view._devices.size());

    CPPUNIT_ASSERT_EQUAL(0, view.add(m2));
    CPPUNIT_ASSERT_EQUAL(2, (int) view._devices.size());

    CPPUNIT_ASSERT_EQUAL(0, view.add(sp1));
    CPPUNIT_ASSERT_EQUAL(3, (int) view._devices.size());

    CPPUNIT_ASSERT_EQUAL(0, view.add(sp2));
    CPPUNIT_ASSERT_EQUAL(4, (int) view._devices.size());
}

/**
 * Test if the internal DeviceView buffer gets the proper updated values
 * from MeasurementDevices.
 *
 * First add one MeasurementDevice, read it and check view buffer.
 *
 * Then add another MeasurementDevice, with different pattern and repeat test.
 *
 * Lastly, read both MeasurementDevices such that values in the view buffer
 * alternate between the MeasurementDevices.
 *
 * @author L.Piccoli
 */
void FF::DeviceViewTest::testUpdateMeasurements() {
  Log::getInstance().logToConsole();
  
    int index = 5;
    char type = 'M';
    DeviceView view(size, index, type);

    int readings = 15;
    double expectedBuffer[size];

    // These are the values expected from updating the view
    for (int i = 0; i < readings; ++i) {
        expectedBuffer[i] = i;
    }

    // Add MeasurementDevice to View
    view.add(m1);

    // Add one MeasurementDevice, the normal sequence of events is
    // 1) read device -> get data from communication channel into MeasDevice buffer
    // 2) get device data -> get data from MeasurementDevice buffer
    // 3) update view
    for (int i = 0; i < readings; ++i) {
        // Read device
        m1->read();

        // Get data
        double value;
        epicsTimeStamp timestamp;
        m1->get(value, timestamp);

        // Update view
        CPPUNIT_ASSERT_EQUAL(0, view.update(m1->getPatternMask()));
    }

    // Check if view does have expected values
    for (int i = 0; i < readings; ++i) {
        CPPUNIT_ASSERT_EQUAL(expectedBuffer[i], view._buffer[i]);
    }

    // Add another MeasurementDevice with a different pattern (p2)
    view.add(m2);

    // Now try a more realistic scenario, updating m1 and then m2 on every
    // iteration
    for (int i = 0; i < readings; ++i) {
        // Read device
        m1->read();

        // Get data
        double value;
        epicsTimeStamp timestamp;
        m1->get(value, timestamp);

        // Update view
        CPPUNIT_ASSERT_EQUAL(0, view.update(m1->getPatternMask()));

        // Read device
        m2->read();

        // Get data
        m2->get(value, timestamp);

        // Update view
        CPPUNIT_ASSERT_EQUAL(0, view.update(m2->getPatternMask()));
    }

    // Check if view does have expected values, i.e. values read from m1 are
    // the same as the values read from m2
    for (int i = readings * 2; i < readings * 2; ++i, ++i) {
        CPPUNIT_ASSERT_EQUAL(view._buffer[i], view._buffer[i + 1]);
    }
}

/**
 * Test if the internal DeviceView buffer gets the proper updated values
 * from ActuatorDevices.
 *
 * The DeviceView buffer must contain values that matches the ones saved in
 * the FileChannel for the ActuatorDevices.
 *
 * @author L.Piccoli
 */
void FF::DeviceViewTest::testUpdateActuators() {
    int index = 5;
    char type = 'A';
    DeviceView view(size, index, type);

    // Add both actuators at once
    view.add(a1);
    view.add(a2);

    int startA1 = 100;
    int startA2 = 200;
    int writings = 33;

    // Write to actuators and update view
    for (int i = 0; i < writings; ++i) {
        double a1value = i + startA1;
        double a2value = i + startA2;
        a1->set(a1value);
        a2->set(a2value);

        a1->write();
        a2->write();

        CPPUNIT_ASSERT_EQUAL(0, view.update(a1->getPatternMask()));
        CPPUNIT_ASSERT_EQUAL(0, view.update(a2->getPatternMask()));
	CPPUNIT_ASSERT_EQUAL(0, view.update(p3));
    }

    // At this point _next should be pointing to writings * 2 position
    CPPUNIT_ASSERT_EQUAL(writings * 2, view._next);

    // Contents of the Actuator output files must be the same as the
    // view buffer
    std::ifstream a1file;
    std::ifstream a2file;
    a1file.open(fc1->getFileName().c_str(), std::ifstream::in);
    a2file.open(fc2->getFileName().c_str(), std::ifstream::in);
    for (int i = 0; i < writings; ++i) {
        double a1ExpectedValue = i + startA1;
        double a2ExpectedValue = i + startA2;

        double a1value;
        double a2value;

        a1file >> a1value;
        a2file >> a2value;

        CPPUNIT_ASSERT_EQUAL(a1ExpectedValue, a1value);
        CPPUNIT_ASSERT_EQUAL(a2ExpectedValue, a2value);

        CPPUNIT_ASSERT_EQUAL(a1value, view._buffer[i * 2]);
        CPPUNIT_ASSERT_EQUAL(a2value, view._buffer[i * 2 + 1]);
    }
    a1file.close();
    a2file.close();
}

/**
 * Test if slow history buffer is being updated with the latest value from
 * the regular history buffer.
 *
 * @author L.Piccoli
 */
void FF::DeviceViewTest::testUpdateSlow() {
    int index = 5;
    char type = 'M';
    DeviceView view(size, index, type);

    view.add(m1);
    for (int i = 0; i < 14; ++i) {
        // Read device
        m1->read();

        // Get data
        double value;
        epicsTimeStamp timestamp;
        m1->get(value, timestamp);

        // Update view
        view.update(m1->getPatternMask());
    }
    view.updateSlow();

    // Check if last element in buffer is the same on in the slow buffer
    CPPUNIT_ASSERT_EQUAL(view._buffer[(view._bufferSize + view._next - 1) % view._bufferSize],
            view._slowBuffer[(view._bufferSize + view._slowNext - 1) % view._bufferSize]);

    // Last element in the slow buffer must be the last value read in
    CPPUNIT_ASSERT_EQUAL(m1->peek(),
            view._slowBuffer[(view._bufferSize + view._slowNext - 1) % view._bufferSize]);
}

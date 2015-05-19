#include "SineWaveTest.h"
#include "SineWave.h"
#include "LoopConfiguration.h"
#include "NullChannel.h"
#include <math.h>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::SineWaveTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

/**
 * Make sure the sine wave is written to all actuators and state devices.
 * 
 * @author L.Piccoli
 */
void FF::SineWaveTest::testCalculate() {
    int size = 10;

    std::string name = "A1";
    ActuatorDevice a1("XX00", name, size);
    NullChannel nc1(CommunicationChannel::WRITE_ONLY);
    a1.setCommunicationChannel(&nc1);

    name = "A2";
    ActuatorDevice a2("XX00", name, size);
    NullChannel nc2(CommunicationChannel::WRITE_ONLY);
    a2.setCommunicationChannel(&nc2);

    name = "S1";
    StateDevice s1("XX00", name, size);
    NullChannel nc3(CommunicationChannel::WRITE_ONLY);
    s1.setCommunicationChannel(&nc3);

    // Since the calculate for SineWave does not use the LoopConfiguration
    // and the Measurements, just create empty ones.
    LoopConfiguration configuration;
    MeasurementSet measurements;
    ActuatorSet actuators;
    StateSet states;

    actuators.insert(&a1);
    actuators.insert(&a2);
    states.insert(&s1);

    SineWave sineWave;

    for (int i = 0; i < 100; ++i) {
        double expectedValue = sin(sineWave._counter);
        sineWave.calculate(configuration, measurements, actuators, states);

        // After values are calculated, they must be written out
        a1.write();
        a2.write();
        s1.write();

        CPPUNIT_ASSERT_EQUAL(expectedValue, a1.peek());
        CPPUNIT_ASSERT_EQUAL(expectedValue, a2.peek());
        CPPUNIT_ASSERT_EQUAL(expectedValue, s1.peek());
    }
}

/**
 * Make sure the sine wave is calculated from value read from the measurement
 * device.
 *
 * @author L.Piccoli
 */
void FF::SineWaveTest::testCalculateMode1() {
    int size = 10;

    std::string name = "A1";
    ActuatorDevice a1("XX00", name, size);
    NullChannel nc1(CommunicationChannel::WRITE_ONLY);
    a1.setCommunicationChannel(&nc1);

    name = "S1";
    StateDevice s1("XX00", name, size);
    NullChannel nc2(CommunicationChannel::WRITE_ONLY);
    s1.setCommunicationChannel(&nc2);

    name = "M1";
    MeasurementDevice m1("XX00", name, size);
    NullChannel nc3(CommunicationChannel::READ_ONLY, true);
    m1.setCommunicationChannel(&nc3);

    // Since the calculate for SineWave does not use the LoopConfiguration
    // and the Measurements, just create empty ones.
    LoopConfiguration configuration;
    MeasurementSet measurements;
    ActuatorSet actuators;
    StateSet states;

    actuators.insert(&a1);
    states.insert(&s1);
    measurements.insert(&m1);

    SineWave sineWave(SineWave::ONE_MEAS_ONE_ACT);

    int counter = 0;
    for (int i = 0; i < 100; ++i) {
        // First need to read in the measurement
        m1.read();
        
        double expectedValue = sin(counter);
        CPPUNIT_ASSERT_EQUAL(0, sineWave.calculate(configuration, measurements,
                actuators, states));

        // After values are calculated, they must be written out
        a1.write();
        s1.write();

        CPPUNIT_ASSERT_EQUAL(expectedValue, a1.peek());
        CPPUNIT_ASSERT_EQUAL(expectedValue, s1.peek());
        counter++;
    }
}

/**
 * Make sure the sine wave is calculated from value read from two measurement
 * device.
 *
 * @author L.Piccoli
 */
void FF::SineWaveTest::testCalculateMode2() {
    int size = 10;

    std::string name = "A1";
    ActuatorDevice a1("XX00", name, size);
    NullChannel nc1(CommunicationChannel::WRITE_ONLY);
    a1.setCommunicationChannel(&nc1);

    name = "S1";
    StateDevice s1("XX00", name, size);
    NullChannel nc2(CommunicationChannel::WRITE_ONLY);
    s1.setCommunicationChannel(&nc2);

    name = "M1";
    MeasurementDevice m1("XX00", name, size);
    NullChannel nc3(CommunicationChannel::READ_ONLY, true);
    m1.setCommunicationChannel(&nc3);

    name = "M2";
    MeasurementDevice m2("XX00", name, size);
    NullChannel nc4(CommunicationChannel::READ_ONLY, true);
    m2.setCommunicationChannel(&nc4);

    // Since the calculate for SineWave does not use the LoopConfiguration
    // and the Measurements, just create empty ones.
    LoopConfiguration configuration;
    MeasurementSet measurements;
    ActuatorSet actuators;
    StateSet states;

    actuators.insert(&a1);
    states.insert(&s1);
    measurements.insert(&m1);
    measurements.insert(&m2);

    SineWave sineWave(SineWave::TWO_MEAS_ONE_ACT);

    int counter = 0;
    for (int i = 0; i < 100; ++i) {
        // First need to read in the measurements
        m1.read();
        m2.read();

        double expectedValue = sin(counter + counter);
        CPPUNIT_ASSERT_EQUAL(0, sineWave.calculate(configuration, measurements,
                actuators, states));

        // After values are calculated, they must be written out
        a1.write();
        s1.write();

        CPPUNIT_ASSERT_EQUAL(expectedValue, a1.peek());
        CPPUNIT_ASSERT_EQUAL(expectedValue, s1.peek());
        counter++;
    }
}


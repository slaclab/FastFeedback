#include "LoopTest.h"
#include "Loop.h"
#include "SineWave.h"
#include <math.h>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::LoopTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

#ifdef RTEMS
#define TEST_DIR "/boot/g/lcls/vol8/epics/iocTop/FFController/Development/test/"
#else
#define TEST_DIR "/afs/slac/g/lcls/epics/iocTop/FFController/Development/test/"
#endif

/**
 * Make sure the Loop gets the correct set of measurements, actuators and states
 * from the LoopConfiguration.
 *
 * @author L.Piccoli
 */
void FF::LoopTest::testConstructor() {
    /*
    LoopConfiguration config;
    SineWave sine;
    Pattern pattern;

    std::ifstream file;
    std::string fileName = TEST_DIR;
    fileName += "LoopConfig.dat";

    CPPUNIT_ASSERT_EQUAL(0, config.configure(fileName));

    pattern = config._patterns[1];
    Loop loop(&config, pattern, &sine);

    // Make sure the loop picked up the correct set of
    // Measurements, Actuators and States.
    // This code is just checking if the devices have the correct pattern.
    {
        MeasurementSet::iterator iterator;
        for (iterator = loop._measurements.begin();
                iterator != loop._measurements.end(); ++iterator) {
            Pattern devicePattern = (*iterator)->getPattern();
            CPPUNIT_ASSERT(pattern == devicePattern);
        }
    }

    {
        ActuatorSet::iterator iterator;
        for (iterator = loop._actuators.begin();
                iterator != loop._actuators.end(); ++iterator) {
            Pattern devicePattern = (*iterator)->getPattern();
            CPPUNIT_ASSERT(pattern == devicePattern);
        }
    }

    {
        StateSet::iterator iterator;
        for (iterator = loop._states.begin();
                iterator != loop._states.end(); ++iterator) {
            Pattern devicePattern = (*iterator)->getPattern();
            CPPUNIT_ASSERT(pattern == devicePattern);
        }
    }
     * */
}

/**
 * Test the Loop::calculate method using the SineWave algorithm. SineWave
 * updates the actuators and states to the sine value of a counter starting
 * at 0. All actuators and states will be set to the same sine value.
 *
 * @author L.Piccoli
 */
void FF::LoopTest::testCalculateSine() {
    /*
    LoopConfiguration config;
    SineWave sine;
    Pattern pattern;

    std::ifstream file;
    std::string fileName = TEST_DIR;
    fileName += "LoopConfig.dat";

    config.configure(fileName);

    pattern = config._patterns[0];
    Loop loop(&config, pattern, &sine);

    // Calculate new actuator and states a few times
    for (int i = 0; i < 100; ++i) {
        // Call SineWave algorithm that updates actuators and states
        // Updated values are written out by the loop
        loop.calculate(pattern);

        // Now make sure the values written out are the expected sin(i)
        double expectedValue = sin(i);

        {
            ActuatorSet::iterator iterator;
            for (iterator = loop._actuators.begin();
                    iterator != loop._actuators.end(); ++iterator) {
                double value = (*iterator)->peek();
                CPPUNIT_ASSERT_EQUAL(expectedValue, value);
            }
        }

        {
            StateSet::iterator iterator;
            for (iterator = loop._states.begin();
                    iterator != loop._states.end(); ++iterator) {
                double value = (*iterator)->peek();
                CPPUNIT_ASSERT_EQUAL(expectedValue, value);
            }
        }
    }
     */
}


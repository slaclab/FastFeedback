#include "ExecThreadTest.h"
#include "ExecThread.h"
#include "CollectorThread.h"
#include "ExecConfiguration.h"
#include "PatternManager.h"
#include "PatternGenerator.h"
#include <iomanip>
#include <math.h>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::ExecThreadTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

#define TEST_DIR "/afs/slac/g/lcls/epics/iocTop/FFController/Development/test/"
#define SIMPLE_TEST_DIR "/afs/slac/g/lcls/epics/iocTop/FFController/Development/test/simple/"

/**
 * Check if the ExecThread creates all LoopThreads and initializes all elements
 * in the system using a test configuration file.
 *
 * @author L.Piccoli
 */
void FF::ExecThreadTest::testConfigure() {
  // REMOVED
}

/**
 * Run a simple system configuration test, with one feedback loop, one
 * measurement, one actuator and one pattern.
 *
 * @author L.Piccoli
 */
void FF::ExecThreadTest::testRun() {
    ExecThread et;
    std::ifstream file;
    std::string fileName = SIMPLE_TEST_DIR;
    fileName += "ExecConfigSimple.dat";

    // Drops previous configuration - memory leak here
    ExecConfiguration::getInstance().clear();

    // Configure again with simple test environment
    CPPUNIT_ASSERT_EQUAL(0, et.configure(fileName));

    // Start the system
    CPPUNIT_ASSERT_EQUAL(0, et.start());
    sleep(20);

    // Stop system
    CPPUNIT_ASSERT_EQUAL(0, et.stop());

//    et.show();

    // Quit ExecThread
    Event e(QUIT_EVENT);
    CPPUNIT_ASSERT_EQUAL(et.send(e), 0);
    while(et.getPendingEvents() != 0); // wait for QUIT_EVENT to be consumed
    CPPUNIT_ASSERT_EQUAL(et.join(), 0);

    // Make sure the actuator settings are correct, i.e. sin(counter)
    // The number of settings is the same as the counter of MEASUREMENT_EVENTS
    // received by the SimpleLoop
    long long measurementEvents;
    LoopThreadMap::iterator it = et._loopThreads.begin();
    LoopThread *loopThread = it->second;
    measurementEvents = loopThread->_measurementCount;

    fileName = "/tmp/SimpleAct1.dat";
    std::ifstream actuatorFile;
    actuatorFile.open(fileName.c_str(), std::ifstream::in);
    for (int i = 0; i < measurementEvents; ++i) {
        double expectedValue = sin(i);
        double value = -1;
        actuatorFile >> value;
        double diff = fabs(expectedValue - value);
        CPPUNIT_ASSERT(diff < 0.00001);
    }

    // Reading one more value from the file should generate an eof
    double noMoreValue;
    actuatorFile >> noMoreValue;
    CPPUNIT_ASSERT(actuatorFile.eof());
    
    actuatorFile.close();
}

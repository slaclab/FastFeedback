#include <fstream>
#include "ExecConfigurationTest.h"
#include "ExecConfiguration.h"
#include "PatternGenerator.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::ExecConfigurationTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

#ifdef RTEMS
#define TEST_DIR "/boot/g/lcls/vol8/epics/iocTop/FFController/Development/test/"
#else
#define TEST_DIR "/afs/slac/g/lcls/epics/iocTop/FFController/Development/test/"
#endif

/**
 * Test if ExecConfiguration is reading a file with feedback configuration
 * correctly.
 *
 * @author L.Piccoli
 */
void FF::ExecConfigurationTest::testConfigure() {
    /*
    ExecConfiguration ec;

    std::ifstream file;
    std::string fileName = TEST_DIR;
#ifdef RTEMS
    fileName += "ExecConfigRTEMS.dat";
#else
    fileName += "ExecConfig.dat";
#endif

    CPPUNIT_ASSERT_EQUAL(0, ec.configure(fileName));

    // There are 4 loops in the "ExecConfig.dat" file
    CPPUNIT_ASSERT_EQUAL(4, (int) ec._loopConfigurations.size());

    // Loop names are "Test1", "Test2", "Test3" and "Test4"
    LoopConfigurationMap::iterator it;
    std::string loopName = "Test1";
    it = ec._loopConfigurations.find(loopName);
    CPPUNIT_ASSERT(it != ec._loopConfigurations.end());

    loopName = "Test2";
    it = ec._loopConfigurations.find(loopName);
    CPPUNIT_ASSERT(it != ec._loopConfigurations.end());

    loopName = "Test3";
    it = ec._loopConfigurations.find(loopName);
    CPPUNIT_ASSERT(it != ec._loopConfigurations.end());

    loopName = "Test4";
    it = ec._loopConfigurations.find(loopName);
    CPPUNIT_ASSERT(it != ec._loopConfigurations.end());

    // Check that the PatternGenerator singleton was configured
    // with 4 patterns as described in the file "PatternConfig.dat"
    CPPUNIT_ASSERT_EQUAL(4, (int) PatternGenerator::getInstance()._patterns.size());

    // Check the measurement delay, must be 1
    CPPUNIT_ASSERT_EQUAL(1.0, ec._measurementDelay);
     * */
}

void FF::ExecConfigurationTest::testGetLoopNames() {
    /*
    std::ofstream testFile;
    testFile.open("/tmp/loops.txt", std::ifstream::trunc);

    std::vector<std::string> loopsIn;
    loopsIn.push_back("GN01");
    loopsIn.push_back("GN02");
    loopsIn.push_back("LG01");
    loopsIn.push_back("TR01");
    loopsIn.push_back("TR02");
    loopsIn.push_back("TR03");
    loopsIn.push_back("TR04");
    loopsIn.push_back("TR05");
    loopsIn.push_back("TR06");
    loopsIn.push_back("TR07");
    loopsIn.push_back("TR08");
    loopsIn.push_back("TR09");
    loopsIn.push_back("TR10");
    loopsIn.push_back("TR11");

    std::vector<std::string>::iterator it;
    for (it = loopsIn.begin(); it != loopsIn.end(); ++it) {
        testFile << "FBCK:FB02:" << *it << ":NAME" << std::endl;
    }

    testFile.close();

    ExecConfiguration ec;

    std::vector<std::string> loops;
    CPPUNIT_ASSERT_EQUAL(0, ec.getSlotNames(loops));
    CPPUNIT_ASSERT_EQUAL(loopsIn.size(), loops.size());
    for (int i = 0; i < (int) loops.size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(loops[i], loopsIn[i]);
    }

     */
}

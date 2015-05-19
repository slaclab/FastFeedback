#include <stdexcept>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestSuite.h>
#include <cppunit/Exception.h>
#include "CaChannel.h"

#ifdef RTEMS

int runTest(char *test) {
#else

int main(int argc, char* argv[]) {
#endif
    // Command line argument is the test path. Defaults to "", which resolves to the
    // top level suite
#ifdef RTEMS
    std::string testPath = std::string(test);
#else
    std::string testPath = (argc > 1) ? std::string(argv[1]) : std::string("");
#endif

    if (testPath == "info") {
        CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry("FeedbackUnitTest");
        CppUnit::Test *suite = registry.makeTest();

        int testCount = suite->getChildTestCount();
        std::cout << "There are " << testCount << " test units:" << std::endl;
        for (int i = 0; i < testCount; ++i) {
            CppUnit::Test *t = suite->getChildTestAt(i);
            std::cout << "  " << t->getName() << "\n";
        }
        return 0;
    }

    CppUnit::TestResult controller;
    CppUnit::TestResultCollector result;
    controller.addListener(&result);
    CppUnit::TextTestProgressListener progress;
    controller.addListener(&progress);

    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    try {
        std::cout << "Running " << testPath.c_str() << std::endl;
        runner.run(controller, testPath);
        CppUnit::TextOutputter outputter(&result, std::cout);
        outputter.write();
    } catch (std::invalid_argument &e) {
        std::cerr << std::endl << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return result.wasSuccessful() ? 0 : 1;
}

#ifdef RTEMS
extern "C" {

    int ffUnitTest(char *test) {
        return runTest(test);
    };
}
#endif

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

extern int PvDataTest_force_ld;

int main(int argc, char* argv[]) {
    // Command line argument is the test path. Defaults to "", which resolves to the
    // top level suite
    std::string testPath = (argc > 1) ? std::string(argv[1]) : std::string("");

    if (testPath == "info" || testPath == "-h" || testPath == "--help") {
        CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry("FeedbackUnitTest");
        CppUnit::Test *suite = registry.makeTest();

        int testCount = suite->getChildTestCount();
	std::cout << "Usage: " << argv[0] << " [test]" << std::endl;

        std::cout << "There are " << testCount << " test units:" << std::endl;
        for (int i = 0; i < testCount; ++i) {
            CppUnit::Test *t = suite->getChildTestAt(i);
            std::cout << "  " << t->getName() << "\n";
        }

	std::cout << "If no test is specified the command " << argv[0] << " runs all of them." << std::endl;

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
      if (testPath == "") {
        std::cout << "Running all tests (please expect some error/warning messages)" << std::endl;
      }
      else {
        std::cout << "Running " << testPath.c_str() << std::endl;
      }
        runner.run(controller, testPath);
        CppUnit::TextOutputter outputter(&result, std::cout);
        outputter.write();
    } catch (std::invalid_argument &e) {
        std::cerr << std::endl << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return result.wasSuccessful() ? 0 : 1;
}


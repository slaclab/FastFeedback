#ifndef __FFXYTRAJFITTEST_H__
#define __FFXYTRAJFITTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <iostream>
#include <string>

class ffXYTrajFitTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ffXYTrajFitTest);
  CPPUNIT_TEST(testTrajectory);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testTrajectory();
};

#endif

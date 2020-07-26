/* 
 * File:   TimerTest.h
 * Author: lpiccoli
 *
 * Created on May 27, 2010, 2:37 PM
 */

#ifndef _TIMERTEST_H
#define	_TIMERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class TimerTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TimerTest);
  CPPUNIT_TEST(testCallback);
  CPPUNIT_TEST(testElapsedTime);
  //  CPPUNIT_TEST(testRepeatedCallback);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown() {};

  void testCallback();
  void testElapsedTime();
  //  void testRepeatedCallback();
};

FF_NAMESPACE_END

#endif	/* _TIMERTEST_H */


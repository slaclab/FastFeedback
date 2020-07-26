/* 
 * File:   TimeTest.h
 * Author: lpiccoli
 *
 * Created on September 10, 2010, 11:58 AM
 */

#ifndef _TIMETEST_H
#define	_TIMETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Defs.h"

FF_NAMESPACE_START

class TimeTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TimeTest);
  //CPPUNIT_TEST(testDifference);
  CPPUNIT_TEST(test1HzDifference);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testDifference();
  void test1HzDifference();
};

FF_NAMESPACE_END

#endif	/* _TIMETEST_H */


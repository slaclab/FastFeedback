/* 
 * File:   TimeAverageTest.h
 * Author: lpiccoli
 *
 * Created on September 10, 2010, 11:58 AM
 */

#ifndef _TIMEAVERAGETEST_H
#define	_TIMEAVERAGETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Defs.h"

FF_NAMESPACE_START

class TimeAverageTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TimeAverageTest);
  CPPUNIT_TEST(testAverage);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testAverage();
};

FF_NAMESPACE_END

#endif	/* _TIMEAVERAGETEST_H */


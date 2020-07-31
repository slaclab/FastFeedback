/* 
 * File:   EventLoggerTest.h
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

class EventLoggerTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(EventLoggerTest);
  CPPUNIT_TEST(testLog);
  CPPUNIT_TEST(testSetSamples);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testLog();
  void testSetSamples();
};

FF_NAMESPACE_END

#endif	/* _TIMETEST_H */


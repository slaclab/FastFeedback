/*
 * File:   ThreadTest.h
 * Author: lpiccoli
 *
 * Created on May 25, 2010, 9:30 AM
 */

#ifndef _THREADTEST_H
#define	_THREADTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "Defs.h"

FF_NAMESPACE_START

class ThreadTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ThreadTest);
  CPPUNIT_TEST(testJoin);
  CPPUNIT_TEST(testQuitEvent);
  CPPUNIT_TEST(testProcessEvent);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testJoin();
  void testQuitEvent();
  void testProcessEvent();
};

FF_NAMESPACE_END
        
#endif	/* _THREADTEST_H */

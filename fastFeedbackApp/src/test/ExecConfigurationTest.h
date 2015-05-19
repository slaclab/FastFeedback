/* 
 * File:   ExecConfigurationTest.h
 * Author: lpiccoli
 *
 * Created on June 10, 2010, 2:22 PM
 */

#ifndef _EXECCONFIGURATIONTEST_H
#define	_EXECCONFIGURATIONTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "Defs.h"

FF_NAMESPACE_START

class ExecConfigurationTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ExecConfigurationTest);
  CPPUNIT_TEST(testConfigure);
  CPPUNIT_TEST(testGetLoopNames);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testGetLoopNames();
  void testConfigure();
};

FF_NAMESPACE_END

#endif	/* _EXECCONFIGURATIONTEST_H */


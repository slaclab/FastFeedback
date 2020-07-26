/* 
 * File:   DeviceTest.h
 * Author: lpiccoli
 *
 * Created on May 28, 2010, 12:04 PM
 */

#ifndef _DEVICETEST_H
#define	_DEVICETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class DeviceTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(DeviceTest);
  CPPUNIT_TEST(testEquality);
  CPPUNIT_TEST(testLessThan);
  CPPUNIT_TEST(testDeviceCompare);
  CPPUNIT_TEST(testPvComparison);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testEquality();
  void testLessThan();
  void testDeviceCompare();
  void testPvComparison();
};


FF_NAMESPACE_END

#endif	/* _DEVICETEST_H */


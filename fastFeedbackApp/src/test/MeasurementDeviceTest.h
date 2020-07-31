/* 
 * File:   MeasurementDeviceTest.h
 * Author: lpiccoli
 *
 * Created on June 2, 2010, 8:52 AM
 */

#ifndef _MEASUREMENTDEVICETEST_H
#define	_MEASUREMENTDEVICETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class MeasurementDeviceTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(MeasurementDeviceTest);
  CPPUNIT_TEST(testFailSetWrite);
  CPPUNIT_TEST(testFileChannel);
  CPPUNIT_TEST(testAverage);
  CPPUNIT_TEST(testPeek);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testFailSetWrite();
  void testFileChannel();
  void testAverage();
  void testPeek();
};

FF_NAMESPACE_END

#endif	/* _MEASUREMENTDEVICETEST_H */


/* 
 * File:   LoopConfigurationTest.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 11:40 AM
 */

#ifndef _LOOPCONFIGURATIONTEST_H
#define	_LOOPCONFIGURATIONTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class LoopConfigurationTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(LoopConfigurationTest);
  CPPUNIT_TEST(testInitializeDevices);
  CPPUNIT_TEST(testReconfigureDevices);
  CPPUNIT_TEST(testConfigurePatterns);
  CPPUNIT_TEST(testConfigureFMatrix);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testInitializeDevices();
  void testReconfigureDevices();
  void testConfigurePatterns();
  void testConfigureFMatrix();
};

FF_NAMESPACE_END

#endif	/* _LOOPCONFIGURATIONTEST_H */


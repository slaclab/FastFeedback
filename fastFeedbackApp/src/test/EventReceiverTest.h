/* 
 * File:   EventReceiverTest.h
 * Author: lpiccoli
 *
 * Created on May 24, 2010, 2:30 PM
 */

#ifndef _EVENTRECEIVERTEST_H
#define	_EVENTRECEIVERTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

class EventReceiverTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(EventReceiverTest);
  CPPUNIT_TEST(testSendReceive);
  CPPUNIT_TEST(testPending);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testSendReceive();
  void testPending();
};

#endif	/* _EVENTRECEIVERTEST_H */


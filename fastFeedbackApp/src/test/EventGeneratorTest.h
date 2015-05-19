/* 
 * File:   EventGeneratorTest.h
 * Author: lpiccoli
 *
 * Created on May 26, 2010, 9:53 AM
 */

#ifndef _EVENTGENERATORTEST_H
#define	_EVENTGENERATORTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

class EventGeneratorTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(EventGeneratorTest);
  CPPUNIT_TEST(testAddReceiver);
  CPPUNIT_TEST(testRemoveReceiver);
  CPPUNIT_TEST(testSendEvent);
  CPPUNIT_TEST(testEmpty);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testAddReceiver();
  void testRemoveReceiver();
  void testSendEvent();
  void testEmpty();
};


#endif	/* _EVENTGENERATORTEST_H */


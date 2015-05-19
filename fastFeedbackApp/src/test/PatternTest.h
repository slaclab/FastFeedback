/* 
 * File:   PatternTest.h
 * Author: lpiccoli
 *
 * Created on May 26, 2010, 1:54 PM
 */

#ifndef _PATTERNTEST_H
#define	_PATTERNTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

class PatternTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PatternTest);
  CPPUNIT_TEST(testConstructor);
  CPPUNIT_TEST(testCompare);
  CPPUNIT_TEST(testAssign);
  CPPUNIT_TEST(testPatternMap);
  CPPUNIT_TEST(testMatch);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testConstructor();
  void testCompare();
  void testAssign();
  void testPatternMap();
  void testMatch();
};

#endif	/* _PATTERNTEST_H */


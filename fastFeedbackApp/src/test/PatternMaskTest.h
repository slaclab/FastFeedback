/* 
 * File:   PatternMaskTest.h
 * Author: lpiccoli
 *
 * Created on May 26, 2010, 1:54 PM
 */

#ifndef _PatternMaskTEST_H
#define	_PatternMaskTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "Defs.h"

FF_NAMESPACE_START

class PatternMaskTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PatternMaskTest);
  CPPUNIT_TEST(testCompare);
  CPPUNIT_TEST(testAssign);
  CPPUNIT_TEST(testAssignVector);
  CPPUNIT_TEST(testPatternMaskMap);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testCompare();
  void testAssign();
  void testAssignVector();
  void testPatternMaskMap();
};

FF_NAMESPACE_END

#endif	/* _PatternMaskTEST_H */


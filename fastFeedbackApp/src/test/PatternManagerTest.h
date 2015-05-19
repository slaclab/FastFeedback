/* 
 * File:   PatternManagerTest.h
 * Author: lpiccoli
 *
 * Created on May 26, 2010, 2:52 PM
 */

#ifndef _PATTERNMANAGERTEST_H
#define	_PATTERNMANAGERTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "PatternMask.h"
#include "Defs.h"

FF_NAMESPACE_START

class PatternManagerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PatternManagerTest);
    CPPUNIT_TEST(testAddPattern);
    CPPUNIT_TEST(testAddPatternReceiver);
    CPPUNIT_TEST(testRemovePattern);
    CPPUNIT_TEST(testRemovePatternReceiver);
    CPPUNIT_TEST(testProcess);
//    CPPUNIT_TEST(testGetPatterns);
    //CPPUNIT_TEST(testGetPatternIndex);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

    void tearDown() {
    };

    void testAddPattern();
    void testAddPatternReceiver();
    void testRemovePattern();
    void testRemovePatternReceiver();
    void testProcess();
//    void testGetPatterns();
    //void testGetPatternIndex();

private:
    PatternMask p1;
    PatternMask p2;
    PatternMask p3;
    PatternMask p4;
    PatternMask p5;
};

FF_NAMESPACE_END

#endif	/* _PATTERNMANAGERTEST_H */


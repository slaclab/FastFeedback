/* 
 * File:   PatternGeneratorTest.h
 * Author: lpiccoli
 *
 * Created on June 10, 2010, 2:45 PM
 */

#ifndef _PATTERNGENERATORTEST_H
#define	_PATTERNGENERATORTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "PatternMask.h"
#include "Defs.h"

FF_NAMESPACE_START

class PatternGeneratorTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PatternGeneratorTest);
    CPPUNIT_TEST(testNextPattern);
    CPPUNIT_TEST(testGenerate);
    CPPUNIT_TEST(testStartStop);
    CPPUNIT_TEST(testRemovePattern);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

    void tearDown() {
    };

    void testNextPattern();
    void testGenerate();
    void testStartStop();
    void testRemovePattern();

private:
    FastFeedback::PatternMask p1;
    FastFeedback::PatternMask p2;
    FastFeedback::PatternMask p3;
};

FF_NAMESPACE_END

#endif	/* _PATTERNGENERATORTEST_H */


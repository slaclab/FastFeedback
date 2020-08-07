/* 
 * File:   LoopThreadTest.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 2:14 PM
 */

#ifndef _LOOPTHREADTEST_H
#define	_LOOPTHREADTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <iostream>
#include "Pattern.h"
#include "PatternMask.h"
#include "LoopConfiguration.h"

FF_NAMESPACE_START

class LoopThreadTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(LoopThreadTest);
    CPPUNIT_TEST(testCreateLoops);
    CPPUNIT_TEST(testProcessPattern);
    CPPUNIT_TEST(testProcessMeasurement);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void testCreateLoops();
    void testProcessPattern();
    void testProcessMeasurement();

private:
    FastFeedback::PatternMask p1;
    FastFeedback::PatternMask p2;
    FastFeedback::PatternMask p3;

    FastFeedback::LoopConfiguration *config;
};

FF_NAMESPACE_END

#endif	/* _LOOPTHREADTEST_H */


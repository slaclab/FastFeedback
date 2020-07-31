/* 
 * File:   SineWaveTest.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 4:02 PM
 */

#ifndef _SINEWAVETEST_H
#define	_SINEWAVETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class SineWaveTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SineWaveTest);
    CPPUNIT_TEST(testCalculate);
    CPPUNIT_TEST(testCalculateMode1);
    CPPUNIT_TEST(testCalculateMode2);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    };

    void tearDown() {
    };

    void testCalculate();
    void testCalculateMode1();
    void testCalculateMode2();
};

FF_NAMESPACE_END

#endif	/* _SINEWAVETEST_H */


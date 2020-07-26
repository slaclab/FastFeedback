/* 
 * File:   LoopTest.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 3:40 PM
 */

#ifndef _LOOPTEST_H
#define	_LOOPTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class LoopTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(LoopTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCalculateSine);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    };

    void tearDown() {
    };

    void testConstructor();
    void testCalculateSine();
};

FF_NAMESPACE_END

#endif	/* _LOOPTEST_H */


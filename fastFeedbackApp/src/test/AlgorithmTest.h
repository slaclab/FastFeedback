/* 
 * File:   AlgorithmTest.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 3:48 PM
 */

#ifndef _ALGORITHMTEST_H
#define	_ALGORITHMTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "Defs.h"

FF_NAMESPACE_START

class AlgorithmTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(AlgorithmTest);
    CPPUNIT_TEST(testX);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    };

    void tearDown() {
    };

    void testX();
};

FF_NAMESPACE_END

#endif	/* _ALGORITHMTEST_H */


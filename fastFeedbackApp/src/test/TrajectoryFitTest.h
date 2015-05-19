/* 
 * File:   TrajectoryFitTest.h
 * Author: lpiccoli
 *
 * Created on July 19, 2010, 11:21 AM
 */

#ifndef _TRAJECTORYFITTEST_H
#define	_TRAJECTORYFITTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

#include "Defs.h"

FF_NAMESPACE_START

class TrajectoryFitTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TrajectoryFitTest);
    CPPUNIT_TEST(testCalculateInjLaunch);
    CPPUNIT_TEST(testGelsTime);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    };

    void tearDown() {
    };

    void testCalculateInjLaunch();
    void testGelsTime();
};


FF_NAMESPACE_END

#endif	/* _TRAJECTORYFITTEST_H */


/* 
 * File:   ActuatorDeviceTest.h
 * Author: lpiccoli
 *
 * Created on June 1, 2010, 2:23 PM
 */

#ifndef _ACTUATORDEVICETEST_H
#define	_ACTUATORDEVICETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class ActuatorDeviceTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ActuatorDeviceTest);
    CPPUNIT_TEST(testDropPoints);
    CPPUNIT_TEST(testFailGetRead);
    CPPUNIT_TEST(testFileChannel);
    CPPUNIT_TEST(testPeek);
    CPPUNIT_TEST(testActuatorCompare);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    };

    void tearDown() {
    };

    void testDropPoints();
    void testFailGetRead();
    void testFileChannel();
    void testPeek();
    void testActuatorCompare();
};

FF_NAMESPACE_END

#endif	/* _ACTUATORDEVICETEST_H */


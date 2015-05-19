/* 
 * File:   StateDeviceTest.h
 * Author: lpiccoli
 *
 * Created on June 2, 2010, 3:56 PM
 */

#ifndef _STATEDEVICETEST_H
#define	_STATEDEVICETEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

#include "SetpointDevice.h"

class StateDeviceTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(StateDeviceTest);
    CPPUNIT_TEST(testError);
    CPPUNIT_TEST(testWrite);
    CPPUNIT_TEST_SUITE_END();

private:
    FastFeedback::SetpointDevice *setpoint;

public:
    void setUp();
    void tearDown();

    void testError();
    void testWrite();
};


#endif	/* _STATEDEVICETEST_H */


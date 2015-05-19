/* 
 * File:   CollectorThreadTest.h
 * Author: lpiccoli
 *
 * Created on June 4, 2010, 3:39 PM
 */

#ifndef _COLLECTORTHREADTEST_H
#define	_COLLECTORTHREADTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

#include "Pattern.h"
#include "PatternMask.h"
#include "MeasurementDevice.h"
#include "NullChannel.h"

FF_NAMESPACE_START

class CollectorThreadTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(CollectorThreadTest);
    CPPUNIT_TEST(testProcessPattern);
    CPPUNIT_TEST(testProcessMeasurement);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testProcessPattern();
    void testProcessMeasurement();

private:    
    PatternMask p1;
    PatternMask p2;
    PatternMask p3;

    MeasurementDevice *m1;
    MeasurementDevice *m2;
    MeasurementDevice *m3;
    MeasurementDevice *m4;

    NullChannel *nc1;
    NullChannel *nc2;
    NullChannel *nc3;
    NullChannel *nc4;
};

FF_NAMESPACE_END

#endif	/* _COLLECTORTHREADTEST_H */


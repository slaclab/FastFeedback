/* 
 * File:   MeasurementCollectorTest.h
 * Author: lpiccoli
 *
 * Created on June 3, 2010, 2:10 PM
 */

#ifndef _MEASUREMENTCOLLECTORTEST_H
#define	_MEASUREMENTCOLLECTORTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Pattern.h"
#include "PatternMask.h"
#include "MeasurementDevice.h"
#include "NullChannel.h"
#include "Defs.h"

FF_NAMESPACE_START

class MeasurementCollectorTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MeasurementCollectorTest);
    CPPUNIT_TEST(testHasPattern);
    CPPUNIT_TEST(testAddDevice);
    CPPUNIT_TEST(testUpdate);
    CPPUNIT_TEST(testRemoveDevice);
    CPPUNIT_TEST(testClear);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testHasPattern();
    void testAddDevice();
    void testUpdate();
    void testRemoveDevice();
    void testClear();

private:
    PatternMask p1;
    PatternMask p2;
    PatternMask p3;
    PatternMask p4;

    MeasurementDevice *m1;
    MeasurementDevice *m1Clone;
    MeasurementDevice *m2;
    MeasurementDevice *m3;
    MeasurementDevice *m4;
    MeasurementDevice *m5;

    NullChannel *nc1;
    NullChannel *nc2;
    NullChannel *nc3;
    NullChannel *nc4;
    NullChannel *nc5;
};

FF_NAMESPACE_END

#endif	/* _MEASUREMENTCOLLECTORTEST_H */


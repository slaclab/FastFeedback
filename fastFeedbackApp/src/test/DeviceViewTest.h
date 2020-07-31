/* 
 * File:   DeviceViewTest.h
 * Author: lpiccoli
 *
 * Created on June 3, 2010, 5:07 PM
 */

#ifndef _DEVICEVIEWTEST_H
#define	_DEVICEVIEWTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Defs.h"
#include "MeasurementDevice.h"
#include "ActuatorDevice.h"
#include "StateDevice.h"
#include "NullChannel.h"
#include "FileChannel.h"
#include "Pattern.h"
#include "PatternMask.h"

FF_NAMESPACE_START

class DeviceViewTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(DeviceViewTest);
  CPPUNIT_TEST(testAddDevice);
  CPPUNIT_TEST(testUpdateMeasurements);
  CPPUNIT_TEST(testUpdateActuators);
  CPPUNIT_TEST(testUpdateSlow);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testAddDevice();
  void testUpdateMeasurements();
  void testUpdateActuators();
  void testUpdateSlow();

private:
    FastFeedback::PatternMask p1;
    FastFeedback::PatternMask p2;
    FastFeedback::PatternMask p3;

    int size;

    FastFeedback::ActuatorDevice *a1;
    FastFeedback::ActuatorDevice *a2;
    FastFeedback::StateDevice *s1;
    FastFeedback::StateDevice *s2;

    FastFeedback::MeasurementDevice *m1;
    FastFeedback::MeasurementDevice *m2;
    FastFeedback::MeasurementDevice *sp1;
    FastFeedback::MeasurementDevice *sp2;

    FastFeedback::FileChannel *fc1; // for a1
    FastFeedback::FileChannel *fc2; // for a2
    FastFeedback::FileChannel *fc3; // for s1
    FastFeedback::FileChannel *fc4; // for s2

    FastFeedback::NullChannel *nc1; // for m1
    FastFeedback::NullChannel *nc2; // for m2
    FastFeedback::NullChannel *nc3; // for sp1
    FastFeedback::NullChannel *nc4; // for sp2
};

FF_NAMESPACE_END

#endif	/* _DEVICEVIEWTEST_H */


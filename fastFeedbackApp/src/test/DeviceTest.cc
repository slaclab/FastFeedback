#include "DeviceTest.h"
#include "Device.h"
#include <set>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::DeviceTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

/**
 * Test if device equality operator works properly. Devices with same name
 * and pattern are equal.
 *
 * @author L.Piccoli
 */
void FF::DeviceTest::testEquality() {
    std::string name;
    name = "BarackObama";
    std::string slotName = "XX00";
    Device device1(slotName, name, 10);
    Device device2(slotName, name, 20);

    PatternMask p1;
    PatternMask p2;
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = 12 + i;
        p2._inclusionMask[i] = p1._inclusionMask[i];
    }
    p2._inclusionMask[3]++; // Make pattern a bit different
    device1.setPatternMask(p1);
    device2.setPatternMask(p1);

    // device1 and device2 are considered the same even though the buffers
    // have different size
    CPPUNIT_ASSERT(device1 == device2);

    // Change pattern of device2, now they are different.
    device2.setPatternMask(p2);
    CPPUNIT_ASSERT(!(device1 == device2));
    CPPUNIT_ASSERT(device1 != device2);
}

/**
 * Test the operator< to make sure the Device has the name and the pattern
 * compared. That operator is used to order Devices in std::sets.
 *
 * @author L.Piccoli
 */
void FF::DeviceTest::testLessThan() {
    std::string name = "Lula";
    std::string slotName = "XX00";
    Device device1(slotName, name, 10);
    Device device2(slotName, name, 20);

    PatternMask p1;
    PatternMask p2;
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = 12 + i;
        p2._inclusionMask[i] = p1._inclusionMask[i];
    }
    p2._inclusionMask[3]++; // Make pattern a bit different
    device1.setPatternMask(p1);
    device2.setPatternMask(p1);

    // device1 < device2 must be false because they are the same device
    CPPUNIT_ASSERT_EQUAL(0, device1.getName().compare(device2.getName()));
    CPPUNIT_ASSERT(!(device1 < device2));
    CPPUNIT_ASSERT(!(device2 < device1));

    // Make device2 > device1 by setting the pattern to p2, which is > p1
    device2.setPatternMask(p2);
    CPPUNIT_ASSERT(device1 < device2);

    // device3 > device1 because "aLula" > "Lula"
    std::string anotherName = "aLula";
    Device device3(slotName, anotherName, 1);
    device3.setPatternMask(p1);
    CPPUNIT_ASSERT(anotherName.compare(name) > 0);
    CPPUNIT_ASSERT(device1 < device3);
    CPPUNIT_ASSERT(!(device3 < device1));
}

/**
 * Test the DeviceCompare, which implements the comparison between two Device
 * pointers.
 *
 * @author L.Piccoli
 */
void FF::DeviceTest::testDeviceCompare() {
    std::string name = "Lula";
    std::string slotName = "XX00";
    Device device1(slotName, name, 10);
    Device device2(slotName, name, 20);

    PatternMask p1;
    PatternMask p2;
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = 12 + i;
        p2._inclusionMask[i] = p1._inclusionMask[i];
    }
    p2._inclusionMask[3]++; // Make pattern a bit different
    device1.setPatternMask(p1);
    device2.setPatternMask(p1);

    std::set<Device *, DeviceCompare> deviceSet;

    deviceSet.insert(&device1);
    CPPUNIT_ASSERT_EQUAL(1, (int) deviceSet.size());

    // Inserting the device with same name and pattern should fail
    deviceSet.insert(&device2);
    CPPUNIT_ASSERT_EQUAL(1, (int) deviceSet.size());

    // Now, change device2's pattern to p2 and the insert should work
    device2.setPatternMask(p2);
    deviceSet.insert(&device2);
    CPPUNIT_ASSERT_EQUAL(2, (int) deviceSet.size());
}

void FF::DeviceTest::testPvComparison() {
    Device deviceM1("XX00", "M1", 1);
    Device deviceM2("XX00", "M2", 1);
    Device deviceM10("XX00", "M10", 1);
    Device deviceM22("XX00", "M22", 1);

    CPPUNIT_ASSERT(deviceM1 < deviceM10);
    CPPUNIT_ASSERT(deviceM1 < deviceM22);
    CPPUNIT_ASSERT(deviceM1 < deviceM2);
    CPPUNIT_ASSERT(deviceM2 < deviceM10);
    CPPUNIT_ASSERT(deviceM2 < deviceM22);
    CPPUNIT_ASSERT(deviceM10 < deviceM22);
}

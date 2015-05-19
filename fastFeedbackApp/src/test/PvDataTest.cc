#include "PvDataTest.h"
#include "PvData.h"
#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(PvDataTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void PvDataTest::setUp() {
    PvDataChar::getPvMap().clear();
    PvDataUChar::getPvMap().clear();
    PvDataInt::getPvMap().clear();
    PvDataUInt::getPvMap().clear();
}

/**
 * @author L.Piccoli
 */
void PvDataTest::testAssign() {
    PvData<int> a("FF1");
    PvData<int> b("FF2");
    PvData<short> c("FF3");
    PvData<int> d("FF4");

    a = 1;
    b = 1;
    CPPUNIT_ASSERT(a == 1);
    // This does not compile, is there a way to call operator== of a in this case?
    // CPPUNIT_ASSERT(1 == a);
    CPPUNIT_ASSERT(a == b);
    CPPUNIT_ASSERT(b == a);
    CPPUNIT_ASSERT(a == a);
    a = b;
    CPPUNIT_ASSERT(a == b);
    CPPUNIT_ASSERT(b == a);
    c = 1;
    // This does not compile ;)
    // CPPUNIT_ASSERT(c == a);
    d = 2;
    CPPUNIT_ASSERT(a != d);
    CPPUNIT_ASSERT(d != b);
}

/**
 * @author L.Piccoli
 */
void PvDataTest::testCheckMap() {
    PvData<char> a("char");
    PvData<unsigned char> b("uchar");
    PvData<int> c("int");
    PvData<unsigned int> d("uint");
    PvData<long> e("long");
    PvData<unsigned long> f("ulong");
    PvData<double> g("double");
    PvData<short> h("short");
    PvData<unsigned short> i("ushort");
    PvData<std::string> k("string");

    // Each PvData must have been added to the map
    CPPUNIT_ASSERT_EQUAL(1, (int) PvDataChar::getPvMap().size());
    CPPUNIT_ASSERT_EQUAL(1, (int) PvDataUChar::getPvMap().size());
    CPPUNIT_ASSERT_EQUAL(1, (int) PvDataInt::getPvMap().size());
    CPPUNIT_ASSERT_EQUAL(1, (int) PvDataUInt::getPvMap().size());

    // A duplicate PV name is not allowed
    PvData<int> x("int");
    CPPUNIT_ASSERT_EQUAL(1, (int) PvDataInt::getPvMap().size());
}

/**
 * Simulate the steps taken to create a device support.
 *
 * @author L.Piccoli
 */
void PvDataTest::testDeviceSupport() {
    /* TODO: rewrite or remove this test
   // Create the PvDatas...
    PvData<int> _measurementDelay("TIMERDELAY");
    PvData<double> _a1Hihi("A1HIHI");
    PvData<std::string> _a1DevDame("A1DEVNAME");

    // When iocInit is called, the INST_IO string is used
    // to search for the PvData
    std::string pvName = "A1HIHI";
    CPPUNIT_ASSERT(PvDataChar::getPvMap().find(pvName) == PvDataChar::getPvMap().end());
    CPPUNIT_ASSERT(PvDataUChar::getPvMap().find(pvName) == PvDataUChar::getPvMap().end());
    CPPUNIT_ASSERT(PvDataInt::getPvMap().find(pvName) == PvDataInt::getPvMap().end());
    // Must check all maps...
    // Until the right one is found:
    PvMapDouble::iterator it = PvDataDouble::getPvMap().find(pvName);
    CPPUNIT_ASSERT(it != PvDataDouble::getPvMap().end());

    void *dpvt = it->second->getValueAddress();

    // If data is being set to the PV then just copy it to dpvt.
    // The types will match because the precord->val has the same
    // type as the PvData
    double newValue = 0.1415;
    *((double *) dpvt) = newValue;

    CPPUNIT_ASSERT(_a1Hihi == newValue);
    double readBack = 2.71;
    _a1Hihi = 2.71;

    // If data is being read from the PV, copy the value from dpvt
    readBack = -1;
    readBack = *(double *) dpvt;
    CPPUNIT_ASSERT(_a1Hihi == readBack);
     * */
}

void PvDataTest::testWrite() {
    std::string aString = "hi";
    PvDataString myString("str", aString);
    CPPUNIT_ASSERT(myString == aString);
    std::string otherString = "hello";
    myString.write(&otherString);
    CPPUNIT_ASSERT(myString == otherString);
}

void PvDataTest::testReadExternal() {
    std::string aString = "hi";
    PvDataString myString;
    myString.setExternalValue(&aString);

    std::string readBack = "xxx";
    myString.read(&readBack);
    CPPUNIT_ASSERT(readBack == aString);
    CPPUNIT_ASSERT(readBack == "hi");
}

void PvDataTest::testWriteExternal() {
    long val = -10;
    PvDataLong myLong;
    myLong.setExternalValue(&val);

    long newVal = 22;
    myLong.write(&newVal);
    CPPUNIT_ASSERT_EQUAL(newVal, val);
}

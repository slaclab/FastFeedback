#include "PatternTest.h"
#include "Pattern.h"
#include "PatternMask.h"

#include "Log.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::PatternTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::PatternTest::testConstructor() {
    Log l;
    //    l.logToConsole();
    l << "Test message number=" << 1 << " double=" << 3.1415 << Log::flush;

    l << "Another message PI=" << 3.1415 << Log::flush;

    epicsTimeStamp timestamp;
    timestamp.nsec = 10;
    timestamp.secPastEpoch = 20;
    evrModifier_ta mask;
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        mask[i] = i;
    }
    Pattern p(mask, timestamp);
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        CPPUNIT_ASSERT_EQUAL((epicsUInt32) i, p._value[i]);
    }
    CPPUNIT_ASSERT_EQUAL((epicsUInt32) 10, p._timestamp.nsec);
    CPPUNIT_ASSERT_EQUAL((epicsUInt32) 20, p._timestamp.secPastEpoch);
}

void FF::PatternTest::testAssign() {
    Pattern p1;

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        p1._value[i] = i;
    }

    Pattern p2 = p1;

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        CPPUNIT_ASSERT_EQUAL(p1._value[i], p2._value[i]);
    }

    Pattern p3(p1);
    CPPUNIT_ASSERT(p3 == p1);
    CPPUNIT_ASSERT(!(p3 < p1));

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        CPPUNIT_ASSERT_EQUAL(p1._value[i], p3._value[i]);
    }
}

void FF::PatternTest::testCompare() {
    Pattern p1;

    p1._value[0] = 0;
    p1._value[1] = 1;
    p1._value[2] = 2;
    p1._value[3] = 3;
    p1._value[4] = 4;
    p1._value[5] = 5;
    p1._timestamp.nsec = 1;
    p1._timestamp.secPastEpoch = 0;
    p1._status = 4;

    Pattern p2 = p1;

    p2._value[0] = 2;
    p2._timestamp.nsec = 9;
    p2._timestamp.secPastEpoch = 0;
    p2._status = 66;

    CPPUNIT_ASSERT(p1 != p2);

    p1._value[0] = 2;
    CPPUNIT_ASSERT(p1 == p2);
}

void FF::PatternTest::testPatternMap() {
    Pattern p1;
    Pattern p3;
    Pattern p2;
    std::map<Pattern, int> pmap;

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        p1._value[i] = i;
        p3._value[i] = i;
        p2._value[i] = 100 + i;
    }

    pmap.insert(std::pair<Pattern, int>(p1, 1));
    std::map<Pattern, int>::iterator it;

    it = pmap.find(p1);
    CPPUNIT_ASSERT(it != pmap.end());

    it = pmap.find(p3);
    CPPUNIT_ASSERT(it != pmap.end());

    it = pmap.find(p2);
    CPPUNIT_ASSERT(it == pmap.end());
}

void FF::PatternTest::testMatch() {
    PatternMask m1;
    epicsUInt32 maskBit = 0x1;
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        m1._inclusionMask[i] = maskBit;
        maskBit <<= 1;
    }
    maskBit = 0x8000;
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        m1._exclusionMask[i] = maskBit;
        maskBit >>= 1;
    }

    Pattern p1(m1);
    CPPUNIT_ASSERT_EQUAL(true, p1.match(m1));

    p1._value[2] = 0;
    CPPUNIT_ASSERT_EQUAL(false, p1.match(m1));

    m1._inclusionMask[2] = 0;
    CPPUNIT_ASSERT_EQUAL(true, p1.match(m1));

    //0x8000 0x6000001 0x0 0x2c003ff0 0x3f00000 0x0
    p1._value[0] = 0x8000;
    p1._value[1] = 0x7000001;
    p1._value[2] = 0;
    p1._value[3] = 0x2c003ff0;
    p1._value[4] = 0x3f00000;
    p1._value[5] = 0;

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        m1._inclusionMask[i] = 0;
        m1._exclusionMask[i] = 0;
    }
    m1._inclusionMask[1] = 0x1000000;
    //m1._inclusionMask[2] = 0x80000;

    CPPUNIT_ASSERT_EQUAL(true, p1.match(m1));

    PatternMask m2;
    std::vector<unsigned short> v;

    // inclusion mask
    v.push_back(0);
    v.push_back(256);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);

    // exclusion mask
    v.push_back(0);
    v.push_back(0);
    v.push_back(54);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);

    // time slot
    v.push_back(0);

    m2 = v;

    CPPUNIT_ASSERT_EQUAL(true, p1.match(m2));
}

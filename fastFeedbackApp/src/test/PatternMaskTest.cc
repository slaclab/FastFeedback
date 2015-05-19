#include "PatternMaskTest.h"
#include "PatternMask.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(PatternMaskTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void PatternMaskTest::testAssign() {
    PatternMask p1;

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        p1._inclusionMask[i] = i + 10;
        p1._exclusionMask[i] = i + 20;
    }
    p1._timeSlot = 101;

    PatternMask p2 = p1;

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        CPPUNIT_ASSERT_EQUAL(p1._inclusionMask[i], p2._inclusionMask[i]);
        CPPUNIT_ASSERT_EQUAL(p1._exclusionMask[i], p2._exclusionMask[i]);
    }
    CPPUNIT_ASSERT_EQUAL(p1._timeSlot, p2._timeSlot);

    PatternMask p3(p1);
    CPPUNIT_ASSERT(p3 == p1);
    CPPUNIT_ASSERT(!(p3 < p1));

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        CPPUNIT_ASSERT_EQUAL(p1._inclusionMask[i], p3._inclusionMask[i]);
        CPPUNIT_ASSERT_EQUAL(p1._exclusionMask[i], p3._exclusionMask[i]);
    }
    CPPUNIT_ASSERT_EQUAL(p1._timeSlot, p3._timeSlot);
}

void PatternMaskTest::testAssignVector() {
    PatternMask p1;
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        p1._inclusionMask[i] = i + 10;
        p1._exclusionMask[i] = i + 20;
    }
    p1._timeSlot = 101;

    // Assigning a vector with less than 21 elements should fail, i.e. PatternMask
    // will remain the same
    std::vector<unsigned short> vSmall(14);
    p1 = vSmall;
    for (epicsUInt32 i = 0; i < MAX_EVR_MODIFIER; ++i) {
        CPPUNIT_ASSERT_EQUAL(i + 10, p1._inclusionMask[i]);
        CPPUNIT_ASSERT_EQUAL(i + 20, p1._exclusionMask[i]);
    }
    CPPUNIT_ASSERT_EQUAL((unsigned long) 101, p1._timeSlot);

    // v[0..9] -> inclusion mask
    // v[10..19] -> exclusion mask
    // v[20] -> time slot
    std::vector<unsigned short> v(21);
    for (unsigned short i = 0; i < (unsigned short) v.size(); ++i) {
        v[i] = 100 + i;
    }

    p1 = v;
    for (epicsUInt32 i = 0; i < MAX_EVR_MODIFIER - 1; ++i) {
        epicsUInt32 val = v[i * 2 + 1] << 16 | v[i * 2];
        CPPUNIT_ASSERT_EQUAL(val, p1._inclusionMask[i]);
    }
    CPPUNIT_ASSERT_EQUAL((epicsUInt32) 0, p1._inclusionMask[MAX_EVR_MODIFIER - 1]);
    int maskIndex = 0;
    for (epicsUInt32 i = MAX_EVR_MODIFIER - 1; i < (MAX_EVR_MODIFIER - 1) * 2 - 1; ++i) {
        epicsUInt32 val = v[i * 2 + 1] << 16 | v[i * 2];
        CPPUNIT_ASSERT_EQUAL(val, p1._exclusionMask[maskIndex]);
        maskIndex++;
    }
    CPPUNIT_ASSERT_EQUAL((epicsUInt32) 0, p1._exclusionMask[MAX_EVR_MODIFIER - 1]);
    CPPUNIT_ASSERT_EQUAL((unsigned long) v[20], p1._timeSlot);
}

void PatternMaskTest::testCompare() {
    PatternMask p1;
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        p1._inclusionMask[i] = i + 10;
        p1._exclusionMask[i] = i + 20;
    }
    p1._timeSlot = 5;

    PatternMask p2 = p1;
    CPPUNIT_ASSERT(p1 == p2);

    p2._timeSlot = 3;
    CPPUNIT_ASSERT(p1 != p2);

    p2 = p1;
    p2._exclusionMask[2] = 1;
    CPPUNIT_ASSERT(p1 != p2);

    p2 = p1;
    p2._inclusionMask[2] = 13;
    CPPUNIT_ASSERT(p1 != p2);
}

void PatternMaskTest::testPatternMaskMap() {
    PatternMask p1;
    PatternMask p3;
    PatternMask p2;
    std::map<PatternMask, int> pmap;

    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        p1._inclusionMask[i] = i + 10;
        p1._exclusionMask[i] = i + 20;
        p2._inclusionMask[i] = 100 + i + 10;
        p2._exclusionMask[i] = 100 + i + 20;
    }
    p1._timeSlot = 101;
    p2._timeSlot = 101;
    p3 = p1;

    pmap.insert(std::pair<PatternMask, int>(p1, 1));
    std::map<PatternMask, int>::iterator it;

    it = pmap.find(p1);
    CPPUNIT_ASSERT(it != pmap.end());

    it = pmap.find(p3);
    CPPUNIT_ASSERT(it != pmap.end());

    it = pmap.find(p2);
    CPPUNIT_ASSERT(it == pmap.end());

    PatternMask m1;
    PatternMask m2;
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        m1._inclusionMask[i] = i;
        m1._exclusionMask[i] = 0xffffffff;
    }
    m1._timeSlot = 10;
    pmap.clear();
    pmap.insert(std::pair<PatternMask, int>(m1, 1));

    m2._inclusionMask[0] = 0;
    m2._inclusionMask[1] = 0x1000000;
    m2._inclusionMask[2] = 0x80000;
    m2._inclusionMask[3] = 0;
    m2._inclusionMask[4] = 0;
    m2._inclusionMask[5] = 0;

    m2._exclusionMask[0] = 0;
    m2._exclusionMask[1] = 0x36;
    m2._exclusionMask[2] = 0;
    m2._exclusionMask[3] = 0;
    m2._exclusionMask[4] = 0;
    m2._exclusionMask[5] = 0;

    m2._timeSlot = 0;

    it = pmap.find(m2);
    CPPUNIT_ASSERT(m1 != m2);
    CPPUNIT_ASSERT(!(m1 == m2));
    CPPUNIT_ASSERT(m2 < m1);
    CPPUNIT_ASSERT(it == pmap.end());
}

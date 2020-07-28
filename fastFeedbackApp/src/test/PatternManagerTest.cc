#include "PatternManagerTest.h"
#include "PatternManager.h"
#include "EventReceiver.h"
#include "Event.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::PatternManagerTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::PatternManagerTest::setUp() {
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = i;
        p2._inclusionMask[i] = i + 10;
        p3._inclusionMask[i] = i + 20;
        p4._inclusionMask[i] = i + 30;
    }
    p5 = p1;
    p5._exclusionMask[2] = 0x8000;
}

void FF::PatternManagerTest::testAddPattern() {
    PatternManager pm;

    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1));
    CPPUNIT_ASSERT_EQUAL(-1, pm.add(p1));
    CPPUNIT_ASSERT_EQUAL(1, (int) pm._generators.size());

    p1._timeSlot = 9;

    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1));
    CPPUNIT_ASSERT_EQUAL(2, (int) pm._generators.size());
}

void FF::PatternManagerTest::testAddPatternReceiver() {
    PatternManager pm;
    EventReceiver r1; // Receives P1
    EventReceiver r2; // Receives P2
    EventReceiver r3; // Receives P1 and P2

    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1, r1));
    CPPUNIT_ASSERT_EQUAL(1, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(0, pm.add(p2, r2));
    CPPUNIT_ASSERT_EQUAL(2, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1, r3));
    CPPUNIT_ASSERT_EQUAL(0, pm.add(p2, r3));
    CPPUNIT_ASSERT_EQUAL(2, (int) pm._generators.size());

    std::map<PatternMask, EventGenerator *>::iterator iterator;

    iterator = pm._generators.find(p1);
    CPPUNIT_ASSERT(iterator != pm._generators.end());
    CPPUNIT_ASSERT_EQUAL(false, iterator->second->isEmpty());
    CPPUNIT_ASSERT_EQUAL(2, (int) iterator->second->_receivers.size());

    iterator = pm._generators.find(p2);
    CPPUNIT_ASSERT(iterator != pm._generators.end());
    CPPUNIT_ASSERT_EQUAL(false, iterator->second->isEmpty());
    CPPUNIT_ASSERT_EQUAL(2, (int) iterator->second->_receivers.size());
}

void FF::PatternManagerTest::testRemovePattern() {
    PatternManager pm;

    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1));
    CPPUNIT_ASSERT_EQUAL(1, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(0, pm.remove(p1));
    CPPUNIT_ASSERT_EQUAL(0, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(-1, pm.remove(p1));

    p1._timeSlot = 9;

    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1));
    CPPUNIT_ASSERT_EQUAL(1, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(0, pm.remove(p1));
    CPPUNIT_ASSERT_EQUAL(0, (int) pm._generators.size());
}

void FF::PatternManagerTest::testRemovePatternReceiver() {
    PatternManager pm;
    EventReceiver r1; // Receives P1
    EventReceiver r2; // Receives P2
    EventReceiver r3; // Receives P1 and P2
    EventReceiver r4; // Receives P5 -> which also matches P1, but has different
                      // exclusion mask!

    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1, r1));
    CPPUNIT_ASSERT_EQUAL(1, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(0, pm.add(p2, r2));
    CPPUNIT_ASSERT_EQUAL(2, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(0, pm.add(p1, r3));
    CPPUNIT_ASSERT_EQUAL(0, pm.add(p2, r3));
    CPPUNIT_ASSERT_EQUAL(2, (int) pm._generators.size());
    CPPUNIT_ASSERT_EQUAL(0, pm.add(p5, r4));
    CPPUNIT_ASSERT_EQUAL(3, (int) pm._generators.size());

    std::map<PatternMask, EventGenerator *>::iterator iterator;

    iterator = pm._generators.find(p1);
    CPPUNIT_ASSERT(iterator != pm._generators.end());
    CPPUNIT_ASSERT_EQUAL(false, iterator->second->isEmpty());
    CPPUNIT_ASSERT_EQUAL(2, (int) iterator->second->_receivers.size());

    iterator = pm._generators.find(p2);
    CPPUNIT_ASSERT(iterator != pm._generators.end());
    CPPUNIT_ASSERT_EQUAL(false, iterator->second->isEmpty());
    CPPUNIT_ASSERT_EQUAL(2, (int) iterator->second->_receivers.size());

    // Try to remove a pattern/receiver combination not registered
    CPPUNIT_ASSERT_EQUAL(-1, pm.remove(p2, r1));
    CPPUNIT_ASSERT_EQUAL(-1, pm.remove(p1, r2));

    // Remove receivers of p1, starting with r3 and then r1
    CPPUNIT_ASSERT_EQUAL(0, pm.remove(p1, r3));
    iterator = pm._generators.find(p1);
    CPPUNIT_ASSERT(iterator != pm._generators.end());
    CPPUNIT_ASSERT_EQUAL(false, iterator->second->isEmpty());
    CPPUNIT_ASSERT_EQUAL(1, (int) iterator->second->_receivers.size());

    CPPUNIT_ASSERT_EQUAL(0, pm.remove(p1, r1));
    iterator = pm._generators.find(p1);
    // Pattern p1 should have been removed, only p2 is left
    CPPUNIT_ASSERT(iterator == pm._generators.end());
    CPPUNIT_ASSERT_EQUAL(2, (int) pm._generators.size());

    iterator = pm._generators.find(p2);
    CPPUNIT_ASSERT(iterator != pm._generators.end());
    CPPUNIT_ASSERT_EQUAL(false, iterator->second->isEmpty());
    CPPUNIT_ASSERT_EQUAL(2, (int) iterator->second->_receivers.size());
}

void FF::PatternManagerTest::testProcess() {
    EventReceiver r1; // Receives P1
    EventReceiver r2; // Receives P2
    EventReceiver r3; // Receives P1 and P2
    EventReceiver r4; // Receives P5 -> which also matches P1, but has different
                      // exclusion mask!

    PatternManager pm;

    // Add receivers
    pm.add(p1, r1);
    pm.add(p2, r2);
    pm.add(p1, r3);
    pm.add(p2, r3);
    pm.add(p5, r4);

    // Generate p1
    Pattern pattern1(p1);
    CPPUNIT_ASSERT_EQUAL(0, pm.process(pattern1));
    CPPUNIT_ASSERT_EQUAL(1, r1.getPendingEvents());
    CPPUNIT_ASSERT_EQUAL(1, r3.getPendingEvents());
    CPPUNIT_ASSERT_EQUAL(0, r2.getPendingEvents());
    CPPUNIT_ASSERT_EQUAL(1, r4.getPendingEvents());

    // Generate p2
    Pattern pattern2(p2);
    CPPUNIT_ASSERT_EQUAL(0, pm.process(pattern2));
    CPPUNIT_ASSERT_EQUAL(1, r1.getPendingEvents());
    CPPUNIT_ASSERT_EQUAL(2, r3.getPendingEvents());
    CPPUNIT_ASSERT_EQUAL(1, r2.getPendingEvents());

    // Make sure the events are of the type PATTERN_EVENT
    Event e;
    e._type = NO_EVENT;
    CPPUNIT_ASSERT_EQUAL(0, r1.receive(e));
    CPPUNIT_ASSERT_EQUAL(PATTERN_EVENT, e._type);
    CPPUNIT_ASSERT(pattern1 == e._pattern);
    CPPUNIT_ASSERT_EQUAL(0, r1.getPendingEvents());

    e._type = NO_EVENT;
    CPPUNIT_ASSERT_EQUAL(0, r2.receive(e));
    CPPUNIT_ASSERT_EQUAL(PATTERN_EVENT, e._type);
    CPPUNIT_ASSERT(pattern2 == e._pattern);
    CPPUNIT_ASSERT_EQUAL(0, r2.getPendingEvents());

    e._type = NO_EVENT;
    CPPUNIT_ASSERT_EQUAL(0, r3.receive(e));
    CPPUNIT_ASSERT_EQUAL(PATTERN_EVENT, e._type);
    CPPUNIT_ASSERT(pattern1 == e._pattern);
    e._type = NO_EVENT;
    CPPUNIT_ASSERT_EQUAL(0, r3.receive(e));
    CPPUNIT_ASSERT_EQUAL(PATTERN_EVENT, e._type);
    CPPUNIT_ASSERT(pattern2 == e._pattern);
    CPPUNIT_ASSERT_EQUAL(0, r3.getPendingEvents());
}

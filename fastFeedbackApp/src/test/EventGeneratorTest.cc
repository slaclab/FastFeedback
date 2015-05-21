#include "EventGeneratorTest.h"
#include "EventGenerator.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::EventGeneratorTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::EventGeneratorTest::testAddReceiver() {
    EventReceiver r1;
    EventReceiver r2;
    EventGenerator g;

    CPPUNIT_ASSERT_EQUAL(0, g.add(r1));
    CPPUNIT_ASSERT_EQUAL(-1, g.add(r1));

    CPPUNIT_ASSERT_EQUAL(0, g.add(r2));
    CPPUNIT_ASSERT_EQUAL(-1, g.add(r1));
    
    CPPUNIT_ASSERT_EQUAL(-1, g.add(r1));
    CPPUNIT_ASSERT_EQUAL(-1, g.add(r1));
}

void FF::EventGeneratorTest::testRemoveReceiver() {
    EventReceiver r1;
    EventReceiver r2;
    EventGenerator g;

    CPPUNIT_ASSERT_EQUAL(-1, g.remove(r1));
    CPPUNIT_ASSERT_EQUAL(-1, g.remove(r2));

    CPPUNIT_ASSERT_EQUAL(0, g.add(r1));
    CPPUNIT_ASSERT_EQUAL(0, g.add(r2));

    CPPUNIT_ASSERT_EQUAL(0, g.remove(r1));
    CPPUNIT_ASSERT_EQUAL(0, g.remove(r2));
}

void FF::EventGeneratorTest::testSendEvent() {
    EventReceiver r1;
    EventReceiver r2;
    EventGenerator g;

    CPPUNIT_ASSERT_EQUAL(0, g.add(r1));
    CPPUNIT_ASSERT_EQUAL(0, g.add(r2));

    Event e(QUIT_EVENT);

    CPPUNIT_ASSERT_EQUAL(0, g.send(e));
    CPPUNIT_ASSERT_EQUAL(1, r1.getPendingEvents());
    CPPUNIT_ASSERT_EQUAL(1, r2.getPendingEvents());

    Event r1Event;
    CPPUNIT_ASSERT_EQUAL(0, r1.receive(r1Event));
    CPPUNIT_ASSERT_EQUAL(QUIT_EVENT, r1Event._type);

    Event r2Event;
    CPPUNIT_ASSERT_EQUAL(0, r2.receive(r2Event));
    CPPUNIT_ASSERT_EQUAL(QUIT_EVENT, r2Event._type);

    CPPUNIT_ASSERT_EQUAL(0, g.remove(r1));
    CPPUNIT_ASSERT_EQUAL(0, g.remove(r2));
}

void FF::EventGeneratorTest::testEmpty() {
    EventGenerator g;

    CPPUNIT_ASSERT_EQUAL(true, g.isEmpty());

    EventReceiver r;
    CPPUNIT_ASSERT_EQUAL(0, g.add(r));
    CPPUNIT_ASSERT_EQUAL(false, g.isEmpty());
    CPPUNIT_ASSERT_EQUAL(0, g.remove(r));
    CPPUNIT_ASSERT_EQUAL(true, g.isEmpty());
}

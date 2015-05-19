#include "ThreadTest.h"
#include "Thread.h"
#include "Defs.h"

#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::ThreadTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::ThreadTest::testJoin() {
    Thread t("MyJoinThread");

    // Thread not started
    CPPUNIT_ASSERT_EQUAL(t.join(), -1);

    // Let Thread go through without reading from input queue
    CPPUNIT_ASSERT_EQUAL(t._done, false);
    t._done = true;
    t._thread.start();
    sleep(1);
    CPPUNIT_ASSERT_EQUAL(t.join(), 0);
}

/**
 * Test if FastFeedback::Threads do exit when receiving a QUIT_EVENT.
 * The several calls to sleep() were placed here in order to make this
 * test run on RTEMS.
 *
 * @author L.Piccoli
 */
void FF::ThreadTest::testQuitEvent() {
    Thread t("QuitTest");

    CPPUNIT_ASSERT_EQUAL(t.start(), 0);
    sleep(1);
    Event e(QUIT_EVENT);
    CPPUNIT_ASSERT_EQUAL(t._done, false);
    CPPUNIT_ASSERT_EQUAL(t.send(e), 0);
    sleep(3);
    while(t.getPendingEvents() != 0); // wait for QUIT_EVENT to be consumed
    CPPUNIT_ASSERT_EQUAL(0, t.getPendingEvents());
    sleep(3);
    while(t._done == false);
    CPPUNIT_ASSERT_EQUAL(t.join(), 0);
    CPPUNIT_ASSERT_EQUAL(t._done, true);
    CPPUNIT_ASSERT_EQUAL(0, t.getPendingEvents());
}

class TestThread : public FF::Thread {
public:
    EventType _type;

    TestThread () :
    Thread ("TestThread"), _type(NO_EVENT) {};

protected:
    virtual int processEvent(Event &event) {
        _type = event._type;
        return 0;
    }
};

/**
 * Test a subclass of FF::Thread processing events.
 * Calls to sleep() inserted for testing under RTEMS
 *
 * @author L.Piccoli
 */
void FF::ThreadTest::testProcessEvent() {
    TestThread t;

    CPPUNIT_ASSERT_EQUAL(t.start(), 0);
    sleep(1);
    Event e(PATTERN_EVENT);
    CPPUNIT_ASSERT_EQUAL(t.send(e), 0);
    sleep(1);
    while(t.getPendingEvents() != 0); // wait for PATTERN_EVENT to be consumed

    CPPUNIT_ASSERT_EQUAL(PATTERN_EVENT, t._type);

    Event quit(QUIT_EVENT);
    CPPUNIT_ASSERT_EQUAL(t._done, false);
    CPPUNIT_ASSERT_EQUAL(t.send(quit), 0);
    sleep(1);
    while(t.getPendingEvents() != 0); // wait for QUIT_EVENT to be consumed
    CPPUNIT_ASSERT_EQUAL(0, t.getPendingEvents());
    sleep(1);
    while(t._done == false);
    CPPUNIT_ASSERT_EQUAL(t.join(), 0);
    CPPUNIT_ASSERT_EQUAL(t._done, true);
    CPPUNIT_ASSERT_EQUAL(t.getPendingEvents(), 0);
}

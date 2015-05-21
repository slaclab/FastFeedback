#include "EventReceiverTest.h"
#include "EventReceiver.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::EventReceiverTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FF::EventReceiverTest::testSendReceive() {
    EventReceiver er;
    Event sendEvent(QUIT_EVENT);
    Event recvEvent;

    CPPUNIT_ASSERT_EQUAL(er.send(sendEvent), 0);
    CPPUNIT_ASSERT_EQUAL(er.receive(recvEvent), 0);

    CPPUNIT_ASSERT_EQUAL(recvEvent._type, sendEvent._type);
}

void FF::EventReceiverTest::testPending() {
    EventReceiver er;
    int pending = 11;
    Event pendingEvent(PATTERN_EVENT);

    for (int i = 0; i < pending; ++i) {
        CPPUNIT_ASSERT_EQUAL(er.send(pendingEvent), 0);
    }
    CPPUNIT_ASSERT_EQUAL(er.getPendingEvents(), pending);
}

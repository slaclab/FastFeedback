#include "TimerTest.h"
#include "Timer.h"

#include <iostream>
#include <stdlib.h>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TimerTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

        int COUNTER = 0;

void callback(void *arg) {
    COUNTER++;
}

void TimerTest::setUp() {
    COUNTER = 0;
}

void TimerTest::testCallback() {
    Timer t(&callback);

    t.start(1);
    CPPUNIT_ASSERT_EQUAL(0, COUNTER);
    epicsThreadSleep(2);
    CPPUNIT_ASSERT_EQUAL(1, COUNTER);
    t.start(1);
    epicsThreadSleep(2);
    CPPUNIT_ASSERT_EQUAL(2, COUNTER);
    t.start(2); // Start timer that won't expire
}

void TimerTest::testRepeatedCallback() {
    Timer t(&callback);

    t.start(2, true);
    CPPUNIT_ASSERT_EQUAL(0, COUNTER);
    epicsThreadSleep(7);
    t.cancel();
    CPPUNIT_ASSERT_EQUAL(3, COUNTER);
}

void callback2(void *arg);

void callback1(void *arg) {
    Timer *t = reinterpret_cast<Timer *>(arg);
    COUNTER = 10;
    t->setCallback(&callback2);
}

void callback2(void *arg) {
    Timer *t = reinterpret_cast<Timer *>(arg);
    COUNTER = 20;
    t->setCallback(&callback1);
}


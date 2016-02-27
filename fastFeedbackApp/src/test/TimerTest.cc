#include "TimerTest.h"
#include "Timer.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "Log.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::TimerTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

int COUNTER = 0;

void callback(void *arg) {
  COUNTER++;
}

void FF::TimerTest::setUp() {
  COUNTER = 0;
  Log::getInstance().setAllLogToConsole();
  Log::getInstance().setDebugLevel(Log::flagEvent, 10);
}

timespec ts2;

void callback_time(void *arg) {
  clock_gettime(CLOCK_REALTIME, &ts2);
}

void FF::TimerTest::testCallback() {
  /*
    Timer t(&callback);

    t.start();
    epicsThreadSleep(2);
    t.go(1);
    CPPUNIT_ASSERT_EQUAL(0, COUNTER);
    epicsThreadSleep(3);
    CPPUNIT_ASSERT_EQUAL(1, COUNTER);
    t.go(1);
    epicsThreadSleep(3);
    CPPUNIT_ASSERT_EQUAL(2, COUNTER);
  */
    //    t.go(); // Start timer that won't expire
}

void FF::TimerTest::testElapsedTime() {
  Timer t(&callback_time);

  t.start();
  epicsThreadSleep(2);

  timespec ts1;
  clock_gettime(CLOCK_REALTIME, &ts1);
  t.go();
  epicsThreadSleep(3);

  long nano1 = ts1.tv_nsec;
  long nano2 = ts2.tv_nsec;
  long diff = nano2 - nano1;
  std::cout << diff << std::endl;
  
  CPPUNIT_ASSERT(false);
}


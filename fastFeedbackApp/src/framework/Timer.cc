/* 
 * File:   Timer.cc
 * Author: lpiccoli
 * 
 * Created on May 27, 2010, 2:20 PM
 */

#include "Timer.h"
#include "ExecConfiguration.h"

#include <time.h>

USING_FF_NAMESPACE

/**
 * Class constructor specifying a callback invoked when the timer
 * expires.
 *
 * @param callback a "C" function that is invoked when timer expires.
 * @author L.Piccoli
 */
Timer::Timer(CallbackFunction callback) :
  Thread("Timer", 99), _callback(callback) {
  _startEvent._type = START_TIMER_EVENT;
}

/**
 * Class destructor
 *
 * @author L.Piccoli
 */
Timer::~Timer() {
  //  stop();
  exit(0);
}

/**
 * Start the timer, which runs for the specified amount of time (seconds?)
 * 
 * @param restart true if timer should be rearmed every time it expires,
 * false if this is a one time event. (Default = false)
 * @param delay time in seconds
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int Timer::go(double delay, bool restart) {
  Log::getInstance() << Log::showtime << Log::flagEvent << Log::dpInfo
		     << "INFO: Timer::go() triggering timer" << Log::dp;
  this->send(_startEvent);
  return 0;
};

/**
 */
int Timer::processEvent(Event& event) {
  unsigned int delay = ExecConfiguration::getInstance()._timerDelayPv.getValue(); // delay in usec 
  struct timespec deadline;
  deadline.tv_sec = 0;
  deadline.tv_nsec = delay * 1000; // usec -> nanosec

#ifdef DEBUG
  Log::getInstance() << Log::showtime << Log::flagEvent << Log::dpInfo
		     << "INFO: Timer::processEvent() type=" << (int) event._type
		     << ", delay=" << (int) delay << " usec" << Log::dp;
#endif

  timespec ts1, ts2;
  long nano1, nano2, diff;
  switch (event._type) {
    case START_TIMER_EVENT:
#ifdef DEBUG
      Log::getInstance() << Log::showtime << Log::flagEvent << Log::dpInfo
			 << "INFO: Timer::processEvent() starting nanosleep" << Log::dp;
      clock_gettime(CLOCK_REALTIME, &ts1);
#endif
      clock_nanosleep(CLOCK_REALTIME, 0, &deadline, NULL);
#ifdef DEBUG
      clock_gettime(CLOCK_REALTIME, &ts2);
      if (ts1.tv_sec < ts2.tv_sec) {
	nano1 = ts1.tv_nsec;
	nano2 = ts2.tv_nsec + 1000000000;
      }
      else {
	nano1 = ts1.tv_nsec;
	nano2 = ts2.tv_nsec;
      }
      diff = nano2 - nano1;
      Log::getInstance() << Log::showtime << Log::flagEvent << Log::dpInfo
			 << "INFO: Timer::processEvent() callback ns=" << diff << Log::dp;
#endif      
      callback();
      return 0;
    default:
      return -1;
  }
}


/**
 * Cancel current timer.
 *
 * @author L.Piccoli
 */
void Timer::cancel() {
  //    uint32_t timerId = 1;
  //    BSP_timer_stop(timerId);
}

void Timer::callback() {
    if (_callback != NULL) {
      _callback(this);
    }
}

void Timer::showDebug() {
}

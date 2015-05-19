/* 
 * File:   Timer.cc
 * Author: lpiccoli
 * 
 * Created on May 27, 2010, 2:20 PM
 */

#include "Timer.h"
#include "ExecConfiguration.h"

#ifdef RTEMS
#include <bsp/gt_timer.h>
#endif

#include <epicsTimer.h>

USING_FF_NAMESPACE

#ifdef LINUX
/**
 * Notification constructor, creates the epicsTimer and sets up the
 * callback function.
 *
 * @param callback a pointer to a "C" function that returns void and
 * take a void * argument
 * @author L.Piccoli
 */
Notification::Notification(CallbackFunction callback, const char* nm,
        epicsTimerQueueActive &queue) :
_callback(callback),
_timer(queue.createTimer()),
_restart(false),
_delay(60) {
}

/**
 * Notification destructor, destroyes the epicsTimer
 *
 * @author L.Piccoli
 */
Notification::~Notification() {
    _timer.destroy();
}

/**
 * Start the timer, which will expire after the specified amount of seconds
 * elapses. When the timer expires the expire() method is called.
 *
 * @param delay number of seconds to expire the timer
 * @param restart true if timer should be rearmed every time it expires,
 * false if this is a one time event. (Default = false)
 * @author L.Piccoli
 */
void Notification::start(double delay, bool restart) {
    _delay = delay;
    _restart = restart;
    _timer.start(*this, delay);
}

/**
 * Cancel current timer.
 *
 * @author L.Piccoli
 */
void Notification::cancel() {
    _timer.cancel();
}
/**
 * This method is called when the timer expires. The callback specified in
 * the Notification constructor is called at this point.
 *
 * @param currentTime time when this method was called
 * @return expireStatus either to restart the timer or not (see Epics
 * AppDevGuide)
 * @author L.Piccoli
 */

/*
 * TODO: Compiler error 'expireStatus' does not name a type. This method
 * compiles only if the body is in the header file.
 *
expireStatus Notification::expire(const epicsTime &currentTime) {
    currentTime.show(1);
    _callback(0);
    return noRestart;
}
 */
#endif

/**
 * This is the callback function for the timer configured using the EPICS
 * monitor. The argument eha has a pointer to the original Timer object,
 * through which the user callback is invoked.
 *
 * @param eha channel access monitor parameter (see Channel Access monitor)
 * @author L.Piccoli
 */
void monitorTimerCallback(struct event_handler_args eha) {
    Timer *t = reinterpret_cast<Timer *> (eha.usr);
    t->callback();
}

/**
 * Class constructor specifying a callback invoked when the timer
 * expires. Optionally a PV name can be specified, in case the name is not
 * empty ("") and "RTEMS", the timer sets up a monitor on this PV and a callback
 * is generated every time the monitored PV changes value.
 *
 * If the parameter monitoredPv is "RTEMS", then the timer uses the RTEMS
 * real time timer.
 *
 * @param callback a "C" function that is invoked when timer expires.
 * @author L.Piccoli
 */
Timer::Timer(CallbackFunction callback) :
#ifdef LINUX
_queue(epicsTimerQueueActive::allocate(true)),
_notification(callback, "EpicsTimer", _queue),
#endif
_callback(callback) {
#ifdef RTEMS
    configureRtemsTimer();
#endif
}

/**
 * Class destructor
 *
 * @author L.Piccoli
 */
Timer::~Timer() {
    //_queue.release(); <- This generates seg fault 
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
int Timer::start(double delay, bool restart) {
#ifdef RTEMS
  //    delay = 2550;
    delay = ExecConfiguration::getInstance()._timerDelayPv.getValue();
    //    Log::getInstance() << "DEBUG: Timer delay " << (int) delay << Log::flush;
    
    //    BSP_timer_start(0, (uint64_t)
    BSP_timer_start(1, (uint64_t)
            (delay * (double) (_clockFreq) * 1.E-6));
#else
    _notification.start(delay, restart);
#endif
    return 0;
};

/**
 * Cancel current timer.
 *
 * @author L.Piccoli
 */
void Timer::cancel() {
#ifdef RTEMS
  //    uint32_t timerId = 0;
    uint32_t timerId = 1;
    BSP_timer_stop(timerId);
#else
    _notification.cancel();
#endif
}

void Timer::callback() {
    if (_callback != NULL) {
        _callback(this);
    }
}

int Timer::configureRtemsTimer() {
#ifdef RTEMS
  //    int firstTimer = 0;
    int firstTimer = 1;
    int doNotReload = 0;
    BSP_timer_setup(firstTimer, _callback, 0, doNotReload);
    _clockFreq = BSP_timer_clock_get(firstTimer);
#endif
    return 0;
}

void Timer::showDebug() {
}

/* 
 * File:   Timer.h
 * Author: lpiccoli
 *
 * Created on May 27, 2010, 2:20 PM
 */

#ifndef _TIMER_H
#define	_TIMER_H

#include <iostream>

#include "Defs.h"
#include "TimeUtil.h"

#include <cadef.h>
#include <epicsTimer.h>

typedef void(*CallbackFunction)(void *arg);

FF_NAMESPACE_START

/**
 * This class is used for the epicsTimer. It defines when the timer starts
 * and has the callback invoked by the epicsTimer when time expires.
 *
 * The resolution of this timer is in seconds, so it can be used for testing
 * purposes only.
 *
 * @author L.Piccoli
 */
#ifdef LINUX
class Notification : public epicsTimerNotify {
public:
    Notification(CallbackFunction callback, const char* nm,
            epicsTimerQueueActive &queue);
    virtual ~Notification();

    void start(double delay, bool restart = false);
    void cancel();
    virtual expireStatus expire(const epicsTime &currentTime) {
        //currentTime.show(1);
        _callback(0);
        if (_restart) {
            return expireStatus(restart, _delay);
        }
        return noRestart;
    }

private:
    /** "C" function called by the expire() method */
    CallbackFunction _callback;

    /** The actual timer (resolution is in seconds!) */
    epicsTimer &_timer;

    /** Defines whether timer should be rearmed after expiration */
    bool _restart;

    /** Timer delay, this is used if a timer needs to be restarted */
    double _delay;
};
#endif

/**
 * This class provides access to timer functions using the epicsTimer C++
 * interface or the RTEMS hardware timer.
 *
 * @author L.Piccoli
 */
class Timer {
public:
    Timer(CallbackFunction callback);
    virtual ~Timer();

    int start(double delay = 1, bool restart = false);
    void cancel();
    void setCallback(CallbackFunction callback) {
        _callback = callback;
    }
    void callback();

    void showDebug();

    enum TimerType {
        EPICS_TIMER,
        RTEMS_TIMER
    };

private:
    int configureRtemsTimer();
#ifdef LINUX
    epicsTimerQueueActive &_queue;
    Notification _notification;
#endif
    CallbackFunction _callback;
    unsigned int _clockFreq;
};

FF_NAMESPACE_END

#endif	/* _TIMER_H */


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
#include "Thread.h"


#include <cadef.h>

typedef void(*CallbackFunction)(void *arg);

FF_NAMESPACE_START

/**
 * This class provides access to timer functions using the epicsTimer C++
 * interface or the RTEMS hardware timer.
 *
 * @author L.Piccoli
 */
class Timer : public Thread {
public:
    Timer(CallbackFunction callback);
    virtual ~Timer();

    int go(double delay = 1, bool restart = false);
    void cancel();
    void setCallback(CallbackFunction callback) {
        _callback = callback;
    }
    void callback();
    void showDebug();

protected:
    virtual int processEvent(Event &event);

private:
    CallbackFunction _callback;
    unsigned int _clockFreq;
    Event _startEvent;
};

FF_NAMESPACE_END

#endif	/* _TIMER_H */


/* 
 * File:   Thread.h
 * Author: lpiccoli
 *
 * Created on May 24, 2010, 9:44 AM
 */

#ifndef _THREAD_H
#define	_THREAD_H

#include <epicsThread.h>
#include <epicsMutex.h>
#include <string>

#include "Defs.h"
#include "EventReceiver.h"
#include "ChannelAccess.h"
#include "TimeUtil.h"

FF_NAMESPACE_START

class ThreadTest;

/**
 * Thread is basically an epicsThread capable of receiving events through an
 * EventReceiver.
 *
 * The run() method defines an event loop, that processes basic commands/events
 * and invokes virtual methods for tailored commands/events. For example, the
 * Thread does not know what to do when a PATTERN_EVENT is received. This
 * should be handled by a Thread subclass.
 *
 * @author L.Piccoli
 */
class Thread : public epicsThreadRunable, public EventReceiver {
public:
    Thread(const std::string name, int priority = 80);
    virtual ~Thread();

    virtual void run();
    virtual int preRun() { return 0; };
    virtual int postRun() { return 0; };
    int start();
    int join();
    void setPriority(int value);
    std::string getName() { return _name; };

    virtual void showDebug();

    friend class ThreadTest;

protected:
    virtual int processEvent(Event &event);

    /** Set to true when the Thread is to exit the run() method */
    bool _done;

    /** Count the number of events received by the thread */
    unsigned long _eventCount;

    /** Count the number of events not recognized and not processed */
    unsigned long _eventCountNotHandled;

    /** Actual EPICS thread running, created at constuction time */
    epicsThread _thread;

    /** Thread name */
    std::string _name;

private:
    /** EPICS mutex used to join the Thread
     * TODO: use a mutex that does work, current join() is based on busy
     * waiting on the _done attribute.
     */
    //epicsMutex *_join;

    /** Indicates whether Thread has started or not */
    bool _started;

    /**
     * Each thread has a ChannelAccess instance, the first one to be created
     * creates the ChannelAccess context. The following threads only attach
     * to the existing context.
     */
    ChannelAccess _channelAccess;

    /** Statistics about the time between events */
    TimeAverage _idleStats;

    /** Statistics about the time taken to process events */
    TimeAverage _processStats;
};

FF_NAMESPACE_END

#endif	/* _THREAD_H */


/* 
 * File:   EventReceiver.h
 * Author: lpiccoli
 *
 * Created on May 24, 2010, 11:39 AM
 */

#ifndef _EVENTRECEIVER_H
#define	_EVENTRECEIVER_H

#include "Defs.h"
#include "Event.h"
#include "TimeUtil.h"

#include <epicsMessageQueue.h>
#include <epicsMutex.h>

#include <pthread.h>

FF_NAMESPACE_START

const int EVENT_RECEIVER_QUEUE_SIZE = 100;

/**
 * The EventReceiver class is loosely based on the Observer Pattern with the
 * difference that instead of declaring an update() method it implements the
 * send(event)/receive(event) methods.
 *
 * When a EventGenerator generates an event, it is passed to all EventReceivers
 * through the send(event) call.
 *
 * The send() method puts the event in the message queue (attribute of
 * EventReceiver).
 *
 * The actual entity that receives the event must read the EventReceiver queue
 * by invoking the EventReceiver::receive(event) method.
 *
 * Usually the user of EventReceiver is a Thread that keeps reading
 * commands/events from an input queue.
 */
class EventReceiver {
public:
    EventReceiver();
    virtual ~EventReceiver();

    int receive(Event &event);
    int send(Event &event);
    int getPendingEvents();
    virtual void clear();
    long getDiscardedEvents();
    long getFailedSendCount() {
      return _failSendCount;
    }
    long getFailedReceiveCount() {
      return _failReceiveCount;
    }

 protected:
    epicsMessageQueue queue;

    long _discardedEvents;

private:
    /** Count how many times send() method fails */
    long _failSendCount;

    /** Count how many times receive() method fails */
    long _failReceiveCount;

    /** Mutex to control access to configuration data */
    //epicsMutex _queueMutex; DMF - taking it out for now
};

FF_NAMESPACE_END

#endif	/* _EVENTRECEIVER_H */


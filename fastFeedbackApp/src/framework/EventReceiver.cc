/* 
 * File:   EventReceiver.cc
 * Author: lpiccoli
 * 
 * Created on May 24, 2010, 11:39 AM
 */

#include "EventReceiver.h"

USING_FF_NAMESPACE

EventReceiver::EventReceiver() :
  queue(EVENT_RECEIVER_QUEUE_SIZE, sizeof (Event)),
  _discardedEvents(0),
  _failSendCount(0),
  _failReceiveCount(0) {
}

EventReceiver::~EventReceiver() {
}

/**
 * Reads an Event from the queue. This method blocks until an event is
 * received.
 *
 * @param event Place holder for the event retrieved from the queue
 * @return 0 if event has been received, -1 if there is a time out (according
 * to the EPICS AppDevGuide) or the message received is larger than an Event.
 * @author L.Piccoli
 */
int EventReceiver::receive(Event& event) {
  if (queue.receive(&event, sizeof (event))) {
    return 0;
  }
  _failReceiveCount++;
  return -1;
}

/**
 * Send an Event to the EventReceiver. A copy of the passed Event is
 * saved in the queue.
 *
 * @param event Event to be sent to EventReceiver
 * @return 0 if Event was sent, -1 queue is full
 * @author L.Piccoli
 */
int EventReceiver::send(Event &event) {
  if (queue.trySend(&event, sizeof (event)) == 0) {
    return 0;
  }
  _failSendCount++;

  return -1;
}

/**
 * Return the number of Events pending in the queue.
 *
 * @return number of Events available in the queue, ready to be consumed.
 * @author L.Piccoli
 */
int EventReceiver::getPendingEvents() {
    return queue.pending();
}

/**
 * If there are events in the input queue discard them all.
 *
 * @author L.Piccoli
 */
void EventReceiver::clear() {
  //_queueMutex.lock();
  int pendingEvents = getPendingEvents();
  Event event;
  while (pendingEvents > 0) {
    if (queue.tryReceive(&event, sizeof (event)) != -1) {
      _discardedEvents++;
      pendingEvents--;
    }
  }
  //_queueMutex.unlock();
}

long EventReceiver::getDiscardedEvents() {
  return _discardedEvents;
}


/* 
 * File:   EventGenerator.cc
 * Author: lpiccoli
 * 
 * Created on May 26, 2010, 9:31 AM
 */

#include "EventGenerator.h"

USING_FF_NAMESPACE

/**
 * Class constructor.
 *
 * @author L.Piccoli
 */
EventGenerator::EventGenerator() : _eventCount(0) {
}

/**
 * Class destructor.
 *
 * @author L.Piccoli
 */
EventGenerator::~EventGenerator() {
}

/**
 * Add an EventReceiver to the set of receivers. An EventReceiver can be added
 * only once.
 *
 * @param receiver EventReceiver to be added
 * @return 0 if receiver was added successfully, -1 if receiver was already
 * in the set of receivers
 * @author L.Piccoli
 */
int EventGenerator::add(EventReceiver &receiver) {
    std::pair < std::set<EventReceiver *>::iterator, bool> ret;
    ret = _receivers.insert(&receiver);

    if (ret.second == false) {
        return -1;
    }
    return 0;
}

/**
 * Remove the specified EventReceiver from the list of receivers.
 *
 * @param receiver EventReceiver to be removed
 * @return 0 if receiver was removed successfully, -1 if receiver was not
 * in the set of receivers
 * @author L.Piccoli
 */
int EventGenerator::remove(EventReceiver &receiver) {
    if (_receivers.erase(&receiver) != 1) {
        return -1;
    }
    return 0;
}

/**
 * Send the specified Event to all EventReceivers in the receivers list.
 *
 * @param event Event to be sent
 * @return always returns 0, even if there are no EventReceivers
 * @author L.Piccoli
 */
int EventGenerator::send(Event &event) {
    std::set<EventReceiver *>::iterator iterator;

    for (iterator = _receivers.begin(); iterator != _receivers.end(); ++iterator) {
        (*iterator)->send(event);
    }

    _eventCount++;

    return 0;
}

/**
 * Returns whether the current number of receivers is zero or not.
 *
 * @return true if there are no receivers, false if there is at least one.
 */
bool EventGenerator::isEmpty() {
    return _receivers.empty();
}

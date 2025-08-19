/* 
 * File:   Event.h
 * Author: lpiccoli
 *
 * Created on May 24, 2010, 11:13 AM
 */

#ifndef _EVENT_H
#define	_EVENT_H

#include "Defs.h"
#include "Pattern.h"

FF_NAMESPACE_START

/**
 * Definition of all types of events in the Fast Feedback system.
 *
 * @author L.Piccoli
 */
enum EventType {
    NO_EVENT = 0,
    PATTERN_EVENT,
    MEASUREMENT_EVENT,
    QUIT_EVENT,
    RECONFIGURE_EVENT,
    DISCONNECT_EVENT,
    MODE_EVENT,
    HEARTBEAT_EVENT,
    START_TIMER_EVENT,
};

/**
 * An Event represents a message sent between entities (e.g. ISR -> thread)
 * that normally is a command.
 *
 * Events have a type (see EventType enumeration) and a payload, which contains
 * information specific to the command.
 *
 * An Event also carries pattern information (with includes an epicsTimeStamp).
 * Pattern information is included because the majority of Events in the system
 * are tied to a pattern.
 * 
 * TODO: Define an efficient way to new/delete Event structs, possible use
 * of a memory pool or a list of Events created at startup time.
 *
 * TODO: Make sure a void* is appropriate for the payload, consider using
 * unions. Who is going to alloc/dealloc the payload?
 *
 * TODO: Fill out _timestamp field properly.
 *
 * @author L.Piccoli
 */
typedef struct Event {
    /** Type of event, as described in the EventType enum */
    EventType _type;

    /** Pointer to additional external Event information */
    void *_payload;

    /** Pattern information */
    Pattern _pattern;

    /** 
     * Default struct constructor.
     */
    Event() :
    _type(NO_EVENT), _payload(0) {
    }

    /**
     * Constructor specifying an EventType
     */
    Event(EventType type) :
    _type(type), _payload(0) {
    }

    /**
     * Copy constructor
     */
    Event(const Event & other) {
        _type = other._type;
        _payload = other._payload;
        _pattern = other._pattern;
    }
} Event;

FF_NAMESPACE_END

#endif	/* _EVENT_H */


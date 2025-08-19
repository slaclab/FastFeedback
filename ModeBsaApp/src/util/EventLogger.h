/*
 * File:   EventLogger.h
 * Author: lpiccoli
 *
 * Created on April 10, 2012, 10:57 AM
 */

#ifndef _EVENTLOGGER_H
#define	_EVENTLOGGER_H

#include <string>
#include "Defs.h"
#include "Event.h"
#include <epicsTime.h>
#include <epicsMutex.h>

FF_NAMESPACE_START

enum EventOperation {
  NONE = 0,
  RECEIVED,
  PROCESSED,
  DISCARDED,
};

typedef struct EventLogEntry {
  EventType _event;
  char _operation; // Event received (1), Event processed (2)
  char _loop[4];
  // For Event received operation this has no meaning, for Event processed this holds the
  char _result;
  epicsTimeStamp _time;
} EventLogEntry;


/**
 *
 * @author L.Piccoli
 */
class EventLogger {
public:
  EventLogger(int samples = 10000);
  ~EventLogger();
  
    void add(EventType event, EventOperation operation, char loop[4], char result);
    void add(EventType event, EventOperation operation, std::string loop, char result);
    void setSamples(int samples);
    void dump();

    friend class EventLoggerTest;
    
    static EventLogger &getInstance();

private:
    /**
     * Access to the EventLogger must pass through the mutex, for both 
     * adding events and printing tem out
     */
    epicsMutex _mutex;

    /** Buffer that holds a number of event log entries */
    EventLogEntry *_buffer;

    /** Buffer size */
    int _size;

    /** Points to the next element to receive the measurement */
    int _head;

    /** Points to the last element that received a measurement */
    int _tail;

    bool _full;

    char _auxString[4];
};

FF_NAMESPACE_END

#endif

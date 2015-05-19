/*
 * File:   EventLogger.cc
 * Author: lpiccoli
 *
 * Created on April 10, 2012, 12:36 PM
 */

#include <iostream>
#include <cmath>

#include "EventLogger.h"

USING_FF_NAMESPACE

/**
 * EventLogger singleton initialization
 */
EventLogger EventLogger::_instance;

#define CIRCULAR_INCREMENT(X) (X + 1) % _size
#define CIRCULAR_DECREMENT(X) (_size + X - 1) % _size

EventLogger::EventLogger(int samples) :
_size(samples),
_head(0),
_tail(0),
_full(false) {
    _buffer = new EventLogEntry[samples];
    for (int i = 0; i < samples; ++i) {
        _buffer[i]._operation = 0;
    }
}

EventLogger::~EventLogger() {
    delete [] _buffer;
}
void EventLogger::add(EventType event, EventOperation operation, std::string loop, char result) {
  strncpy(_auxString, loop.c_str(), 4);
  add(event, operation, _auxString, result);
}

void EventLogger::add(EventType event, EventOperation operation, char loop[4], char result) {
  _mutex.lock();
  _head = CIRCULAR_INCREMENT(_head);
  if (_head == _tail) {
    _full = true;
    _tail = CIRCULAR_INCREMENT(_tail);
  }

  _buffer[_head]._event = event;
  _buffer[_head]._operation = operation;
  _buffer[_head]._result = result;
  epicsTimeGetCurrent(&_buffer[_head]._time);
  strncpy(_buffer[_head]._loop, loop, 4);

  if (_buffer[_head]._operation != 0) {
    char timeString[256];
    char loopString[5];
    strncpy(loopString, _buffer[_head]._loop, 4);
    loopString[4] = '\0';
    epicsTimeToStrftime(timeString, sizeof(timeString), "%Y/%m/%d %H:%M:%S.%06f", &_buffer[_head]._time);
    /*
    std::cout << timeString
	      << ";" << loopString
	      << ";" << _buffer[_head]._event
	      << ";" << (int) _buffer[_head]._operation
	      << ";" << (int) _buffer[_head]._result << std::endl;
    */
	//      printf("%s;%s;%d;%d;%d\n", timeString, loopString, _buffer[index]._event,
	//	     _buffer[index]._operation, _buffer[index]._result);
  }

  _mutex.unlock();
}

/**
 * Print the events currently in the buffer.
 * 
 * This method uses printf because RTEMS does not redirect std::cout!
 */
void EventLogger::dump() {
  _mutex.lock();
  char timeString[256];
  char loopString[5];

  int index = _head; // with the latest event
  bool done = false;
  while (!done) {
    if (_buffer[index]._operation != 0) {
      strncpy(loopString, _buffer[index]._loop, 4);
      loopString[4] = '\0';
      epicsTimeToStrftime(timeString, sizeof(timeString), "%Y/%m/%d %H:%M:%S.%06f", &_buffer[index]._time);
      
      /*
	std::cout << timeString
	<< ";" << loopString
	<< ";" << _buffer[index]._event
	<< ";" << (int) _buffer[index]._operation
	<< ";" << (int) _buffer[index]._result << std::endl;
      */
      
      printf("%s;%s;%d;%d;%d\n", timeString, loopString, _buffer[index]._event,
	     _buffer[index]._operation, _buffer[index]._result);
    }
    index = CIRCULAR_DECREMENT(index);
    if (!_full) {
      if (index == 0) {
	done = true;
      }
    }
    else {
      if (CIRCULAR_INCREMENT(index) == _tail) {
	done = true;
      }
    }
  }

  _mutex.unlock();
}

void EventLogger::setSamples(int samples) {
  _mutex.lock();
  _size = samples;
  _head = 0;
  _tail = 0;
  _full = false;
  delete [] _buffer;
  _buffer = new EventLogEntry[samples];
  for (int i = 0; i < samples; ++i) {
    _buffer[i]._operation = 0;
  }
  _mutex.unlock();
}

/**
 * Return the EventLogger singleton
 *
 * @return the EventLogger
 * @author L.Piccoli
 */
EventLogger &EventLogger::getInstance() {
    return _instance;
}

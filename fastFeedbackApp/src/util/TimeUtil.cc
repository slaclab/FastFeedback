/*
 * File:   Time.cc
 * Author: lpiccoli
 *
 * Created on September 10, 2010, 11:31 AM
 */

#include <iostream>
#include <cmath>

#include "TimeUtil.h"

USING_FF_NAMESPACE

/**
 * Default constructor, which sets the itself to the current time.
 * 
 * @author L.Piccoli
 */
Time::Time() {
    // now();
}

/**
 * Copy constructor.
 *
 * @param other Time to be copied from.
 * @author L.Piccoli
 */
Time::Time(const Time& other) {
    _time = other._time;
}

/**
 * Adds the specified time to this time and return the result as Time.
 *
 * @param other Time to be added
 * @return result of adding this Time to the other Time
 * @author L.Piccoli
 */
Time Time::operator+(Time other) {
    Time temp;

    if ((_time.nsec + other._time.nsec) > 1000000000) {
        temp._time.secPastEpoch = _time.secPastEpoch + other._time.secPastEpoch + 1;
        temp._time.nsec = _time.nsec + other._time.nsec - 1000000000;
    } else {
        temp._time.secPastEpoch = _time.secPastEpoch + other._time.secPastEpoch;
        temp._time.nsec = _time.nsec + other._time.nsec;
    }
    return temp;
}

/**
 * Calculates the difference in nanoseconds between the specified Time and
 * this Time.
 *
 * This is used to measure elapsed time, so the parameter other is the
 * end time. This time is the start time.
 *
 * TODO: review this code!!!
 *
 * @param other usually the measurement end Time.
 * @return difference between this Time and the other Time
 * @author L.Piccoli
 */
Time Time::operator-(Time other) {
    Time temp;

    epicsUInt32 secDiff = 0;

    secDiff = _time.secPastEpoch - other._time.secPastEpoch;
    if (other._time.nsec < _time.nsec) {
        temp._time.nsec = _time.nsec - other._time.nsec;
    } else {
        secDiff--;
        temp._time.nsec = 1000000000 + _time.nsec - other._time.nsec;
    }
    temp._time.secPastEpoch = secDiff;
    return temp;

    /*

        if (other._time.secPastEpoch > _time.secPastEpoch) {
            secDiff = other._time.secPastEpoch - _time.secPastEpoch;
        } else {
            secDiff = _time.secPastEpoch - other._time.secPastEpoch;
        }

        if (secDiff == 0) {
            if (other._time.nsec > _time.nsec) {
                temp._time.nsec = other._time.nsec - _time.nsec;
            } else {
                temp._time.nsec = _time.nsec - other._time.nsec;
            }
        } else {


            if ((other._time.nsec - _time.nsec) < 0) {
                secDiff--;
                temp._time.nsec = 1000000000 + other._time.nsec - _time.nsec;
            } else {
                temp._time.nsec = other._time.nsec - _time.nsec;
            }
        }
        temp._time.secPastEpoch = secDiff;
     */
    //    return temp;
}

long Time::toMillis() {
    long millis = _time.secPastEpoch * 1000;
    millis += _time.nsec * 1e-6;
    return millis;
}

long Time::toMicros() {
    long micros = _time.secPastEpoch * 1000000;
    micros += _time.nsec * 1e-3;
    return micros;
}

/**
 * Set the time to the current time.
 *
 * @author L.Piccoli
 */
void Time::now() {
    epicsTimeGetCurrent(&_time);
}

void Time::show() {
    std::cout << _time.secPastEpoch << ":" << _time.nsec << std::endl;
}

#define CIRCULAR_INCREMENT(X) (X + 1) % _size

TimeAverage::TimeAverage(int samples, std::string name) :
_name(name),
_size(samples),
_head(0),
_tail(0),
_maxTime(0),
_minTime(0xfffffff),
_averageTime(0),
_sum(0),
_elapsedTime(0),
_started(false),
_falseStartCount(0),
_sampleCount(0),
_endFailedCount(0),
_aboveAverageCount(0),
_full(false) {
    _buffer = new long [samples];
    for (int i = 0; i < samples; ++i) {
        _buffer[i] = 0;
    }
}

TimeAverage::~TimeAverage() {
    delete [] _buffer;
}

void TimeAverage::start() {
    if (_started) {
        _falseStartCount++;
    }

    _start.now();
    _started = true;
}

void TimeAverage::restart() {
    _start.now();
    _started = true;
}

long TimeAverage::end() {
    if (_started) {
        Time end;
        end.now();
        Time difference;
        difference = end - _start;
        _elapsedTime = difference.toMicros();

        update();
        _started = false;
	return _elapsedTime;
    }
    else {
      _endFailedCount++;
    }
    return -1;
}

void TimeAverage::update() {
    if (_elapsedTime > _maxTime) {
        _maxTime = _elapsedTime;
    }

    if (_elapsedTime < _minTime) {
        _minTime = _elapsedTime;
    }

    _sum -= _buffer[_tail];

    _head = CIRCULAR_INCREMENT(_head);
    if (_head == _tail) {
        _full = true;
        _tail = CIRCULAR_INCREMENT(_tail);
    }

    _buffer[_head] = _elapsedTime;
    _sum += _buffer[_head];

    _sampleCount++;

    if (_full) {
      long average = getAverage();
      if (_elapsedTime > 5 * average) {
	_aboveAverageCount++;
      }
    }
}

long TimeAverage::getMax() {
    return _maxTime;
}

long TimeAverage::getMin() {
    return _minTime;
}

long TimeAverage::getAverage() {
    if (_full) {
        _averageTime = _sum / _size;
    } else {
      _averageTime = 0;
      if (_head != 0) {
        _averageTime = _sum / _head;
      }
    }
    return _averageTime;
}

long TimeAverage::getRate() {
    if (_full) {
        _averageTime = _sum / _size;
    } else {
        _averageTime = _sum / _head;
    }
    return std::floor(1000000/_averageTime);
}

void TimeAverage::show(std::string leadingSpaces) {
    std::cout << leadingSpaces << "--- " << _name << " ---" << std::endl;
    std::cout << leadingSpaces << "  average: " << getAverage() << " usec (samples="
            << _size << ", samples=" << _sampleCount << ", fs="
	      << _falseStartCount << ", end fail=" << _endFailedCount << ")" << std::endl;
    std::cout << leadingSpaces << "  max:     " << _maxTime << " usec (above average: "
	      << _aboveAverageCount << ")" << std::endl;
    std::cout << leadingSpaces << "  min:     " << _minTime << " usec" << std::endl;
}

void TimeAverage::clear() {
    _head = 0;
    _tail = 0;
    _maxTime = 0;
    _minTime = 0xfffffff;
    _averageTime = 0;
    _sum = 0;
    _elapsedTime = 0;
    _started = false;
    _sampleCount = 0;
    _endFailedCount = 0;
    _full = false;
    for (int i = 0; i < _size; ++i) {
        _buffer[i] = 0;
    }
}


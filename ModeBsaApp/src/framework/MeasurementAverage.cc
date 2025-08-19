/* 
 * File:   MeasurementAverage.cc
 * Author: lpiccoli
 * 
 * Created on November 30, 2010, 10:44 AM
 */

#include <cmath>
#include "MeasurementAverage.h"

#define CIRCULAR_INCREMENT(X) (X + 1) % _size

MeasurementAverage::MeasurementAverage(int samples, std::string name) :
_name(name),
_size(samples),
_head(0),
_tail(0),
_sum(0),
_average(0),
_full(false) {
    _buffer = new double [samples];
    for (int i = 0; i < samples; ++i) {
        _buffer[i] = 0;
    }
}

MeasurementAverage::~MeasurementAverage() {
    delete [] _buffer;
}

void MeasurementAverage::add(double measurement) {
  if (!std::isnan(measurement)) {
    if (_full) {
        _sum -= _buffer[_tail];
    }

    _head = CIRCULAR_INCREMENT(_head);
    if (_head == _tail) {
        _full = true;
        _tail = CIRCULAR_INCREMENT(_tail);
    }

    _buffer[_head] = measurement;
    _sum += _buffer[_head];
  }
}

double MeasurementAverage::getAverage() {
    if (_full) {
        _average = _sum / _size;
    } else {
        _average = _sum / _head;
    }
    return _average;
}

double MeasurementAverage::getSum() {
    return _sum;
}

void MeasurementAverage::clear() {
    _head = 0;
    _tail = 0;
    _sum = 0;
    _average = 0;
    _full = false;
}

double MeasurementAverage::getLatestValue() {
    return _buffer[_head];
}

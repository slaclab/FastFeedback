/* 
 * File:   DeviceView.cc
 * Author: lpiccoli
 * 
 * Created on June 3, 2010, 5:05 PM
 */

#include <iostream>
#include "DeviceView.h"
#include "Log.h"

USING_FF_NAMESPACE

/**
 * DeviceView constructor
 *
 * @author L.Piccoli
 */
DeviceView::DeviceView(int bufferSize, int index, char type, std::string suffix,
		       std::string loopName, int patternIndex) :
_index(index), _type(type), _suffix(suffix), _next(0), _slowNext(0),
_bufferSize(bufferSize), _loopIndex(0), _loopName(loopName),
_patternIndex(patternIndex) {
    _buffer = new double[_bufferSize];
    _slowBuffer = new double[_bufferSize];
}

/**
 * DeviceView destructor, deallocate buffers
 *
 * @author L.Piccoli
 */
DeviceView::~DeviceView() {
    delete [] _buffer;
    delete [] _slowBuffer;
}

/**
 * Add the specified device to the list of 'viewed' devices. This method does
 * *not* check if device has already been added, does *not* check whether added
 * devices are of the same type (e.g. mix of ActuatorDevices and
 * MeasurementDevices) and also does *not* verify if added devices have the same
 * pattern.
 *
 * @param device new device to be added to the list of viewed devices
 * @return always 0
 * @author L.Piccoli
 */
int DeviceView::add(Device *device) {
    _devices.push_back(device);
    return 0;
}

/**
 * Update the DeviceView buffer with the latest available data from the viewed
 * Devices. Only data from those devices contaning the specified pattern are
 * retrieved.
 * 
 * In normal operation the update() should get data from only one Device,
 * however the code does *not* prevent Devices with the same pattern be watched
 * by the same DeviceView.
 *
 * @param pattern determines from which devices the view should get new data
 * @return always 0
 * @author L.Piccoli
 */
int DeviceView::update(Pattern evrPattern) {
    std::vector<Device *>::iterator iterator;
    
    for (iterator = _devices.begin(); iterator < _devices.end(); ++iterator) {
      /* This was the code running on RTEMS, not sure why. Removed while
	 fixing the unit test 
      if (iterator != _devices.begin()) {// && !evrPattern.hasBeam()) {
	return 0;
      }
      */
      PatternMask m = (*iterator)->getPatternMask();
      if (evrPattern.match(m)) {
	_buffer[_next] = (*iterator)->peek();
	_next = CIRCULAR_INCREMENT(_next);
      }
    }
    return 0;
}

/**
 * This method must be called every second in order to keep a 1 Hz device
 * value history. When updateSlow() is called the most recent value from the
 * history buffer is copied into the slow buffer.
 *
 * @author L.Piccoli
 */
int DeviceView::updateSlow() {
    _slowBuffer[_slowNext] = _buffer[CIRCULAR_DECREMENT(_next)];
    _slowNext = CIRCULAR_INCREMENT(_slowNext);
    return -1;
}

/**
 * Return the latest value added to the internal buffer using the update()
 * call.
 * 
 * @return last device value
 * @author L.Piccoli
 */
double DeviceView::get() {
    return _buffer[CIRCULAR_DECREMENT(_next)];
}

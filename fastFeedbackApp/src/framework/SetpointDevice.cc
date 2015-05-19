/* 
 * File:   SetpointDevice.cc
 * Author: lpiccoli
 * 
 * Created on June 1, 2010, 4:07 PM
 */

#include <iostream>
#include <string.h>
#include "Pattern.h"
#include "SetpointDevice.h"
#include "Log.h"

USING_FF_NAMESPACE

/**
 * SetpointDevice constructor, passing parameters to the Device constructor.
 *
 * @author L.Piccoli
 */
SetpointDevice::SetpointDevice(std::string loopName, std::string name,
        int bufferSize, PatternMask patternMask, int patternIndex) :
Device(loopName, name, 0, patternMask, patternIndex),
_setpointPv(loopName + " " + name, 0) {
}

/**
 * SetpointDeviceDestructor
 *
 * @author L.Piccoli
 */
SetpointDevice::~SetpointDevice() {
}

/**
 * Read the current setpoint.
 *
 * @param value new measurement
 * @param timestamp timestamp of the new measurement
 * @return 0 on success, -1 if there are no new values to be returned
 * @author L.Piccoli
 */
int SetpointDevice::get(double &value, epicsTimeStamp &timestamp) {
    value = peek();
    return 0;
}

/**
 * Returns the next value available for reading. This method behaves differently
 * that get(). Peek() returns the same Setpoint if called in sequence, while
 * get() returns Setpoints read from the communication channel once and moves
 * to the next one, such that consecutive calls to get() to not return the same
 * value.
 *
 * CHANGE: Always use the last value read in, do not use _nextRead
 *
 * @return latest Setpoint available
 * @author L.Piccoli
 */
double SetpointDevice::peek() {
  return _setpointPv.getValue();
}

int SetpointDevice::peek(DataPoint &dataPoint) {
  dataPoint._value = _setpointPv.getValue();
  return 0;
}

int SetpointDevice::peek(DataPoint &dataPoint, int pos) {
  dataPoint._value = _setpointPv.getValue();
  return 0;
}

void SetpointDevice::show() {
    Device::show();
    std::cout << "[Value= " << peek() << "] ";
}

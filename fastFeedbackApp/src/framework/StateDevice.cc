/* 
 * File:   StateDevice.cc
 * Author: lpiccoli
 * 
 * Created on June 2, 2010, 1:42 PM
 */

#include "StateDevice.h"
#include "Log.h"
#include "ExecConfiguration.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "BsaApi.h"

USING_FF_NAMESPACE

/**
 * StateDevice constructor specifying the name and buffer size
 *
 * @param name device name
 * @param bufferSize number of state values kept in internal buffer
 * @author L.Piccoli
 */
StateDevice::StateDevice(std::string loopName, std::string name, int bufferSize,
        PatternMask patternMask, int patternIndex) :
ActuatorDevice(loopName, name, bufferSize, patternMask, patternIndex),
_setpoint(0),
_error(0),
_offset(0),
_statesChannel(NULL),
_stateIndex(1) {
    //_usedByLoopPv(loopName + " " + name + "USEDBYLOOP", true) {
    _caModePv = false;
    _setFbckPv = false; // States do not have FBCK PV!

    std::ostringstream strstream;
    strstream << loopName << " " << name << "OFFSET" << patternIndex;
    _offsetPv = new PvData<double>(strstream.str(), 0);
	
	_bsaStateChannel = BSA_CreateChannel(("STATE"+name.substr(1)).data());
}

/**
 * StateDevice deconstructor to release a BsaChannel.
 *
 * @author K.Wessel
 */
 
StateDevice::~StateDevice() {
	BSA_ReleaseChannel(_bsaStateChannel);
}

/**
 * Save the specified value and timestamp as a DataPoint waiting to be written.
 *
 * @param value new actuator value
 * @param timestamp for the specified value
 * @return 0 if value is between _loloLimit and _hihiLimit, -1 if it is outside
 * the limits
 * @author L.Piccoli
 */
int StateDevice::set(double value, epicsTimeStamp timestamp) {
    // Update the state error - difference between value and setpoint
    if (_setpoint != 0) {
        _error = value - _setpoint->peek();
    } else {
        _error = 0;
    }

    return ActuatorDevice::set(value, timestamp);
}

/**
 * Save the specified value in the internal buffer. Value is not written out
 * using the communication channel until write() is called.
 *
 * When set() is called, the difference between the value and the setpoint is
 * saved in the _error attribute.
 *
 * @author L.Piccoli
 */
int StateDevice::set(double value) {
    // Update the state error - difference between value and setpoint
    if (_setpoint != 0) {
        _error = value - _setpoint->peek();
    } else {
        _error = 0;
    }

    return ActuatorDevice::set(value);
}

/**
 * Write out the next value using the communication channel. The actual value
 * written is added to the existing _offset.
 *
 * UPDATE: Actually, the states do not have an associated PV as Measurements or
 * Actuators have (e.g. XCOR:IN20:551:BDES). The write() method should do nothing
 * more than just update the status. However, to maintain the same status set
 * by the ActuatorDevice::write() the communicationChannel of the StateDevice
 * should be set to a NullChannel (see Loop::reconfigure() and
 * Device::configure())
 *
 * @return 0 on success, -1 if there are no values to be written or
 * there has been a problem with the communication channel
 * @author L.Piccoli
 */
int StateDevice::write(epicsTimeStamp timestamp) {
  _lastValueSet = _buffer[_nextWrite]._value + _offsetPv->getValue();
  BSA_StoreData(_bsaStateChannel, timestamp, _lastValueSet, epicsAlarmNone, epicsSevNone); //BsaChannel, epicsTimeStamp, double, BsaStat, BsaSevr
#ifdef DEV_FCOM
  if (ExecConfiguration::getInstance()._forceDataPv.getValue()) {
      if (_statesChannel != NULL) {
          float value = 0;
          _statesChannel->write(value, _stateIndex);
          return 0;
      }
      else {
          Log::getInstance() << Log::showtime << "NULL statesChannel. Check bcastStates PV" << Log::cout;
      }
  }
#endif

    // Write value to the outgoing blob
    if (_statesChannel != NULL) {
        float value = _lastValueSet;
        _statesChannel->write(value, _stateIndex);
        }
        //_statesChannel->write(value, _stateIndex);
        return ActuatorDevice::write(_buffer[_nextWrite]._value + _offsetPv->getValue());
}

int StateDevice::writeFcom(epicsTimeStamp timestamp) {
    // Send the blob
    if (_statesChannel != NULL) {
      _statesChannel->write(timestamp);
    }

    return 0;
}

/**
 * Return last calculated error, i.e. difference between last value set() and
 * the last setpoint available.
 *
 * @return current error
 * @author L.Piccoli
 */
double StateDevice::getError() {
    return _error;
}

/**
 * Return the current offset, which is added to the value set() when it is
 * written out using the communication channel.
 *
 * @return state offset
 * @author L.Piccoli
 */
double StateDevice::getOffset() {
    //    return _offset;
    return _offsetPv->getValue();
}

/**
 * Change the state offset to the specified value.
 *
 * @param offset new state offset
 * @author L.Piccoli
 */
void StateDevice::setOffset(double offset) {
    *_offsetPv = offset;
}

/**
 * Specify the setpoint device that should be used by this StateDevice.
 *
 * @param setpoint new SetpointDevice that contains the setpoints for the
 * state
 * @author L.Piccolixb
 */
void StateDevice::setSetpointDevice(SetpointDevice *setpoint) {
    _setpoint = setpoint;
}

/**
 * Override ActuatorDevice::getInitialSetting() method. There is no initial
 * setting for StateDevices.
 *
 * @return always 0
 * @author L.Piccoli
 */
int StateDevice::getInitialSetting() {
    return 0;
}

/**
 * Returns the current value of the setpoint for this device.
 * 
 * @return current setpoint value or ZERO if the setpoind device
 * has not been assigned (NULL).
 * @author L.Piccoli
 */
double StateDevice::getSetpoint() {
    if (_setpoint != NULL) {
      return _setpoint->peek();
    } else {
        return 0;
    }
}

void StateDevice::show() {
    Device::show();
    if (_setpoint != NULL) {
        std::cout << "[" << _setpoint->getName() << "=" << _setpoint->peek()
                << ", offset=" << _offsetPv->getValue();
        if (_usedByLoopPv == true) {
            std::cout << ", used=true";
        }
        std::cout << "]";
    }
}

/**
 * Defines a unique Fcom device name for all states in the IOC. The name has to
 * include the IOC name, which is available in the $(LOCA_NAME) environment
 * variable set in the startup script. E.g. FBCK:FB01:TR01:STATES.
 * 
 * The ActuatorDevice::configure() method is bypassed because states do not need to
 * get an initial value. The Device::configure() is called directly.
 *
 * In Device::configure() the Fcom device name is built based on the _devNamePv 
 * variable ($(LOOP):SxxNAME PV). In order to have the correct name used by Fcom 
 * to get a unique id accross all states in this IOC we have to temporarily assign 
 * a name to the _devNamePv. The actual SxxNAME PV value is restored only the
 * Device::configure() method returns.
 *
 * The Fcom blob carrying the states has a specific slot for each state. The
 * slot is assigned based on the state index, which is taken from the FBCK PV
 * name: e.g. FBCK:FB01:TR01:S1 -> index 1, FBCK:FB01:TR01:S5 -> index 5.
 * 
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int StateDevice::configure(FastFeedback::CommunicationChannel::AccessType accessType,
        int patternIndex) {
    int status = 0;

    Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
		       << "INFO: StateDevice::configure()" << Log::dp;

    if (isFile()) {
        status = Device::configure(accessType, patternIndex);
    }

    // Discards the 'S' in front of the name
    std::istringstream stateStr(_name.substr(1));
    stateStr >> _stateIndex;

    LoopConfiguration *loopConfig = ExecConfiguration::getInstance().getLoopConfiguration(_slotName);
    if (loopConfig != NULL) {
        _statesChannel = loopConfig->_statesFcomChannel;
        if (_statesChannel == NULL &&
	    loopConfig->_bcastStatesPv == 1) {
            throw Exception("Failed to connect to FCOM to send state information");
        }
    }
    else {
        std::string message = "Failed to find LoopConfiguration";
        throw Exception(message + " (" + _slotName + ")");
    }

    return status;
}

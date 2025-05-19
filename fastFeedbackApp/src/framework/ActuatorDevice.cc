/* 
 * File:   ActuatorDevice.cc
 * Author: lpiccoli
 * 
 * Created on June 1, 2010, 2:02 PM
 */

#include "ActuatorDevice.h"
#include "FileChannel.h"
#include "ExecConfiguration.h"
#include "Log.h"
#include <string.h>
#include <sstream>
#include <cmath>
#include <sched.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

USING_FF_NAMESPACE

/**
 * ActuatorDevice constructor, passing parameters to the Device constructor.
 *
 * @author L.Piccoli
 */
ActuatorDevice::ActuatorDevice(std::string loopName, std::string name,
        int bufferSize, PatternMask patternMask, int patternIndex) :
Device(loopName, name, bufferSize, patternMask, patternIndex),
_nextWrite(0),
_droppedPoints(0),
_referenceActuator(NULL),
_initialChannel(NULL),
_referenceOffset(0),
#ifdef DEV_FCOM
_forcedValPv(loopName + " " + name + "FORCEDVAL"),
#endif
_hihiInPv(loopName + " " + name + "HIHIIN", 10),
_highInPv(loopName + " " + name + "HIGHIN", 1),
_lowInPv(loopName + " " + name + "LOWIN", -1),
_loloInPv(loopName + " " + name + "LOLOIN", -10) {
#ifdef CONTROL_ACTUATORS	      
  _setFbckPv = true;
#ifdef NO_FBCK_PV
  _setFbckPv = false;
#endif
#else
  _setFbckPv = false;
#endif
  //  _setFbckPv = true; // DEBUG ONLY DEBUG ONLY -> check Device::setFeedbackPv() too!!!!
}

/**
 * ActuatorDeviceDestructor
 *
 * @author L.Piccoli
 */
ActuatorDevice::~ActuatorDevice() {
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
int ActuatorDevice::set(double value, epicsTimeStamp timestamp) {
    int returnValue = set(value);

    // set(value) does not fill in the timestamp field
    // that's why we set the timestamp of the previous element of _next
    _buffer[CIRCULAR_DECREMENT(_next)]._timestamp = timestamp;

#ifdef DEBUG_PRINT
        if (_name == "A3") {
	  Log::getInstance() << Log::flagPulseId << Log::dpInfo
			     << "ActuatorDevice::get() A3 only, _next="
			     << (int) _next << ", _nextWrite=" << (int) _nextWrite
			     << " PULSEID=" << (int) PULSEID(timestamp) << Log::dp;
	}
#endif

    return returnValue;
}

int ActuatorDevice::set(double value, DataPoint::Status status) {
    int returnValue = set(value);

    // set(value) changes the status to DataPoint::SET
    // we want the status to be different
    _buffer[CIRCULAR_DECREMENT(_next)]._status = status;

    return returnValue;
}

/**
 * This method should be called for all Actuators by the Loop class if at
 * least one Actuator has a new value beyond the alarm thresholds. If the
 * last value calculated for this Actuator has status of other then SET then
 * it is changed to SET_GROUP_LIMIT.
 *
 * The last value written to the Actuator is copied back into the buffer,
 * replacing the last set value, which is not written to the Actuator.
 * 
 * This assignment is necessary in order to keep the last written value into
 * the Actuator plots.
 *
 * @author L.Piccoli
 */
void ActuatorDevice::setGroupLimit() {
    _buffer[CIRCULAR_DECREMENT(_next)]._status = DataPoint::SET_GROUP_LIMIT;
    _buffer[CIRCULAR_DECREMENT(_next)]._value = _lastValueSet;
}

/**
 * Save the specified value as a DataPoint waiting to be written.
 *
 * @param value new actuator value
 * @return 0 if value is between _loloLimit and _hihiLimit, -1 if it is outside
 * the limits
 * @author L.Piccoli
 */
int ActuatorDevice::set(double value) {
   if (_buffer[_next]._status == DataPoint::SET) {
        _droppedPoints++;
        _nextWrite = CIRCULAR_INCREMENT(_nextWrite);
    }
    _buffer[_next]._value = value;
    _buffer[_next]._status = DataPoint::SET;

    _next = CIRCULAR_INCREMENT(_next);

    if (value < _loloPv.getValue() || _hihiPv.getValue() < value) {
        _buffer[CIRCULAR_DECREMENT(_next)]._status = DataPoint::SET_LIMIT;
        return -1;
    }

    return 0;
}

/**
 * Send out a warning about FCOM not being enabled by the feedback - this
 * will tell OPS that there may be something wrong with the Feedback release!
 */
/*
void ActuatorDevice::warnAboutFcom() {
  Log::getInstance() << Log::showtime << "Feedback not controlling actuators "
		     << "- FCOM DISABLED (contact L.Piccoli NOW!)." << Log::flushpv;
}
*/

/**
 * Write the next value to the actuator using the communication channel.
 *
 * @param send if true (default) then the value is written out using the
 * communication channel and status is set to DataPoint::WRITE, otherwise
 * the status is set to DataPoint::WRITE_SKIP
 * @return 0 on success, -1 if there are no values to be written or
 * there has been a problem with the communication channel
 * @author L.Piccoli
 */
int ActuatorDevice::write(bool send) {
    int res = -1;

    // The next value to be written is always the last value set
    // Previously the _nextWrite would have the next value to be written,
    // but it did not make sense to update actuators with old data.
    int writeIndex = CIRCULAR_DECREMENT(_next);

    if (_buffer[writeIndex]._status == DataPoint::SET) {
        if (send) {

            // Save the last data sent to actuator
            _lastValueSet = _buffer[writeIndex]._value;
            // ... and send it!
            if (_communicationChannel != NULL && !std::isnan(_lastValueSet)) {
#ifdef CONTROL_ACTUATORS	      
	      res = _communicationChannel->write(_lastValueSet, _buffer[writeIndex]._timestamp);
#else 
	      res = 0;
#endif
                // Save offset relative to the reference actuator
                if (_referenceActuator != NULL) {
                    _referenceOffset = _referenceActuator->getLastValueSet() - _lastValueSet;
		    Log::getInstance() << Log::flagOffset << Log::dpInfo
				       << "ActuatorDevice::write() _referenceOffset="
				       << _referenceOffset << Log::dp;
                }
            }
            if (res == 0) {
                _buffer[writeIndex]._status = DataPoint::WRITE;
            } else {
	      if (std::isnan(_lastValueSet)) {
                _buffer[writeIndex]._status = DataPoint::WRITE_NAN;
		Log::getInstance() << "DEBUG: NAN detected! _nextWrite=" << _nextWrite
				   << ", _next=" << _next
				   << Log::flush;
		Log::getInstance() << "Invalid actuator values (nan), no change"
				   << Log::flush;

	      } else {
                _buffer[writeIndex]._status = DataPoint::WRITE_INVALID;
	      }
            }
        } else {
            _buffer[writeIndex]._status = DataPoint::WRITE_SKIP;
        }
        _nextWrite = CIRCULAR_INCREMENT(_nextWrite);

        return res;
    } else if (_buffer[writeIndex]._status == DataPoint::SET_LIMIT ||
            _buffer[writeIndex]._status == DataPoint::SET_GROUP_LIMIT ||
            _buffer[writeIndex]._status == DataPoint::SET_SKIP) {
        _buffer[writeIndex]._status = DataPoint::WRITE_SKIP;
        _nextWrite = CIRCULAR_INCREMENT(_nextWrite);
    }
/*
 * This build flag is for forcing custom, user-defined data to be sent over FCOM
 * on the dev network. This is meant to be used to test things such as magnets,
 * llrf, and bld. This should not be deployed to production. - Kyle Leleux (kleleux) 01/06/25 
 */
#ifdef DEV_FCOM
    else if (ExecConfiguration::getInstance()._forceDataPv.getValue()){
        if (_communicationChannel != NULL) {
            //Log::getInstance() << "_communicationChannel not NULL" << Log::cout;
            //res = _communicationChannel->write(ExecConfiguration::getInstance()._forceActValPv.getValue());
            srand(time(0));
            int randInt = rand() % 5;
            int randInt2 = rand() % 300;
            double randSmallDouble = (double)rand() / RAND_MAX / 1000;
            double randDouble = (double)rand() / RAND_MAX;
            //res = _communicationChannel->write(-57.1);
            //Log::getInstance() << getDeviceIndex() << Log::cout;
            // TODO: Instantiate some pvs for the random int and decimal - Kyle Leleux
            //double value = _forcedValPv.getValue();
            //res = _communicationChannel->write(value);
            //res = _communicationChannel->write(-0.01+randSmallDouble);
            //Log::getInstance() << -0.001 << Log::cout;
            //return res;
            
            if (getDeviceIndex() == 1) { 
                _pdesOffset = std::fmod(_pdesOffset + 0.1,5); 
                res = _communicationChannel->write(70+randInt+randDouble);
                //res = _communicationChannel->write(_pdesStartPoint + _pdesOffset);
                //Log::getInstance() << res << Log::cout;
                return res;
            }
            else if (getDeviceIndex() == 2) {
                res = _communicationChannel->write(9500+randInt2+randDouble);
                //_adesOffset = (_adesOffset + 100) % 300;
                //res = _communicationChannel->write(_adesStartPoint + _adesOffset);
                return res;
            }
            /*
            if (getDeviceIndex() == 1) {
                res = _communicationChannel->write(-0.01 + randSmallDouble);
                return res;
            }
            */
        
    }
}
#endif
    return res;
}

/**
 * This method is very similar to write(), however it writes
 * the specified value to the communication channel instead of the
 * current pending value in the internal buffer.
 *
 * All cursors are updated as if write() was called.
 *
 * @param value value to be written out instead of the current element
 * in the internal buffer
 * @return 0 on success, -1 if there are no values to be written or
 * there has been a problem with the communication channel
 * @author L.Piccoli
 */
int ActuatorDevice::write(double value) {
    //  int writeIndex = _nextWrite;
    int writeIndex = CIRCULAR_DECREMENT(_next);
    if (_buffer[writeIndex]._status == DataPoint::SET) {
        int res = 0;
        if (_communicationChannel != NULL && !std::isnan(value)) {
#ifdef CONTROL_ACTUATORS	      
            res = _communicationChannel->write(value);
#else
            res = 0;
#endif
            // Save offset relative to the reference actuator
            if (_referenceActuator != NULL) {
                _referenceOffset = _referenceActuator->getLastValueSet() - value;
            }
        }
        _buffer[writeIndex]._status = DataPoint::WRITE;
        _nextWrite = CIRCULAR_INCREMENT(_nextWrite);

        return res;
    }
    _nextWrite = CIRCULAR_INCREMENT(_nextWrite);
    return -1;
}

/**
 * Clear contents of internal buffer and reset pointers.
 *
 * @author L.Piccoli
 */
void ActuatorDevice::clear() {
    Device::clear();
    _nextWrite = 0;
    _droppedPoints = 0;
    _referenceOffset = 0;
    Log::getInstance() << Log::flagOffset << Log::dpInfo
		       << "ActuatorDevice::clear() _referenceOffset=0"
		       << Log::dp;
}

/**
 * Returns the last value written out
 *
 * @return latest value available for writing
 * @author L.Piccoli
 */
double ActuatorDevice::peek() {
    int index = CIRCULAR_DECREMENT(_next);

#ifdef DEBUG_PRINT
        if (_name == "A3") {
	  Log::getInstance() << Log::flagPulseId << Log::dpInfo
			     << "ActuatorDevice::peek() A3 only, index="
			     << index << ", value=" << _buffer[index]._value
			     << Log::dp;
	}
#endif
    return _buffer[index]._value;
}

int ActuatorDevice::peek(DataPoint &dataPoint) {
    memcpy(&dataPoint, &_buffer[CIRCULAR_DECREMENT(_next)], sizeof (DataPoint));
    return 0;
}

int ActuatorDevice::peek(DataPoint &dataPoint, int pos) {
    memcpy(&dataPoint, &_buffer[pos], sizeof (DataPoint));
    return 0;
}

/**
 * Returns the PulseId of the last value added to the internal buffer.
 * Usually this value is the next one to be written out.
 *
 * @return PulseId of the last value set
 * @author L.Piccoli
 */
epicsUInt32 ActuatorDevice::peekPulseId() {
    return PULSEID(_buffer[CIRCULAR_DECREMENT(_next)]._timestamp);
}

DataPoint::Status ActuatorDevice::peekStatus() {
    return _buffer[CIRCULAR_DECREMENT(_next)]._status;
}

/**
 * Reconfigure the Actuator Device. The Device::configure() is called first
 * and then the current setting of the actuator is read in. This initial value
 * must be read in because the feedback calculate offsets that should be added
 * to the previous actuator value, which means the initial value must be retrieved
 * from the actual device.
 *
 * If Actuator uses FCOM then the _devNamePv is momentarily appended by the
 * string ":GETFCOM_[_patternIndex - 1]". This allows the feedback to send
 * actuators values to the correct device.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ActuatorDevice::configure(CommunicationChannel::AccessType accessType,
															int patternIndex) {
	std::string deviceName = _devNamePv.getValue();

	// If actuator is the Laser IOC, disable the FBCK Pv.
	size_t found = deviceName.find("IN20:LS11:PCTRL");
	if (found != std::string::npos) {
		_setFbckPv = false;
	}

	if (_patternIndex > 0 && _patternIndex < 5 && !isNull() && _caModePv == false) {
		std::ostringstream fcomDeviceName;
		fcomDeviceName << deviceName << ":GETFCOM_" << _patternIndex - 1;
		_devNamePv = fcomDeviceName.str();
	}

	int res = 0;
	try {
		res = Device::configure(accessType, patternIndex);
	} catch (Exception &e) {
		_devNamePv = deviceName;
		throw;
	}

	if (res != 0) {
		_devNamePv = deviceName;
		Log::getInstance() << Log::showtime << "ERROR: Failed at Device::configure()" << Log::flush;
		Log::getInstance() << Log::showtime << "ERROR: Failed at Device::configure()" << Log::cout;
		_devNamePv = deviceName;
		return res;
	}
	_devNamePv = deviceName;

	res = getInitialSetting();
	if (res < 0) {
		Log::getInstance() << Log::showtime << "ERROR: Failed to get initial actuator value" << Log::flush;
		Log::getInstance() << Log::showtime << "ERROR: Failed to get initial actuator value" << Log::cout;
		return res;
	}

	// Set the external source for the _channelCountPv
	if (_communicationChannel != NULL) {
		_channelCountPv.setExternalValuePtr(_communicationChannel->
				getWriteCountAddress());
		_channelErrorCountPv.setExternalValuePtr(_communicationChannel->
				getWriteErrorCountAddress());
	}

	// Reset the reference offset to pattern 0
	_referenceOffset = 0;

	return 0;
}

/**
 * The _initialChannel is used to retrieve the initial actuator setting
 * if it is not NULL. Usually the setting is read from the actuator 
 * itself, unless in special cases where there is a PV for controlling
 * the actuator and another PV for reading back the actual setting
 * (e.g. BunchCharge laser power)
 *
 * @author L.Piccoli
 */
//void ActuatorDevice::setInitialChannel(CommunicationChannel *initialChannel) {
void ActuatorDevice::setInitialChannel(PvData<double> *initialChannel) {
  _initialChannel = initialChannel;
}

/**
 * The internal buffer is cleared and the current actuator setting is read back
 * from the device.
 *
 * FCOM: If the device uses FCOM channel, then it is up to the FcomChannel
 * class to retrieve the initial value using ChannelAccess.
 *
 * @return 0 if initial value could be retrieved, -1 if there is an error or
 * 1 if the initial value read is the same as the last value set (this may indicate
 * problems for the Longitudital feedback).
 *
 * @author L.Piccoli
 */
int ActuatorDevice::getInitialSetting() {
    clear();
    std::string name = _devNamePv.getValue();

    if (isNull()) {
        if (set(0) != 0) {
            Log::getInstance() << "ERROR: Fail to set initial value to ZERO (NULL Channel)"
			       << Log::flush;
            return -7;
        }
        write(false);
        _lastValueSet = 0;
        return 0;
    }

    // If Actuator is written to file, try to get initial setting from
    // a file named FILE:/path/filename[.ini]
    FileChannel *initialFileChannel = NULL;
    if (isFile()) {
        std::string fileName = name.substr(5, name.npos);
        fileName += ".ini";

        Log::getInstance() << "INFO: Getting initial actuator value from file: "
                << fileName.c_str() << Log::flush;

        initialFileChannel = new FileChannel(CommunicationChannel::READ_ONLY,
                fileName, true);
    }

    std::string exceptionMessage;
    if (_communicationChannel != NULL ||
	initialFileChannel != NULL ||
	_initialChannel != NULL) {
        double value;
        epicsTimeStamp timestamp;
	
	if (_initialChannel != NULL) {
	  /*
	  std::cout<< std::hex << _initialChannel << std::endl;
	  std::cout<<"Reading channel " << _initialChannel->getName() << std::endl;
	  if (_initialChannel->read(value, timestamp) != 0) {
	    Log::getInstance() << "ERROR: Failed to read ActuatorDevice "
			       << name.c_str() << Log::flush;
	    exceptionMessage = "Failed to read " + _initialChannel->getName();
	    throw Exception(exceptionMessage);
	    return -12;
	  }
	  */
	  value = _initialChannel->getValue();
	  std::cout<< "Initial value: " << value << std::endl;
	}
	else {
	  if (initialFileChannel != NULL) {
            if (initialFileChannel->read(value, timestamp) != 0) {
	      Log::getInstance() << Log::showtime << "ERROR: Failed to read ActuatorDevice "
				 << name.c_str() << Log::flush;
	      exceptionMessage = "Failed to read " + initialFileChannel->getName();
	      delete initialFileChannel;
	      initialFileChannel = NULL;
	      throw Exception(exceptionMessage);
	      return -9;
            }
	  } else {
            if (_communicationChannel->read(value, timestamp) != 0) {
	      Log::getInstance() << "ERROR: Failed to read ActuatorDevice. "
				 << name.c_str() << Log::flush;
	      exceptionMessage = "Failed to read " + _communicationChannel->getName();
	      throw Exception(exceptionMessage);
	      return -10;
            }
	  }
	}
	/*
	if (_name == "A6") {
          Log::getInstance() << "ERROR: Fail to set initial value to " << value << Log::flush;
          std::ostringstream strstream;
          strstream << "Check actuator limits. Failed to set " << _communicationChannel->getName()
		        << " initial value to " << value;
          throw Exception(strstream.str());
	  return -50;
	}
	*/
        // Set the value in internal buffer and pretend to write in out
	if (set(value) != 0) {
          Log::getInstance() << "ERROR: Fail to set initial value to " << value << Log::flush;
          std::ostringstream strstream;
          strstream << "Check actuator limits. Failed to set " << _communicationChannel->getName()
		        << " initial value to " << value;
          if (initialFileChannel != NULL) {
	    delete initialFileChannel;
	    initialFileChannel = NULL;
	  }
          throw Exception(strstream.str());
          return -11;
        } else {
	  Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
			     << "INFO: Initial value for " << name.c_str()
			     << " is " << value << " (pattern="
			     << _patternIndex << ")" << Log::dp;
        }
        write(false);
	double lastValueCheck = _lastValueSet;
        _lastValueSet = value;
	if (lastValueCheck == _lastValueSet) {
	Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
			   << "INFO: actuator at same setpoint "
			   << _lastValueSet << Log::dp;
	  return 0;
	}
    }

    if (initialFileChannel != NULL) {
      delete initialFileChannel;
      initialFileChannel = NULL;
    }
    return 0;
}

/**
 * This method initializes the offset between actuators on P0 and P1/2/3.
 *
 * The initial values read from the ":GETFCOM_x" PVs may already have 
 * some offsets that must be kept by the feedback.
 *
 * @author L.Piccoli
 */
void ActuatorDevice::setInitialOffset() {
  if (_referenceActuator != NULL) {
    double offset = _referenceActuator->getLastValueSet() - _lastValueSet;
    Log::getInstance() << Log::flagOffset << Log::dpInfo
		       << "ActuatorDevice::setInitialOffset() from " 
		       << _referenceOffset << " to " << offset << Log::dp;
    _referenceOffset = offset;
  }
}

void ActuatorDevice::zeroOffset() {
  _referenceOffset = 0;
}

/**
 * Assign last value set of the actuator based on current value of reference
 * actuator minus the saved offset between the two.
 *
 * The value based on the offset from the reference actuator may be sent out
 * to the actuator depending on the value of the write input parameter.
 *
 * The _referenceOffset is updated whenever the method write() is called (and
 * the _referenceActuator is not NULL).
 *
 * @param write if true, the setting based on the reference is written to the
 * actuator, if false the value is not written out, just stored in the 
 * _lastValueSet.
 * @author L.Piccoli
 */
void ActuatorDevice::getInitialSettingFromReference(bool writeValue,
						    epicsTimeStamp timestamp) {
    if (_referenceActuator != NULL) {
      double referenceValue = _referenceActuator->getLastValueSet();
      double value = _referenceActuator->getLastValueSet() - _referenceOffset;
      set(value, timestamp);
      write(writeValue);
      _lastValueSet = value;
      
      Log::getInstance() << Log::flagAlgo << Log::dpInfo
			 << "ActuatorDevice::initialFromRef() refDevice="
			 << _referenceActuator->getDeviceName().c_str()
			 << " value=" << value 
			 << " refValue=" << referenceValue
			 << " offset=" << _referenceOffset << Log::dp;
    }
    else {
	Log::getInstance() << Log::flagAlgo << Log::dpInfo
			   << "ActuatorDevice::initialFromRef() no reference - first pattern"
			   << Log::dp;
    }
}

void ActuatorDevice::show() {
    Device::show();
    if (_referenceActuator != NULL) {
        std::cout << "[ref.offset = " << _referenceOffset << "] ";
    }
}

/**
 * Set the _hihiInPv, _highInPv, _lowInPv, _loloInPv to the specified values.
 * HIHI is equal to HIGH, and LOLO is equal to LOW.
 *
 * When the attributes are changes the scanIoRequest is invoked, which propagate
 * the values into the Axx.[HIHI, HIGH, LOW, LOLO] fields.
 *
 * This method is used only by the Longitudinal feedback to protect the actuators
 * against energy changes.
 * 
 * @param high value used for the Axx.HIHI and Axx.HIGH fields
 * @param low value used for the Axx.LOLO and Axx.LOW fields
 * @author L.Piccoli
 */
void ActuatorDevice::setLimits(double high, double low) {
    if (_hihiInPv.getValue() != high) {
        _hihiInPv = high;
        _highInPv = high;
        _hihiInPv.scanIoRequest();
        _highInPv.scanIoRequest();
    }

    if (_loloInPv.getValue() != low) {
        _lowInPv = low;
        _loloInPv = low;
        _lowInPv.scanIoRequest();
        _loloInPv.scanIoRequest();
    }
}

/* 
 * File:   MeasurementDevice.cc
 * Author: lpiccoli
 * 
 * Created on June 1, 2010, 4:07 PM
 */

#include <iostream>
#include <string.h>
#include "Pattern.h"
#include "MeasurementDevice.h"
#include "ActuatorDevice.h"
#include "CaChannel.h"
#include "FcomChannel.h"
#include "FileChannel.h"
#include "Log.h"

USING_FF_NAMESPACE

/**
 * MeasurementDevice constructor, passing parameters to the Device constructor.
 *
 * @author L.Piccoli
 */
MeasurementDevice::MeasurementDevice(std::string loopName, std::string name,
        int bufferSize, PatternMask patternMask, int patternIndex) :
Device(loopName, name, bufferSize, patternMask, patternIndex),
_nextRead(0),
_droppedPoints(0),
_tmit(100),
_timestampBehind(0),
_timestampAhead(0),
_tmitCommunicationChannel(NULL),
// TODO: this PV does not handle multiple pattern
_timestampMismatchCountPv(loopName + " " + name + "TSMISMATCH", 0),
_checkFailCount(0),
_readCount(0),
_getCount(0),
_isBpm(false),
_isBlen(false) {
  _setFbckPv = false; // REMOVEME: TEMPORARY - DEBUG
}

/**
 * MeasurementDeviceDestructor
 *
 * @author L.Piccoli
 */
MeasurementDevice::~MeasurementDevice() {
  if (_tmitCommunicationChannel != NULL) {
    std::cout << "DELETE: " << _devNamePv.getValue() << std::endl;
    delete _tmitCommunicationChannel;
  }
}

/**
 * Read the next measurement value from the internal buffer.
 *
 * CHANGES: Always return the last value read, not the one pointed by
 * _nextRead.
 *
 * @param value new measurement
 * @param timestamp timestamp of the new measurement
 * @return 0 on success, -1 if there are no new values to be returned
 * @author L.Piccoli
 */
int MeasurementDevice::get(double &value, epicsTimeStamp &timestamp) {
    _getCount++;
    int index = CIRCULAR_DECREMENT(_next);
    if (_buffer[index]._status == DataPoint::READ) {
        value = _buffer[index]._value;
        timestamp = _buffer[index]._timestamp;
        _buffer[index]._status = DataPoint::GET;

#ifdef DEBUG_PRINT
        if (_name == "M1") {
	  Log::getInstance() << Log::flagPulseId << Log::dpInfo
			     << "MeasurementDevice::get() M1 only, _next="
			     << (int) _next << ", _nextRead=" << (int) _nextRead
			     << " PULSEID=" << (int) PULSEID(timestamp) << Log::dp;
	}
#endif
        return 0;
    }
    return -1;
}

/**
 * Get the next measurement value from the communication channel.
 *
 * In case of failure retrieving data from communication channel, the
 * internal buffer remains unchanged.
 *
 * If the MeasurementDevice is a BPM (by checking if there is a
 * _tmitCommunicationChannel) then read the associated TMIT value.
 *
 * TODO: if there is a channel reading error then consider adding zero
 * data point with status indicating the error.
 *
 * @return 0 on success, -1 if there is a problem getting data from
 * the device.
 * @author L.Piccoli
 */
int MeasurementDevice::read() {
    _readCount++;
    double value;
    epicsTimeStamp timestamp;
    int status = 0;

    // Read the TMIT value (if this is a BPM)
    if (_tmitCommunicationChannel != NULL) {
      status = _tmitCommunicationChannel->read(_tmit, timestamp, 0.08);
        if (status != 0) {
            _buffer[_next]._status = DataPoint::READ_INVALID;
            _next = CIRCULAR_INCREMENT(_next);
            return -1;
        }
    }

    if (_communicationChannel != NULL) {
      status = _communicationChannel->read(value, timestamp, 0.08);
    }
    else {
      return -1;
    }

    if (status != 0) {
        _buffer[_next]._status = DataPoint::READ_INVALID;
        _next = CIRCULAR_INCREMENT(_next);
        return -1;
    }

    if (isTmit()) {
      _tmit = value;
    }

    _buffer[_next]._value = value;
    _buffer[_next]._timestamp = timestamp;
    _buffer[_next]._status = DataPoint::READ;

    _next = CIRCULAR_INCREMENT(_next);
    if (_buffer[_next]._status == DataPoint::READ) {
        _droppedPoints++;
    }
    _buffer[_next]._status = DataPoint::EMPTY;
    /*
      std::cout 
	<< "MeasurementDevice::read() M1 only, _next="
	<< (int) _next << ", _nextRead=" << (int) _nextRead
	<< " PULSEID=" << (int) PULSEID(timestamp)
	<< ", status=" << _buffer[_next]._status << std::endl;
*/
    return 0;
}

/**
 * Set the average count to the specified value. If value is greated than the
 * buffer size the average count is set to the buffer size.
 *
 * @param averageCount number of point used to calculate average measurement
 * @author L.Piccoli
 */
void MeasurementDevice::setAverageCount(int averageCount) {
    if (averageCount > _bufferSize) {
        _averageCount = _bufferSize;
    } else {
        _averageCount = averageCount;
    }
}

/**
 * Return the average measurement of the last _averageCounter measurements.
 *
 * The average is calculated only when this method is invoked.
 * 
 * @return measurement average
 * @author L.Piccoli
 */
double MeasurementDevice::getAverage() {
    _average = 0;
    int pointsUsed = 0;
    int next = _next;

    for (int i = 0; i < _averageCount; ++i) {
        next = CIRCULAR_DECREMENT(next);
        if (_buffer[next]._status != DataPoint::EMPTY) {
            _average += _buffer[next]._value;
            pointsUsed++;
        }
    }

    _average /= pointsUsed;

    return _average;
}

/**
 * Clear contents of internal buffer and reset pointers.
 *
 * @author L.Piccoli
 */
void MeasurementDevice::clear() {
    Device::clear();
    _nextRead = 0;
    _droppedPoints = 0;
}

/**
 * Returns the next value available for reading. This method behaves differently
 * that get(). Peek() returns the same measurement if called in sequence, while
 * get() returns measurements read from the communication channel once and moves
 * to the next one, such that consecutive calls to get() to not return the same
 * value.
 *
 * @return latest measurement available
 * @author L.Piccoli
 */
double MeasurementDevice::peek() {
    return _buffer[CIRCULAR_DECREMENT(_next)]._value;
}

int MeasurementDevice::peek(DataPoint &dataPoint) {
    memcpy(&dataPoint, &_buffer[CIRCULAR_DECREMENT(_next)], sizeof (DataPoint));
    return 0;
}

int MeasurementDevice::peek(DataPoint &dataPoint, int pos) {
    memcpy(&dataPoint, &_buffer[pos], sizeof (DataPoint));
    return 0;
}

/**
 * Returns the status of the next value available for reading.
 *
 * CHANGE: Always use the last value read in, do not use _nextRead
 *
 * @return status of the latest measurement available
 * @author L.Piccoli
 */
DataPoint::Status MeasurementDevice::peekStatus() {
    return _buffer[CIRCULAR_DECREMENT(_next)]._status;
}

epicsUInt32 MeasurementDevice::peekPulseId() {
    return PULSEID(_buffer[CIRCULAR_DECREMENT(_next)]._timestamp);
}

/**
 * Determines if a MeasurementDevice is a BPM based on the device name. BPMs
 * are always named BPMS:xxx:xxx:xxx.
 * 
 * @return true if device name starts with BPMS:, false otherwise.
 * @author L.Piccoli
 */
bool MeasurementDevice::isBpm() {
    return _isBpm;
}

/**
 * Checks whethes the device name contains the string "BPMS:" or not.
 *
 * @return true if "BPMS:" is present in the device name at any position,
 * false if "BPMS:" is not present.
 * @author L.Piccoli
 */
bool MeasurementDevice::hasBpmName() {
  return hasStringInName("BPMS:");
}

/**
 * Determines if a MeasurementDevice is a BLEN based on the device name. BLENs
 * are always named BLEN:xxx:xxx:xxx.
 * 
 * @return true if device name starts with BLEN:, false otherwise.
 * @author L.Piccoli
 */
bool MeasurementDevice::isBlen() {
    return _isBlen;
}

/**
 * Checks whethes the device name contains the string "BLEN:" or not.
 *
 * @return true if "BLEN:" is present in the device name at any position,
 * false if "BLEN:" is not present.
 * @author L.Piccoli
 */
bool MeasurementDevice::hasBlenName() {
  return hasStringInName("BLEN:");
}

/**
 * Returns whether this device is reading the TMIT value or not. This
 * is needed to avoid reading the TMIT value twice.
 *
 * @return true if the MxDEVNAME ends with ":TMIT", false otherwise
 * @author L.Piccoli
 */
bool MeasurementDevice::isTmit() {
  return _isTmit;
}

/**
 * Checks whethes the device name contains the string ":TMIT" or not.
 *
 * @return true if ":TMIT" is present in the device name at any position,
 * false if ":TMIT" is not present.
 * @author L.Piccoli
 */
bool MeasurementDevice::hasTmitName() {
  return hasStringInName(":TMIT");
}

/**
 * Checks if the device name has the specified string within.
 *
 * @param str string to be searched in the device name
 * @return true if device name has the substring, false if not
 * @author L.Piccoli
 */
bool MeasurementDevice::hasStringInName(std::string str) {
    size_t found = _devNamePv.getValue().find(str);
    if (found != std::string::npos) {
        return true;
    } else {
        return false;
    }
}

/**
 * Reconfigure the MeasurementDevice. This involves invoking the
 * Device::configure() method and then creating a communication channel
 * for receiving TMIT updates - in case this MeasurementDevice is a BPM.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int MeasurementDevice::configure(CommunicationChannel::AccessType accessType,
        int patternIndex) throw (Exception) {
    // Check if the device name indicates this is a BPM or BLEN
    _isBpm = hasBpmName();
    _isBlen = hasBlenName();
    _isTmit = hasTmitName();

    // BLEN devices do not have FBCK PV available
    if (_isBlen) {
        _setFbckPv = false;
    }

    int res = Device::configure(accessType, patternIndex);

    if (res == 0) {
        if (isBpm() && !isTmit()) {
            std::string devName;
	    if (isFile()) {
	      devName = getDeviceFileName();
	    }
	    else {
	      devName = _devNamePv.getValue();
	    }

            size_t lastColon = devName.find_last_of(":") + 1;
            if (lastColon == std::string::npos) {
                throw Exception("Failed to build TMIT PV name");
            }

            std::string tmitName;	    
            tmitName = devName.substr(0, lastColon);
            tmitName += "TMIT";
	    
	    Log::getInstance() << Log::flagConfig << Log::dpInfo
			       << "INFO: MeasurementDevice::configure() "
			       << devName.c_str() << ", " << tmitName.c_str() << ")"
			       << Log::dp;
	    
            if (!isFile()) {
                if (_caModePv == true) {
                    _tmitCommunicationChannel = new CaChannel(accessType, tmitName);
                } else {
                    _tmitCommunicationChannel = new FcomChannel(accessType, tmitName);
                }
            } else {
                _tmitCommunicationChannel = new FileChannel(accessType, tmitName, true);
            }
        }
        // Set the external source for the _channelCountPv
        if (_communicationChannel != NULL) {
            _channelCountPv.setExternalValuePtr(_communicationChannel->
                    getReadCountAddress());
            _channelErrorCountPv.setExternalValuePtr(_communicationChannel->
                    getReadErrorCountAddress());
        }

        return 0;
    } else {
        return res;
    }
}

/**
 * Disconnects the device from the CommunicationChannels. This method is
 * overriden because the MeasurementDevice must also have the _tmitCommunicationChannel
 * disconnected.
 * 
 * @author L.Piccoli
 */
void MeasurementDevice::disconnect() {
  if (_tmitCommunicationChannel != NULL) {
    delete _tmitCommunicationChannel;
    _tmitCommunicationChannel = NULL;
  }

  Device::disconnect();
}

void MeasurementDevice::show() {
    Device::show();
    if (_tmitCommunicationChannel != NULL) {
        std::cout << "[TMIT = " << _tmit << "] ";
        std::cout << "(" << _checkFailCount
		  << ", " << _readCount << ", "
		  << _getCount << ", ahead=" 
		  << _timestampAhead << ", behind="
		  << _timestampBehind << ")";

	if (!isFcom()) {
	  CaChannel *caChannel = reinterpret_cast<CaChannel *>(_communicationChannel);
	  std::cout << "[" << caChannel->_readStats.getAverage() << "/"
		    << caChannel->_readStats.getMax() << "/"
		    << caChannel->_readStats.getMin() << "]";
	}
    }
}

/**
 * This method provides a way to change the value read from the communication
 * channel to a user provided value.
 *
 * TODO: add code to make sure this is replacing the correct value!
 *
 * @param value new value to replace the latest read value
 * @return always 0
 * @author L.Piccoli
 */
int MeasurementDevice::replace(double value) {
    _buffer[CIRCULAR_DECREMENT(_next)]._value = value;
    return 0;
}

/**
 * This method is for testing only. It allows a value to be inserted into
 * the internal buffer without going through the communication channel.
 *
 * TODO: This code was taken from the read() method -> needs refactoring!
 *
 * @param value value to be written to the buffer (which will eventually be
 * read back)
 * @param timestamp timestamp for the value
 * @return always 0
 * @author L.Piccoli
 */
int MeasurementDevice::insert(double value, epicsTimeStamp timestamp) {
    if (_buffer[_next]._status == DataPoint::READ) {
        _droppedPoints++;
        _nextRead = CIRCULAR_INCREMENT(_nextRead);
    }

    _buffer[_next]._value = value;
    _buffer[_next]._timestamp = timestamp;
    _buffer[_next]._status = DataPoint::READ;

    _next = CIRCULAR_INCREMENT(_next);

    // Read the TMIT value (if this is a BPM)
    if (_tmitCommunicationChannel != NULL) {
        if (_tmitCommunicationChannel->read(_tmit, timestamp) != 0) {
            Log::getInstance() << "ERROR: Failed to read TMIT "
                    << "value for MeasurementDevice "
                    << _devNamePv.getValue().c_str() << Log::flush;
            return -1;
        }
    }

    return 0;
}

/**
 * Check the PulseId of the lastest value read from the CommunicationChannel.
 * This method is invoked by the MeasurementCollector just after calling read().
 *
 * If the MeasurementDevice is a BLEN then return true without checking the
 * pulseid.
 *
 * @return true if PulseIds from the measurement (FCOM) and the pattern (EVR)
 * match
 * @author L.Piccoli
 */
bool MeasurementDevice::checkPulseId(epicsUInt32 patternPulseId) {
    if (_isBlen) {
        return true;
    }

    //    if (_caModePv == false && !isFile()) {
    if (isFcom() && !isFile()) {
        if (_buffer[CIRCULAR_DECREMENT(_next)]._status != DataPoint::READ) {
            _checkFailCount++;
            return true;
        }
        // The last value read is pointed by the element just before _next.
        epicsUInt32 measurementPulseId = PULSEID(_buffer[CIRCULAR_DECREMENT(_next)]._timestamp);
        if (patternPulseId == measurementPulseId) {
            return true;
        } else {
            // Change the data status to indicate the PulseId mismatch
            _buffer[CIRCULAR_DECREMENT(_next)]._status = DataPoint::READ_TSMISMATCH;

            // Increase timestamp mismatch count PV
            _timestampMismatchCountPv = _timestampMismatchCountPv.getValue() + 1;

	    if (patternPulseId > measurementPulseId) {
	      _timestampBehind++;
	    }
	    else {
	      _timestampAhead++;
	    }

	    /*
            Log::getInstance() << "ERROR: " << (int) patternPulseId << " != "
                    << (int) measurementPulseId << " (pattern "
                    << _patternIndex << ")"
                    << Log::flush;
	    */
            return false;
        }
    }
    return true;
}

void MeasurementDevice::resetNextRead() {
    if (_buffer[_next]._status == DataPoint::EMPTY) {
        _nextRead = CIRCULAR_DECREMENT(_next);
    } else {
        _nextRead = _next;
    }
}

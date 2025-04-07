/*
 * File:   Device.cc
 * Author: lpiccoli
 *
 * Created on May 28, 2010, 11:38 AM
 */

#include <iostream>
#include <stdlib.h>
#include "Device.h"
#include "FileChannel.h"
#include "CaChannel.h"
#include "FcomChannel.h"
#include "NullChannel.h"
#include "Log.h"
#include "PatternManager.h"
#include <fcomUtil.h>
#include <fcom_api.h>
#include <fcomLclsBpm.h>
#include <fcomLclsBlen.h>
#include <fcomLclsFFCtrl.h>

USING_FF_NAMESPACE

/**
 * Device constructor, allocates the internal buffer according to the specified
 * size.
 *
 * @param name device name (used by ChannelAccess and/or FCOM to access real
 * device)
 * @param bufferSize number of data points that the internal buffer can hold
 * @param patternMask
 * @param patternIndex
 * @author L.Piccoli
 */
Device::Device(std::string slotName, std::string name, int bufferSize,
        PatternMask patternMask, int patternIndex) :
_slotName(slotName),
_name(name),
_buffer(0),
_bufferSize(bufferSize),
_next(0),
_communicationChannel(NULL),
_patternMask(patternMask),
_patternIndex(patternIndex),
_hihiPv(slotName + " " + name + "HIHI", 1000),
_highPv(slotName + " " + name + "HIGH", 500),
_lowPv(slotName + " " + name + "LOW", -500),
_loloPv(slotName + " " + name + "LOLO", -1000),
_caModePv(slotName + " " + name + "CAMODE", false),
_usedPv(slotName + " " + name + "USED", false),
_devNamePv(slotName + " " + name + "DEVNAME", name + "DEVNAME"),
_fbckCommunicationChannel(NULL),
_setFbckPv(true),
_usedByLoopPv(slotName + " " + name + "USEDBYLOOP", true),
_usedByLoop(true),
_channelCountPv(slotName + " " + name + "DATACNT", 0),
_channelErrorCountPv(slotName + " " + name + "DATAERRORS", 0),
_isFile(false),
_measCheckInclusionPv(slotName + " " + name + "MEASCHECKINCL"),
_measCheckInclusion(true) {
    _buffer = new DataPoint[bufferSize];
    for (int i = 0; i < _bufferSize; ++i) {
        _buffer[i]._status = DataPoint::EMPTY;
    }
    _usedByLoopPv.initScanList(); // This enable PV updates.
    _isFile = isFile();
}

/**
 * Device destructor, deletes the internal buffer
 * /
Device::Device(const Device& orig) {
}
*/
Device::~Device() {
    delete [] _buffer;
    if (_fbckCommunicationChannel != NULL) {
    	delete _fbckCommunicationChannel;
    }
    if (_communicationChannel != NULL) {
      //      std::cout << "DELETE: " << _devNamePv.getValue() << " (Device)" << std::endl;
      delete _communicationChannel;
    }
}

/**
 * Set the communication channel for this Device.
 * ONLY USED BY TEST METHODS - so the destructor can
 * delete all _communicationChannels assoc with this object
 *
 * @param newChannel the new communication channel
 * @author L.Piccoli
 */
void Device::setCommunicationChannel(CommunicationChannel *newChannel) {
    _communicationChannel = newChannel;
}

/**
 * Clear contents of internal buffer and reset pointers.
 *
 * @author L.Piccoli
 */
void Device::clear() {
    for (int i = 0; i < _bufferSize; ++i) {
        _buffer[i]._status = DataPoint::EMPTY;
    }
    _next = 0;
}

/**
 * Set the pattern mask for this device.
 *
 * @param patternMask the new pattern mask
 * @author L.Piccoli
 */
void Device::setPatternMask(PatternMask patternMask) {
    _patternMask = patternMask;
}

/**
 * Returns the pattern mask associated with the device.
 *
 * @return pattern mask for this device
 * @author L.Piccoli
 */
PatternMask Device::getPatternMask() {
    return _patternMask;
}

/**
 * Verifies if this device is the same as the specified other device.
 * The devices are considered the same if the name and the pattern masks are the
 * same.
 *
 * @param other the other device being compared
 * @returns true if names and patterns are the same, false otherwise
 * @author L.Piccoli
 */
bool Device::operator==(const Device &other) {
    if (_name == other._name &&
	_patternMask == other._patternMask) {
      Log::getInstance() << Log::dpInfo << "INFO: equal" << Log::flush;

        return true;
    } else {
      Log::getInstance() << Log::dpInfo << "INFO: not equal" << Log::flush;
        return false;
    }
}

/**
 * Checks whether a this device and another one are different. Devices are
 * different if either name or pattern mask or both are different.
 *
 * @param other the other device being compared
 * @returns true if names and pattern masks are the same, false otherwise
 * @author L.Piccoli
 */
bool Device::operator!=(const Device &other) {
    return !(*this == other);
}

/**
 * Defines the order of devices in std::map.
 *
 * @author L.Piccoli
 */
bool Device::operator<(const Device &other) {
    std::string name = _name;
    std::string otherName = other._name;

    if (_isFile) {
        size_t lastSlash = _name.find_last_of("/");
        name = _name.substr(lastSlash + 1);

        lastSlash = other._name.find_last_of("/");
        if (lastSlash != std::string::npos) {
	  otherName = other._name.substr(lastSlash + 1);
        }
    }

    // If names begin with 'M', 'A' or 'S' then the order
    // depends on the number following the first letter.
    // The PV database is configured such that the devices are named:
    // M1, M2..., M10, M11,... and ordering by string does not work
    // because M10 < M1 -> we need M1 < M10.
    // We need to extract the device index from the name and compare those.
    if (name[0] == otherName[0] &&
            (name[0] == 'M' || name[0] == 'A' || name[0] == 'S')) {
        if (name.size() < otherName.size()) {
            return true;
        } else if (name.size() > otherName.size()) {
            return false;
        }
    }

    if (name.compare(otherName) > 0) {
        return false;
    } else if (name.compare(otherName) < 0) {
        return true;
    } else {
      return _patternMask < other._patternMask;
    }
}

/**
 * Display information about the device.
 *
 * @author L.Piccoli
 */
void Device::show() {
    std::string used = "true";
    std::string channel = "CA";

    if (_isFile) {
        channel = "File";
    } else {
        if (_caModePv == false) {
            channel = "FCOM";
        }
    }
    if (_usedPv == false) {
        used = "false";
    }

    std::cout << _name << ": " << _devNamePv.getValue()
            << " (" << channel << ", used=" << used << ", index="
            << _patternIndex << ") ";
    if (_communicationChannel == NULL) {
        std::cout << "[NO CHANNEL]";
    } else {
        std::cout << "[read="
                << _communicationChannel->getReadCount() << "/err="
                << _communicationChannel->getReadErrorCount() << ", write="
                << _communicationChannel->getWriteCount() << "/err="
                << _communicationChannel->getWriteErrorCount() << "] ";
    }
}

/**
 * Disconnects the device from the CommunicationChannels. This method is
 * invoked when a Loop is stopped.
 * 
 * Reconnection occurs when the Loop is started (see reconfigure() method).
 * 
 * @author L.Piccoli
 */
void Device::disconnect() {
  if (_communicationChannel != NULL) {
    delete _communicationChannel;
    _communicationChannel = NULL;
  }

  if (_fbckCommunicationChannel != NULL) {
    delete _fbckCommunicationChannel;
    _fbckCommunicationChannel = NULL;
  }

  Log::getInstance() << Log::dpInfo
		     << "INFO: " << _devNamePv.getValue().c_str()
		     << " disconnected." << Log::flush;
}

/**
 * Reconfigure the device. This involves:
 * - Check device name through $(LOOP):*DEVNAME PV
 * - Verify whether actual device is accessible through ChannelAccess or FCOM
 *
 * @param accessType
 * @param patternIndex (optional, default value -1). Used if FileChannel is
 * used
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int Device::configure(CommunicationChannel::AccessType accessType,
        int patternIndex) {
    if (_communicationChannel != NULL) {
        disconnect();
    }

    std::string name = _devNamePv.getValue();

    // If _devName starts with "FILE:", and NO_CHANNEL is selected,
    // change accessType to WRITE_ONLY -> this is for debugging StateDevices
    // allowing values to be written to file instead of being discarded
    if (accessType == CommunicationChannel::NO_CHANNEL && _isFile) {
        accessType = CommunicationChannel::WRITE_ONLY;
    }

    if (accessType == CommunicationChannel::NO_CHANNEL) {
        _communicationChannel = new NullChannel(accessType);
        _fbckCommunicationChannel = new NullChannel(accessType);
        return 0;
    }

    // Check if the device name indicates that the channel to be used is of
    // type FileChannel (for testing/debugging only)
    // Attention: the string can have up to 40 characters only!
    if (isFile()) {
    	if (name.find("DES") == std::string::npos) {
    		createFileChannel(name, patternIndex, accessType);
    		return 0;
    	}
    	else {
    		switch (getDeviceIndex()) {
    		case 1:
    			name = "SIOC:SYS0:ML01:AO001";
    			break;
    		case 2:
    			name = "SIOC:SYS0:ML01:AO002";
    			break;
    		case 3:
    			name = "SIOC:SYS0:ML01:AO003";
    			break;
    		case 4:
    			name = "SIOC:SYS0:ML01:AO004";
    			break;
    		case 5:
    			name = "SIOC:SYS0:ML01:AO005";
    			break;
    		case 6:
    			name = "SIOC:SYS0:ML01:AO006";
    			break;
    		default:
    			name = "SIOC:SYS0:ML01:AO010";
    			break;
    		}
    		_caModePv = true;
    	}
    }

    // If device name starts with "NULL" create a NullChannel
    if (isNull()) {
        _communicationChannel = new NullChannel(accessType);
        _fbckCommunicationChannel = new NullChannel(accessType);
        return 0;
    }

    if (name == "") {
        Log::getInstance() << "ERROR: Device has empty DEVNAME" << Log::flush;
        throw Exception("Device name is empty, check configuration");
    }

    if (_caModePv == true) {
      _communicationChannel = new CaChannel(accessType, name);
    } else {
      //      std::cout << "Create FcomChannel for " << name << std::endl;
      _communicationChannel = new FcomChannel(accessType, name);
    }
    // Create a ChannelAccess connection to set the FBCK device
    _fbckCommunicationChannel = NULL;
    if (!_isFile && _setFbckPv) {
        createFbckPv(accessType);
    }
    return 0;
}

bool Device::isSetFbckPvEnabled() {
  return _setFbckPv;
}

/**
 * If the Device is supposed to have a FBCK PV to be controlled, this method creates
 * a CaChannel to it.
 *
 * The name of the FBCK PV for measurement devices and actuators that are not RF
 * (PV does not start with ACCL) have the last PV field name replaced with the string
 * "FBCK", e.g.:
 *
 *   BPMS:LI20:230:X -> BPMS:LI20:230:FBCK
 *
 * RF actuators whose PV name starts with ACCL must have the "FBCK" string appended 
 * to the original nome, e.g.:
 *
 *   ACCL:LI24:2:ADES -> ACCL:LI24:2:ADES:FBCK
 * 
 * If the device uses FCOM, then the name will have a ":GETFCOM_x" added to the name.
 * this string must be removed before appending/replacing with the "FBCK" string.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int Device::createFbckPv(CommunicationChannel::AccessType accessType) {
    // If name contains ":GETFCOM_x" then we must remove it before
    // appending ":FBCK"
    std::string deviceName = _devNamePv.getValue();
    std::string fcomString = ":GETFCOM_";
    size_t found = deviceName.find(fcomString);
    if (found != std::string::npos) {
        deviceName = deviceName.substr(0, (int) (found));
    }

    std::string acclName = "ACCL:";
    found = deviceName.find(acclName);
    std::string fbckName;

    // Used to find if feedback is using Matlab PVs as actuators
    std::string siocName = "SIOC:";
    size_t foundMatlab = deviceName.find(siocName);

    size_t lastColon = deviceName.find_last_of(":"); // + 1;
    if (lastColon == std::string::npos) {
      //        std::stringstream s;
      _stringStream.str(std::string()); // Clear the stream
      _stringStream << "Failed to create FBCK PV for " << deviceName;
      throw Exception(_stringStream.str());
    }

    if (foundMatlab != std::string::npos) {
      fbckName = "SIOC:SYS0:ML02:AO250";
      std::cout << "Feedback PV: " << fbckName << std::endl;
    }
    else {
      // If has ACCL, then append ":FBCK" to the PV name
      if (found != std::string::npos) {
        fbckName = deviceName;
        fbckName += ":FBCK";
      }// if there is no ACCL, remove last part of PV name and add ":FBCK"
      else {
        fbckName = deviceName.substr(0, lastColon);
        fbckName += ":FBCK";
      }
    }

    _fbckCommunicationChannel = new CaChannel(accessType, fbckName);

    return 0;
}

/**
 * Create a FileChannel instance if the Device is configured to read/write
 * to files. Device name must start with string 'FILE:'.
 *
 * If there are two or more patterns registered with the PatternManager,
 * then the file name is appended to the pattern index returned for the
 * pattern hold by the device.
 *
 * @return 0 if file could be opened for reading or writing, -1 on failure
 * @author L.Piccoli
 */
int Device::createFileChannel(std::string name, int patternIndex,
        CommunicationChannel::AccessType accessType) {
    std::string fileName = name.substr(5, name.npos);
    if (patternIndex > 0) {
        fileName = getFileChannelName(fileName, patternIndex);
    }

    Log::getInstance() << Log::flagConfig << Log::dpInfo
		       << "INFO: Device::createFileChannel() file ["
		       << fileName.c_str() << "] " << Log::dp;

    _communicationChannel = new FileChannel(accessType, fileName, true);

    return 0;
}

std::string Device::getDeviceFileName() {
  if (isFile()) {
    return _devNamePv.getValue().substr(5, _devNamePv.getValue().npos);
  } else {
    return "";
  }
}

std::string Device::getFileChannelName(std::string fileName, int patternIndex) {
    std::ostringstream name;
    name << fileName << "-P" << _patternIndex;
    Log::getInstance() << "INFO: fileChannel name " << name.str().c_str() << Log::flush;
    std::cout << "INFO: fileChannel name " << name.str().c_str() << std::endl;
    return name.str();
}

/**
 * @author L.Piccoli
 */
int Device::setFeedbackPv(bool state) {
    if (_fbckCommunicationChannel != NULL) {
        if (state) {
#ifdef CONTROL_ACTUATORS
	  return _fbckCommunicationChannel->write(1.0);
#endif
        } else {
#ifdef CONTROL_ACTUATORS
	  return _fbckCommunicationChannel->write(0.0);
#endif
        }
    }
    else {
      if (state) {
	  Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			     << "Device::setFeedbackPv(ON) "
			     << getDeviceName().c_str()
			     << Log::dp;
        } else {
	  Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			     << "Device::setFeedbackPv(OFF) "
			     << getDeviceName().c_str()
			     << Log::dp;
        }
    }

    return 0;
}

/**
 * Return the last value set to the FBCK PV.
 *
 * @return true if FBCK PV is ON, false if it is OFF, or in case the
 * _fbckCommunicationChannel is not valid.
 * @author L.Piccoli
 */
bool Device::getFeedbackPv() {
    if (_fbckCommunicationChannel != NULL) {
        if (_fbckCommunicationChannel->getLastValue() == 0.0) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}

/**
 * Returns whether the device name starts with the string 'FILE:',
 * which indicates that this device is connectod to a file (for reading
 * or writing).
 *
 * @return true if device name starts with 'FILE:', false otherwise.
 * @author L.Piccoli
 */
bool Device::isFile() {
    if (_devNamePv.getValue().compare(0, 5, "FILE:") == 0) {
        return true;
    }
    return false;
}

/**
 * Returs whether the device name starts with the string 'NULL',
 * indicating that the NullChannel is used as communication channel.
 *
 * @return true if device name starts with 'NULL', false otherwise.
 * @author L.Piccoli
 */
bool Device::isNull() {
    if (_devNamePv.getValue().compare(0, 4, "NULL") == 0) {
        return true;
    }
    return false;
}

/**
 * Return whether the device is configured to use FCOM.
 * The device may not be using FCOM if its name starts with 'FILE:' or 'NULL'
 *
 * @return true if the CAMODE PV is false, false if CAMODE is true
 * @author L.Piccoli
 */
bool Device::isFcom() {
    return !_caModePv.getValue();
}

/**
 * Returns the name of the device set through the DEVNAME PV
 *
 * @return the device name set in the DEVNAME PV
 * @author L.Piccoli
 */
std::string Device::getDeviceName() {
    return _devNamePv.getValue();
}

/**
 * Return the index that follows the letter (S, M or A) in
 * the device name (e.g. M1 -> index 1, M14 -> index 14).
 *
 * @return the device index, which is device number in the
 * current configuration
 * @author L.Piccoli
 */
int Device::getDeviceIndex() {
  // Skip the first letter, the rest of the string is the index
  std::string indexStr = _name.substr(1);

  return atoi(indexStr.c_str());
}

void Device::setUsedBy(bool used) {
  if (_usedByLoopPv.getValue() != used) {
    Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
		       << "Device::setUsedBy(";
    if (used) {
      Log::getInstance() << "TRUE";
    }
    else {
      Log::getInstance() << "FALSE";
    }
    
    Log::getInstance() << ") : "
		       << getDeviceName().c_str()
		       << Log::dp;
  }
  _usedByLoopPv = used;
  _usedByLoopPv.scanIoRequest();
  _usedByLoop = used;
}

bool Device::getUsedBy() {
  //    return _usedByLoopPv.getValue();
  return _usedByLoop;
}


/* 
 * File:   FcomChannel.cc
 * Author: lpiccoli
 * 
 * Created on July 2, 2010, 2:47 PM
 * Revision on February 18th, 2016 1:20 PM - 
 * Removing all previous reference to LINUX compiler flag in previous 
 * linux/RTEMS #ifdef/#else token structure
 */

#include <iostream>
#include <exception>
#include <sstream>
#include "FcomChannel.h"
#include <fcomUtil.h>
#include <fcom_api.h>
#include <fcomLclsBpm.h>
#include <fcomLclsBlen.h>
#include <fcomLclsFFCtrl.h>
#include "Log.h"
#include "Exception.h"

USING_FF_NAMESPACE

/**
 * Create a FCOM Channel using the specified name as the device name,
 *
 * @param accessType defines whether this channel is for read access
 * (CommunicationChannel::READ_ONLY) or write access
 * (CommunicationChannel::WRITE_ONLY)
 * @param name name of the FCOM device.
 * @param init specifies whether device should be initialized at construction
 * time (default = true)
 * @author L.Piccoli
 */
FcomChannel::FcomChannel(CommunicationChannel::AccessType accessType,
        std::string name, bool init) :
  CommunicationChannel(accessType),
  _name(name),
  _id(0),
  _counter(0),
  _readCaChannel(NULL) {
    if (init) {
        initialize();
    }


    //    std::cout << "FcomChannel() constructor (_id=" << _id << ")" << std::endl;
}

/**
 * Destructor, unsubscribes channel from Fcom.
 *
 * @author L.Piccoli
 */
FcomChannel::~FcomChannel() {
  if (_readCaChannel != NULL) {
    delete _readCaChannel;
  }
  fcomUnsubscribe(_id);
}

/**
 * Initializes the channel, by first finding out the FcomID for the device
 * name (PV) and then subscribing to updates using fcomSubscribe() call.
 *
 * @return -1 if PV name cannot be found or subscribe operation failed.
 * @author L.Piccoli
 */
int FcomChannel::initialize() {
  //  std::cout << "INIT: " << _name << " (_id = " << _id << ")" << std::endl;

  // Make sure this channel is not initialized more than once
  if (_id != 0) {
    throw Exception("Initializing FcomChannel twice");
  }

    /**
     * If this FcomChannel is write only and the read() method is
     * invoked we must have a CaChannel ready to perform the read.
     */
    if (_accessType == CommunicationChannel::WRITE_ONLY) {
      _readCaChannel = new CaChannel(CommunicationChannel::READ_ONLY, _name);
    }

    std::string fcomName = _name;
    if (fcomName.find("STATES") != std::string::npos) {
        fcomName = fcomName.substr(0, fcomName.size() - 1);
    }

    _id = fcomLCLSPV2FcomID(fcomName.c_str());
    // If fcomLCLSPV2FcomID fails it returns FCOM_ID_NONE, which is 0
    if (_id == 0) {
      _stringStream.str(std::string()); // Clear the stream
      _stringStream << "Failed to get FCOM ID for " << fcomName;
      throw Exception(_stringStream.str());
    }
    int value = fcomSubscribe(_id, FCOM_ASYNC_GET);
    if (value != 0) {
      _stringStream.str(std::string()); // Clear the stream
      _stringStream << "Failed on fcomSubscribe for " << fcomName << ". "
		    << " Error code " << value;
      throw Exception(_stringStream.str());
    }

    // First check if the name indicates it is a BLEN
    std::string imaxString = ":AIMAX";
    size_t found = _name.find(imaxString);
    if (found != std::string::npos) {
        _type = BLEN_AIMAX;
        return 0;
    } else {
        imaxString = ":BIMAX";
        found = _name.find(imaxString);
        if (found != std::string::npos) {
            _type = BLEN_BIMAX;
            return 0;
        }
    }

    std::string::iterator it;
    it = _name.end();
    --it;
    char lastChar = (*it);
    switch (lastChar) {
        case 'Y':
            _type = BPM_Y;
            break;
        case 'T':
            _type = BPM_TMIT;
            break;
        case 'X':
            _type = BPM_X;
            break;
        case '1':
            _type = STATE1;
            break;
        case '2':
            _type = STATE2;
            break;
        case '3':
            _type = STATE3;
            break;
        case '4':
            _type = STATE4;
            break;
        case '5':
            _type = STATE5;
            break;
        case '6':
            _type = STATE6;
            break;
        default:
            _type = ACTUATOR;
    }
    return 0;
}

/**
 * Read the next value from Fcom.
 *
 * If FCOM is used for writing only, then the read operation creates a
 * ChannelAccess communication channel to retrieve the data. This is used
 * by the FCOM enabled actuators, which must be read via ChannelAccess.
 *
 * @param value its an output parameter that contains the read value. In
 * case of error then the value is ZERO - and should *not* be used.
 * @param timestamp
 * @return 0 in case of success reading the next value from the file, -1 if
 * there is an error, on a positive number for FCOM BPM status (see
 * fcomLclsBpm.h file in fcomUtil module)
 * @author L.Piccoli
 */
int FcomChannel::read(double &value, epicsTimeStamp & timestamp, double timeout) {
    // This is to cover the case when ActuatorDevices using FCOM is initialized and
    // the channel must be read. Since FCOM does not support reading of the actuators
    // we must create a ChannelAccess connection, get the initial value
    // and close the connection.
    if (_accessType == CommunicationChannel::WRITE_ONLY) {
        //CaChannel *caChannel;

        int status = _readCaChannel->read(value, timestamp, timeout);

        return status;
    }

    FcomBlobRef blob;

    int status = fcomGetBlob(_id, &blob, 0);
    if (status != 0) {
      Log::getInstance() << Log::flagBuffer << Log::dpInfo
			 << "ERROR: " << fcomStrerror(status) << " "
			 << _name.c_str() << Log::dp; //Log::flushpvonly;
        _readErrorCount++;
        value = 0;
        fcomReleaseBlob(&blob);
        return status;
    }
    switch (_type) {
        case BPM_Y:
            value = blob->fcbl_bpm_Y;
            break;
        case BPM_TMIT:
            value = blob->fcbl_bpm_T;
            break;
        case BLEN_AIMAX:
            value = blob->fcbl_blen_aimax;
            break;
        case BLEN_BIMAX:
            value = blob->fcbl_blen_bimax;
            break;
        default:
            value = blob->fcbl_bpm_X;
    }
    timestamp.secPastEpoch = blob->fc_tsHi;
    timestamp.nsec = blob->fc_tsLo;
    _readCount++;

    fcomReleaseBlob(&blob);

    _lastValue = value;

    return 0;
}

int FcomChannel::readCaChannel(double &value, epicsTimeStamp &timestamp) {
    return 0;
}

/**
 * Write the specified value to the Fcom.
 *
 * TODO: Part of this code should be extracted out and called once for a set
 * of devices (e.g. blob initialization).
 *
 * @param value new value to be written out.
 * @return 0 if value was written successfully, -1 if there is an error
 * @author L.Piccoli
 */
int FcomChannel::write(double value) {
    epicsTimeStamp timestamp;
    timestamp.nsec = 0;
    timestamp.secPastEpoch = 0;
    return write(value, timestamp);
}

/**
 * Write the specified value to the Fcom.
 *
 * TODO: Part of this code should be extracted out and called once for a set
 * of devices (e.g. blob initialization).
 *
 * @param value new value to be written out.
 * @param value timestamp
 * @return 0 if value was written successfully, -1 if there is an error
 * @author L.Piccoli
 */
int FcomChannel::write(double value, epicsTimeStamp timestamp) {
    FcomBlob blob;

    // Code common for Fcom channel instances
    blob.fc_vers = FCOM_PROTO_VERSION;
    blob.fc_tsHi = 0xDD;//timestamp.secPastEpoch;
    blob.fc_tsLo = 0xEE;//timestamp.nsec;
    blob.fc_stat = _counter;
    _counter++;

    // Code for this specific channel
    blob.fc_idnt = _id;

    blob.fc_type = FCOM_EL_DOUBLE;
    blob.fc_nelm = 1;
    blob.fc_dbl = &value;

    int status = fcomPutBlob(&blob);
    //std::cout << "ACT blob out" << std::endl;
    if (status != 0) {
        Log::getInstance() << "ERROR: " << fcomStrerror(status) << " - "
			   << _name.c_str() << " FCOM ID: "
			   << (int) _id << Log::flush;
        _writeErrorCount++;
        return -1;
    }
    _writeCount++;

    _lastValue = value;

    return 0;
}

extern "C" {
  void FCOMWrite(char *pvName, double value) {
    FcomID _id;

    _id = fcomLCLSPV2FcomID(pvName);
    if (_id == 0) {
      std::cout << "Failed to get FCOM ID for " << pvName << std::endl;
      return;
    }

    FcomBlob blob;

    // Code common for Fcom channel instances
    blob.fc_vers = FCOM_PROTO_VERSION;
    blob.fc_tsHi = 0xDD;//timestamp.secPastEpoch;
    blob.fc_tsLo = 0xEE;//timestamp.nsec;
    blob.fc_stat = 0;

    // Code for this specific channel
    blob.fc_idnt = _id;

    blob.fc_type = FCOM_EL_DOUBLE;
    blob.fc_nelm = 1;
    blob.fc_dbl = &value;

    int status = fcomPutBlob(&blob);
    if (status != 0) {
      std::cout << "ERROR: " << fcomStrerror(status) << " - "
		<< pvName << " FCOM ID: "
		<< (int) _id << std::endl;
      return;
    }
    std::cout << pvName << " = " << value << std::endl;
  }
}



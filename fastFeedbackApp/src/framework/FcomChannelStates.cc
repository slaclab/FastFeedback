/* 
 * File:   FcomChannelStates.cc
 * Author: lpiccoli
 * 
 * Created on April 4, 2011, 3:07 PM
 * Revision on February 18th, 2016 at 3:20 PM (ababbitt) 
 * Removing all previous reference to LINUX compiler flag in previous 
 * linux/RTEMS #ifdef/#else token structure
 *
 */

#include <iostream>
#include <exception>
#include <sstream>
#include "FcomChannelStates.h"
#include <fcomUtil.h>
#include <fcom_api.h>
#include <fcomLclsFFCtrl.h>
#include "CaChannel.h"
#include "Log.h"
#include "Exception.h"
#include "evrTime.h"

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
FcomChannelStates::FcomChannelStates(CommunicationChannel::AccessType accessType,
        std::string name, bool init) : FcomChannel(accessType, name, false) {
    if (init) initialize();
}

/**
 * Destructor, unsubscribes channel from Fcom.
 *
 * @author L.Piccoli
 */
FcomChannelStates::~FcomChannelStates() {
    fcomUnsubscribe(_id);
}

/**
 * Initializes the channel, by first finding out the FcomID for the device
 * name (PV) and then subscribing to updates using fcomSubscribe() call.
 *
 * @return -1 if PV name cannot be found or subscribe operation failed.
 * @author L.Piccoli
 */
int FcomChannelStates::initialize() {
    std::string fcomName = _name;

    _id = fcomLCLSPV2FcomID(fcomName.c_str());
    if (_id < 0) {
      _stringStream.str(std::string()); // Clear the stream
      _stringStream << "Failed to get FCOM ID for " << fcomName;
      throw Exception(_stringStream.str());
    }
    _type = STATES;

    return 0;
}

/**
 * Not Supported
 */
int FcomChannelStates::read(double &value, epicsTimeStamp & timestamp) {
    return -1;
}

/**
 * Save the specified value in the blob at the given state index.
 *
 * @param value new state value
 * @param stateIndex position within the blob data, which is equivalent
 * to the state index (state index goes from 1 to 10).
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int FcomChannelStates::write(float value, int stateIndex) {
    if (stateIndex < 1 || stateIndex > MAX_STATES) {
        return -1;
    }
    _blobData[stateIndex - 1] = value;

    return 0;
}

/**
 * Write the current blob to Fcom.
 *
 * TODO: Part of this code should be extracted out and called once for a set
 * of devices (e.g. blob initialization).
 *
 * @return 0 if value was written successfully, -1 if there is an error
 * @author L.Piccoli
 */
int FcomChannelStates::write() {
  std::cout << "FcomChannelStates::write()" << std::endl;
    epicsTimeStamp timestamp;
    timestamp.nsec = 0;
    timestamp.secPastEpoch = 0;
    return write(timestamp);
}

/**
 * Write the current blob to the Fcom, using the specified timestamp.
 *
 * @param value timestamp
 * @return 0 if value was written successfully, -1 if there is an error
 * @author L.Piccoli
 */
int FcomChannelStates::write(epicsTimeStamp timestamp) {
    
    // Code common for Fcom channel instances
    _blob.fc_vers = FCOM_PROTO_VERSION;
    _blob.fc_tsHi = timestamp.secPastEpoch;
    _blob.fc_tsLo = timestamp.nsec;
    _blob.fc_stat = 0;

    // Set type and number of elements to the number of states
    _blob.fc_type = FCOM_EL_FLOAT;
    _blob.fc_nelm = MAX_STATES;

    // Code for this specific channel
    _blob.fc_idnt = _id;
    _blob.fc_flt = _blobData;
    int status = fcomPutBlob(&_blob);
    if (status != 0) {
        Log::getInstance() << "ERROR: " << fcomStrerror(status) << " "
                << _name.c_str() << Log::flush;
        _writeErrorCount++;
        return -1;
    }
    _writeCount++;

    _lastValue = _blobData[0];

    return 0;
}


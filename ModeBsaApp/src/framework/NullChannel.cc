/* 
 * File:   NullChannel.cc
 * Author: lpiccoli
 * 
 * Created on June 1, 2010, 10:39 AM
 */

#include "NullChannel.h"

USING_FF_NAMESPACE

NullChannel::NullChannel(CommunicationChannel::AccessType accessType,
                         bool readCounter) :
CommunicationChannel(accessType), _readCounter(readCounter), _counter(0) {
}

NullChannel::~NullChannel() {
}

/**
 * Read back a ZERO value.
 *
 * @param value fake value read from nowhere.
 * @return always returns 0
 * @author L.Piccoli
 */
int NullChannel::read(double &value, epicsTimeStamp &timestamp, double timeout) {
    value = _counter;

    timestamp.secPastEpoch = 0;
    timestamp.nsec = 0;

    if (_readCounter) {
        _counter = _counter + 1;
    }

    _readCount++;
    return 0;
}

/**
 * Write the specified value to nowhere.
 *
 * @param value new value to be thrown away
 * @return always return 0
 * @author L.Piccoli
 */
int NullChannel::write(double value) {
    _writeCount++;
    return 0;
}

int NullChannel::write(double value, epicsTimeStamp timestamp) {
    _writeCount++;
    return 0;
}

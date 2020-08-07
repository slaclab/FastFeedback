/* 
 * File:   CommunicationChannel.h
 * Author: lpiccoli
 *
 * Created on May 28, 2010, 12:09 PM
 */

#ifndef _COMMUNICATIONCHANNEL_H
#define	_COMMUNICATIONCHANNEL_H

#include <epicsTime.h>
#include <string>
#include <sstream>
#include "Defs.h"
#include "Exception.h"

FF_NAMESPACE_START

/**
 * This abstract class is intended to act as a wrapper for devices when reading
 * or writing out values.
 *
 * There should be two subclasses, one for dealing with FCOM communication and
 * another one to deal with EPICS communication.
 *
 * TODO: can EPICS/FCOM be abstracted with a single interface? Will that have
 * performance impact?
 */
class CommunicationChannel {
public:
    enum AccessType {
        READ_ONLY = 10,
        WRITE_ONLY = 20,
        NO_CHANNEL = 30
    };

    CommunicationChannel(AccessType accessType);
    virtual ~CommunicationChannel();

    virtual int read(double &value, epicsTimeStamp &timestamp, double timeout = 3.0) = 0;
    virtual int write(double vaule) = 0;
    virtual int write(double value, epicsTimeStamp timestamp) = 0;
    virtual std::string getName() {
      return "NONE";
    }

    long getReadCount() {
        return _readCount;
    }

    long getWriteCount() {
        return _writeCount;
    }

    long getReadErrorCount() {
        return _readErrorCount;
    }

    long getWriteErrorCount() {
        return _writeErrorCount;
    }

    long *getReadCountAddress() {
        return &_readCount;
    }

    long *getWriteCountAddress() {
        return &_writeCount;
    }

    long *getReadErrorCountAddress() {
        return &_readErrorCount;
    }

    long *getWriteErrorCountAddress() {
        return &_writeErrorCount;
    }

    double getLastValue() {
      return _lastValue;
    }

protected:
    /** Defines whether the communication channel is used for READ or WRITE */
    AccessType _accessType;

    /** Counts the number of successful reads */
    long _readCount;
    
    /** Counts the number of successful writes */
    long _writeCount;

    /** Count the number of errors while reading */
    long _readErrorCount;

    /** Count the number of errors while writing */
    long _writeErrorCount;

    /** Last value read/written using the channel */
    double _lastValue;

    std::stringstream _stringStream;
};

FF_NAMESPACE_END

#endif	/* _COMMUNICATIONCHANNEL_H */


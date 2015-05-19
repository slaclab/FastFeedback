/* 
 * File:   NullChannel.h
 * Author: lpiccoli
 *
 * Created on June 1, 2010, 10:39 AM
 */

#ifndef _NULLCHANNEL_H
#define	_NULLCHANNEL_H

#include "Defs.h"
#include "CommunicationChannel.h"

FF_NAMESPACE_START

/**
 * This class is for testing only. It provides a /dev/null source and
 * destination for device values.
 *
 * Optionally the read values are a counter starting at 0. Every time read()
 * is invoked the internal counter is returned and incremented.
 *
 * @author L.Piccoli
 */
class NullChannel : public CommunicationChannel {
public:
    NullChannel(AccessType accessType, bool readCounter = true);
    virtual ~NullChannel();
    virtual int read(double &value, epicsTimeStamp &timestamp, double timeout = 3.0);
    virtual int write(double value);
    virtual int write(double value, epicsTimeStamp timestamp);
private:
    /** Defines if read() returns a counter or not */
    bool _readCounter;

    /** Value returned if read counter is enabled */
    double _counter;
};

FF_NAMESPACE_END

#endif	/* _NULLCHANNEL_H */


/* 
 * File:   CaChannel.h
 * Author: lpiccoli
 *
 * Created on June 29, 2010, 1:23 PM
 */

#ifndef _CACHANNEL_H
#define	_CACHANNEL_H

#include <string>

#include "Defs.h"
#include "CommunicationChannel.h"
#include "TimeUtil.h"

#include <cadef.h>

FF_NAMESPACE_START

class CaChannelTest;

/**
 * Allows access to ChannelAccess PVs using the CommunicationChannel interface.
 *
 * @author L.Piccoli
 */
class CaChannel : public CommunicationChannel {
public:

    CaChannel(CommunicationChannel::AccessType accessType, std::string name);

    virtual ~CaChannel();

    virtual int read(double &value, epicsTimeStamp &timestamp, double timeout = 3.0);
    virtual int write(double value);
    virtual int write(double value, epicsTimeStamp timestamp);

    friend class CaChannelTest;

    virtual std::string getName() {
      return _name;
    }

private:
    /** PV Name */
    std::string _name;

    /** Channel Id, returned when connecting to the PV by name */
    chid _channelId;

 public:
    TimeAverage _readStats;
};

FF_NAMESPACE_END

#endif	/* _CACHANNEL_H */


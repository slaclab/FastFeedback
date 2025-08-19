/* 
 * File:   FcomChannelStates.h
 * Author: lpiccoli
 *
 * Created on July 2, 2010, 2:47 PM
 */

#ifndef _FCOMCHANNELSTATES_H
#define	_FCOMCHANNELSTATES_H

#include <string>
#include <fcom_api.h>
#include "Defs.h"
#include "FcomChannel.h"

FF_NAMESPACE_START

/**
 * This is a special class for handling States on FCOM. StateDevices for the
 * same loop share an instance of FcomChannelStates. After new values are
 * calculated, they are written to the same FcomBlob (in different offsets) and
 * sent one time.
 *
 * There can be up to 10 state values.
 *
 * Only WRITE operation is supported.
 *
 * @author L.Piccoli
 */
class FcomChannelStates : public FcomChannel {
public:

    FcomChannelStates(CommunicationChannel::AccessType accessType, std::string name,
            bool init = true);

    virtual ~FcomChannelStates();

    virtual int read(double &value, epicsTimeStamp &timestamp);
    virtual int write();
    virtual int write(epicsTimeStamp timestamp);
    virtual int write(float value, int stateIndex);

    int initialize();

private:
    /**
     * Blob containing state values
     */
    FcomBlob _blob;

    /**
     * Array containing states, which will be in the blob...
     */
    float _blobData[10];

    int readCaChannel(double &value, epicsTimeStamp &timestamp);

    static const int MAX_STATES = 10;
};

FF_NAMESPACE_END

#endif	/* _FCOMCHANNELSTATES_H */


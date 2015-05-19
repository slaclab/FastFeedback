/* 
 * File:   FcomChannel.h
 * Author: lpiccoli
 *
 * Created on July 2, 2010, 2:47 PM
 */

#ifndef _FCOMCHANNEL_H
#define	_FCOMCHANNEL_H

#include <string>
#include <fcom_api.h>
#include "Defs.h"
#include "CommunicationChannel.h"
#include "CaChannel.h"

FF_NAMESPACE_START

class FcomChannelTest;

/**
 * Allows access to FCOM using the CommunicationChannel interface.
 *
 * An actual FCOM blob does not have a one-to-one mapping with a device.
 * For example, the BPMs send X, Y and TMIT value within the same FCOM blob.
 *
 * Although X, Y and TMIT are represented in the FastFeedback as different
 * FcomChannels, the actual values come from the same place.
 *
 * If the FcomChannel is used for sending data there are also several use
 * cases.
 *
 * The simplest case is when an FcomChannel represents the connection to
 * a Magnet using a single POI.
 *
 * State values are also sent out using FcomChannels. States for a feedback
 * are all sent as an FcomGroup, which contains multiple blobs. Each blob
 * contains one state value (there may be up to 10 states).
 *
 * @author L.Piccoli
 */
class FcomChannel : public CommunicationChannel {
public:

    FcomChannel(CommunicationChannel::AccessType accessType, std::string name,
            bool init = true) throw (Exception);

    FcomChannel(FcomChannel &c) : CommunicationChannel(CommunicationChannel::READ_ONLY){
      std::cout << "Copy constructor for " << c._name << std::endl;
    }

    virtual ~FcomChannel();

    virtual int read(double &value, epicsTimeStamp &timestamp, double timeout = 3.0);
    virtual int write(double vaule);
    virtual int write(double value, epicsTimeStamp timestamp);

    friend class FcomChannelTest;

    enum FcomChannelType {
      ACTUATOR,
      BPM_X,
      BPM_Y,
      BPM_TMIT,
      STATE1,
      STATE2,
      STATE3,
      STATE4,
      STATE5,
      STATE6,
      STATE7,
      STATE8,
      STATE9,
      STATE10,
      BLEN_AIMAX,
      BLEN_BIMAX,
      STATES,
    };

    virtual std::string getName() {
      return _name;
    }

    int initialize();

protected:
    /**
     * Fcom device name -> actually this is the PV name that gets converted
     * to an FcomID
     */
    std::string _name;

    /** FcomID for this device. The ID in retrieved based on the device name */
    FcomID _id;

    /**
     * Defines whether the X, Y or TMIT values from the Fcom blob should
     * be used as the device value.
     */
    FcomChannelType _type;

    int readCaChannel(double &value, epicsTimeStamp &timestamp);

    int _counter;

    CaChannel *_readCaChannel;
};

FF_NAMESPACE_END

#endif	/* _FCOMCHANNEL_H */


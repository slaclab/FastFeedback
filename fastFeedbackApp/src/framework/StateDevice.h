/* 
 * File:   StateDevice.h
 * Author: lpiccoli
 *
 * Created on June 2, 2010, 1:42 PM
 */

#ifndef _STATEDEVICE_H
#define	_STATEDEVICE_H

#include "Defs.h"
#include "ActuatorDevice.h"
#include "SetpointDevice.h"
#include "FcomChannelStates.h"

class StateDeviceTest;

FF_NAMESPACE_START

/**
 * States are values calculated on every feedback iteration. Each loop can have
 * several calculated states.
 *
 * Each State has a desired Setpoint, which is represented as MeasurementDevice.
 *
 * The only allowed CommunicationChannel for States is FCOM, unlike Measurements
 * and Actuators.
 *
 * @author L.Piccoli
 */
class StateDevice : public ActuatorDevice {
public:
    StateDevice(std::string loopName, std::string name, int bufferSize,
            PatternMask patternMask = PatternMask::ZERO_PATTERN_MASK,
            int patternIndex = 1);

    virtual ~StateDevice() {
    };

    virtual int set(double value);
    virtual int set(double value, epicsTimeStamp timestamp);
    virtual int write();
    int writeFcom(epicsTimeStamp timestamp);
    virtual int getInitialSetting();
    virtual void show();
    virtual int configure(CommunicationChannel::AccessType accessType,
            int patternIndex = -1);

    double getSetpoint();

    double getError();
    void setSetpointDevice(SetpointDevice *setpoint);

    SetpointDevice *getSetpointDevice() {
        return _setpoint;
    }

    double getOffset();
    void setOffset(double offset);
    //    void setUsedBy(bool used);
    //    bool getUsedBy();

    friend class StateDeviceTest;

private:
    /** State setpoint */
    SetpointDevice *_setpoint;

    /** Difference between the calculated state and the setpoint */
    double _error;

    /**
     * Offset applied to the state for the specific pattern
     */
    double _offset;

    /**
     * State offset based on the pattern of interest (POI).
     */
    PvDataDouble *_offsetPv;

    /**
     * Maps to the PV that indicates whethes this state is in use by
     * the loop or not (used by the Longitudital feedback)
     */
    //    PvDataBool _usedByLoopPv;

    /**
     * Shared FcomCommunication channel between states from the same
     * feedback loop.
     */
    FcomChannelStates *_statesChannel;

    /**
     * State number - digit that follows the S in the PV name, e.g.:
     * State number is 5 for FBCK:FB01:TR01:S5
     */
    int _stateIndex;
};

FF_NAMESPACE_END

#endif	/* _STATEDEVICE_H */


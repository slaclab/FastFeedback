/* 
 * File:   ActuatorDevice.h
 * Author: lpiccoli
 *
 * Created on June 1, 2010, 2:02 PM
 */

#ifndef _ACTUATORDEVICE_H
#define	_ACTUATORDEVICE_H

#include <string>
#include <epicsTime.h>
#include "Defs.h"
#include "Device.h"
#include "CommunicationChannel.h"
#include "PvData.h"

class ActuatorDeviceTest;

FF_NAMESPACE_START

/**
 * Implements a device that can be set only. Any changes to a device are first
 * made via the set() method.
 *
 * Set() first copies the desired value into the next DataPoint in the internal
 * buffer.
 *
 * Once in the buffer the value may be sent to the actual device using the
 * write() method.
 *
 * During normal usage the method set() is first called followed by write(),
 * i.e. first the actuator setpoint is calculated and then the result is
 * sent out using the specified communication channel. However it is possible
 * that set() is called many times before write() is called. In such case
 * the ActuatorDevice must track down which DataPoints have actually been
 * written.
 *
 * The implementation allows set() to be called several times without the
 * subsequent write(). However at some point the internal circular buffer will
 * wrap and older values will be lost. Next time the write() method is invoked
 * the oldest 'surviving' set value will be sent to the actuator.
 *
 * @author L.Piccoli
 */
class ActuatorDevice : public Device {
public:
    ActuatorDevice(std::string loopName, std::string name, int bufferSize,
            PatternMask patternMask = PatternMask::ZERO_PATTERN_MASK,
            int patternIndex = 1);
    virtual ~ActuatorDevice();

    virtual int set(double value);
    virtual int set(double value, epicsTimeStamp timestamp);
    int set(double value, DataPoint::Status status);
    void setGroupLimit();
    virtual int write(bool send = true);
    virtual void clear();
    virtual double peek();
    virtual int peek(DataPoint &dataPoint);
    virtual int peek(DataPoint &dataPoint, int pos);
    virtual epicsUInt32 peekPulseId();
    virtual DataPoint::Status peekStatus();
    virtual int configure(CommunicationChannel::AccessType accessType,
            int patternIndex = -1);
    virtual int getInitialSetting();
    virtual void show();
    void getInitialSettingFromReference(bool writeValue, epicsTimeStamp timestamp);
    void setInitialOffset();
    void zeroOffset();
    //    void setInitialChannel(CommunicationChannel *initialChannel);
    void setInitialChannel(PvData<double> *initialChannel);
    
    void setLimits(double high, double low);

    double getLastValueSet() {
        return _lastValueSet;
    }

    friend class ActuatorDeviceTest;

    /** Unsupported method */
    virtual int get(double &value, epicsTimeStamp &timestamp) {
        return -1;
    };

    /** Unsupported method */
    virtual int read() {
        return -1;
    }

    void setReferenceActuator(ActuatorDevice *referenceActuator) {
        _referenceActuator = referenceActuator;
    }

protected:
    int write(double value);

    /** Points to the next DataPoint that should be written to device */
    int _nextWrite;

    /** Count the number of DataPoints that were lost due to buffer overflow */
    int _droppedPoints;

    /**
     * Keep the last value sent to the actuator in a separate attribute,
     * so when an Algorithm needs to use the previously sent value it reads
     * _lastValueSet instead of searching for it in the internal buffer.
     */
    double _lastValueSet;

    /**
     * This attribute is used when there are multiple patterns configured.
     * This reference contains the values sent to the P1 actuator. It is
     * used to keep track of the offsets between actuators at P1 and P2/P3/P4.
     */
    ActuatorDevice *_referenceActuator;

    /**
     * If not NULL this channel provides the initial actuator setting. This
     * allows the actuator to start from a value given by a readback PV
     * E.g. In the BunchCharge feedback the initial laser power is given
     * by the IOC:IN20:LS11:LASER_PWR_READBACK, while the actuator is
     * the PV IOC:IN20:LS11:PCTRL.
     */
    //    CommunicationChannel *_initialChannel;
    PvData<double> *_initialChannel;

    /**
     * Difference between the last value set for this actuator and the reference
     * actuator (if there is one defined).
     */
    double _referenceOffset;

    /**
     * Alarm high limit setting PV. Setting this device forces the Ax.HIHI field
     * to be changed to the value of _hihiInPv.
     *
     * This attribute maps to the $(LOOP):<dev>HIHIIN PV
     */
    PvData<double> _hihiInPv;

    /**
     * Warning high limit setting PV. Setting this device forces the Ax.HIGH field
     * to be changed to the value of _highInPv.
     *
     * This attribute maps to the $(LOOP):<dev>HIGHIN PV
     */
    PvData<double> _highInPv;

    /**
     * Warning low limit setting PV. Setting this device forces the Ax.LOW field
     * to be changed to the value of _lowInPv.
     *
     * This attribute maps to the $(LOOP):<dev>LOWIN PV
     */
    PvData<double> _lowInPv;

    /**
     * Alarm low limit setting PV. Setting this device forces the Ax.HIHI field
     * to be changed to the value of _loloInPv.
     *
     * This attribute maps to the $(LOOP):<dev>LOLOIN PV
     */
    PvData<double> _loloInPv;
};

FF_NAMESPACE_END

#endif	/* _ACTUATORDEVICE_H */


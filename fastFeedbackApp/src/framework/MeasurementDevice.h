/* 
 * File:   MeasurementDevice.h
 * Author: lpiccoli
 *
 * Created on June 1, 2010, 4:07 PM
 */

#ifndef _MEASUREMENTDEVICE_H
#define	_MEASUREMENTDEVICE_H

#include <string>
#include <epicsTime.h>
#include "Pattern.h"
#include "Defs.h"
#include "Device.h"

class MeasurementDeviceTest;

FF_NAMESPACE_START

/**
 * Implements a device that can be get only. Any new readings for the device
 * are retrieved using the read() method.
 *
 * Read() first uses the communication channel to retrieve the latest available
 * reading of the device and store it in the next DataPoint in the internal
 * buffer.
 *
 * Once in the buffer the value may be read by MeasurementDevice users
 * using the get() method.
 *
 * During normal usage the method read() is first called followed by get(),
 * i.e. first the measurement reading is retrieved from the actual device using
 * the specified communication channel, making it available for users of the
 * MeasurementDevice to get the new value. However it is possible that read()
 * is called many times before get() is called. In such case the
 * MeasurementDevice must track down which DataPoints have actually been read.
 *
 * If the MeasurementDevice is a BPM (name starts with BPMS:) then a communication
 * channel is created to get the TMIT value. The value is updated whenever
 * a new measurement is read.
 *
 * @author L.Piccoli
 */
class MeasurementDevice : public Device {
public:
    MeasurementDevice(std::string loopName, std::string name, int bufferSize,
            PatternMask = PatternMask::ZERO_PATTERN_MASK, int patternIndex = 1);
    virtual ~MeasurementDevice();

    virtual int get(double &value, epicsTimeStamp &timestamp);
    virtual int read();
    virtual void clear();
    virtual int configure(CommunicationChannel::AccessType accessType,
            int patternIndex = -1);
    virtual void disconnect();

    virtual double peek();
    virtual epicsUInt32 peekTsH();
    virtual epicsUInt32 peekTsL();
    virtual epicsUInt32 peekTsHLast();
    virtual epicsUInt32 peekTsLLast();
    virtual int peek(DataPoint &dataPoint);
    virtual int peek(DataPoint &dataPoint, int pos);
    virtual DataPoint::Status peekStatus();
    virtual epicsUInt32 peekPulseId();

    virtual void show();

    bool checkPulseId(epicsUInt32 patternPulseId);
    bool checkTimestamp(epicsTimeStamp ts);
    bool checkTimestampChange();

    epicsTimeStamp getT0();

    int insert(double value, epicsTimeStamp timestamp);
    int replace(double value);

    void setAverageCount(int averageCount);
    double getAverage();
    void resetNextRead();

    friend class MeasurementDeviceTest;

    double getTmit() {
        return _tmit;
    }

/* Since float and double are not of integral or enumeration type, 
 * such members must either be constexpr, or non-static in order 
 * for the initializer in the class definition to be permitted.
 */

#if __GNUC__ > 5
 #define GNU_CONST_STATIC_FLOAT_DECLARATION constexpr
#else
 #define GNU_CONST_STATIC_FLOAT_DECLARATION const
#endif

   GNU_CONST_STATIC_FLOAT_DECLARATION static double LOW_TMIT = 1e7;

    /** Unsupported method */
    virtual int set(double value) {
        return -1;
    }

    /** Unsupported method */
    virtual int write() {
        return -1;
    }

    bool isBpm();
    bool isBlen();
    bool isTmit();

private:
    /** Points to the next DataPoint that should be read by users using get() */
    int _nextRead;

    /**
     * Count the number of DataPoints that were lost due to buffer overflow.
     * This happens when read() is called enough times to fill up the buffer.
     * If the circular buffer wraps around the read values start being 
     * overwritten.
     */
    int _droppedPoints;

    /** Number of DataPoints used to calculate the measurement average */
    int _averageCount;

    /** Average measurement based on the last _averageCount read measurements */
    double _average;

    /**
     * If the measurement device is a BPM then the TMIT is read into this
     * attribute using the proper communication channel. The TMIT value is
     * retrieved using the same type of communication channel used by the
     * measurement device.
     */
    double _tmit;

    /** Counter of number of measurements with low TMIT */
    long _lowTmitCount;

    long _timestampBehind;
    long _timestampAhead;

    /**
     * CommunicationChannel used to read the TMIT value if measurement device
     * is a BPM.
     */
    CommunicationChannel *_tmitCommunicationChannel;

    /**
     * Used to keep the number of timestamp mismatchs if data is received
     * using an FcomChannel.
     */
    PvData<long> _timestampMismatchCountPv;

    /** Counter of how many times checkPulseId couldn't check because of wrong data status */
    unsigned long _checkFailCount;

    /** Count number of times method read() was called */
    unsigned long _readCount;

    /** Count number of times method get() was called */
    unsigned long _getCount;

    bool _isBpm;
    bool _isBlen;
    bool _isTmit;

    bool hasBpmName();
    bool hasBlenName();
    bool hasTmitName();

    bool hasStringInName(std::string str);
};

FF_NAMESPACE_END

#endif	/* _MEASUREMENTDEVICE_H */


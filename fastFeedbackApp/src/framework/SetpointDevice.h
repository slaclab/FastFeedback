/* 
 * File:   SetpointDevice.h
 * Author: lpiccoli
 *
 * Created on June 1, 2010, 4:07 PM
 */

#ifndef _SETPOINTDEVICE_H
#define	_SETPOINTDEVICE_H

#include <string>
#include <epicsTime.h>
#include "Pattern.h"
#include "Defs.h"
#include "Device.h"
#include "PvData.h"

class SetpointDeviceTest;

FF_NAMESPACE_START

/**
 * Implements special kind of input device whose data comes from a PV using
 * device support. The read() method is not defined in this class, only the
 * get() method.
 *
 * @param name setpoint PV name, which must be hooked up via device support
 * @author L.Piccoli
 */
class SetpointDevice : public Device {
public:
    SetpointDevice(std::string loopName, std::string name, int bufferSize,
            PatternMask = PatternMask::ZERO_PATTERN_MASK, int patternIndex = 1);
    virtual ~SetpointDevice();

    virtual int get(double &value, epicsTimeStamp &timestamp);

    virtual double peek();
    virtual int peek(DataPoint &dataPoint, int pos);
    virtual int peek(DataPoint &dataPoint);

    virtual void show();

    friend class SetpointDeviceTest;

    /** Unsupported method */
    virtual int read() {
        return -1;
    }

    /** Unsupported method */
    virtual int set(double value) {
        return -1;
    }

    /** Unsupported method */
    virtual int write() {
        return -1;
    }

private:
    /**
     * This is the source of the setpoint values. It attaches to
     * the $(LOOP) SxxDES<POI> PVs
     */
    PvDataDouble _setpointPv;
};

FF_NAMESPACE_END

#endif	/* _SetpointDEVICE_H */


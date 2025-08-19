/* 
 * File:   DeviceView.h
 * Author: lpiccoli
 *
 * Created on June 3, 2010, 5:05 PM
 */

#ifndef _DEVICEVIEW_H
#define	_DEVICEVIEW_H

#include <vector>
#include <string>
#include "Defs.h"
#include "Device.h"
#include "Pattern.h"

class DeviceViewTest;

FF_NAMESPACE_START

#define CIRCULAR_INCREMENT(X) (X + 1) % _bufferSize
#define CIRCULAR_DECREMENT(X) (_bufferSize + X - 1) % _bufferSize

/**
 * The DeviceView class allows Devices to have their values seen by external
 * EPICS PVs.
 *
 * A DeviceView automatically defines EPICS PVs for a Device, naming convention
 * require the following parameters to be specified:
 *
 *   xx = 1..N (index)
 *   y = device type [A,M,S] (type)
 *   z = suffix (for setpoints only) (suffix)
 *
 * These are the PVs managed by the DeviceView:
 *
 *   yxxz: single device value (double)
 *   yxxzHST: value history (double[])
 *   yxxz_S: single device value at 1 Hz (double)
 *   yxxzHST_S: value history at 1 Hz (double[])
 *
 * Alarms for the PV yxxz are configurable through these PVs:
 *
 *   yxxHIHI
 *   yxxHIGH
 *   yxxLOW
 *   yxxLOLO
 *
 * Multiple POIs: A DeviceView may monitor multiple devices, each one responding
 * to a different POI. If the DeviceView has multiple devices, the update()
 * method gets the latest value only for that device whose POI matches the
 * current POI.
 *
 * @author L.Piccoli
 */
class DeviceView {
public:
    DeviceView(int bufferSize, int index, char type, std::string suffix = "",
	       std::string loopName = "", int patternIndex = 0);
    virtual ~DeviceView();

    int add(Device *device);
    int update(Pattern evrPattern);
    int updateSlow();
    int getLoopIndex() {
        return _loopIndex;
    }
    std::string getLoopName() {
        return _loopName;
    }
    double get();
    int getDeviceCount() {
        return _devices.size();
    }

    friend class DeviceViewTest;

private:
    /**
     * List of devices monitored by this view. Usually there is one device
     * per view. However for views on multiple patterns there is one device
     * per pattern.
     */
    std::vector<Device *> _devices;

    /**
     * PV index for this view. The index is part of the PV name, e.g.
     * M5 name is for the feedback Measurement number 5.
     */
    int _index;

    /**
     * Type of device being viewed: 'A' for Actuator, 'M' for Measurement and
     * 'S' for State.
     */
    char _type;

    /**
     * Suffix used to decorate PV name - used only for setpoints, whose value
     * is "DES", e.g. S1DES
     */
    std::string _suffix;

    /** Buffer containing history of device values */
    double *_buffer;

    /** Points to the next entry in the history buffer */
    int _next;

    /** Buffer containing history of device values @ 1 Hz */
    double *_slowBuffer;

    /** Points to the next entry in the slow history buffer */
    int _slowNext;

    /** Size of the history buffers */
    int _bufferSize;

    /**
     * Loop index -> used by the Device Support code. This is the index
     * used for the device scan.
     */
    int _loopIndex;
    std::string _loopName;

    /**
     * Defines which pattern this view is watching (0 for all,
     * 1/2/3/4 for specific data slots)
     */
    int _patternIndex;
};

FF_NAMESPACE_END

#endif	/* _DEVICEVIEW_H */


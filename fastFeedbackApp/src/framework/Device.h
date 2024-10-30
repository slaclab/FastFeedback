/* 
 * File:   Device.h
 * Author: lpiccoli
 *
 * Created on May 28, 2010, 11:38 AM
 */

#ifndef _DEVICE_H
#define	_DEVICE_H

#include <string>
#include <sstream>
#include <epicsTime.h>
#include "Log.h"
#include "Defs.h"
#include "PatternMask.h"
#include "CommunicationChannel.h"
#include "PvData.h"
#include "Exception.h"

FF_NAMESPACE_START

/**
 * This struct defines one entry in the Device buffer that holds
 * either actuator/state data to be sent out or data just read in from 
 * a measurement/setpoint device.
 * 
 * @author L.Piccoli
 */
typedef struct {

    /**
     * Definition of Status for a DataPoint:
     * EMPTY - not used
     * SET - indicates point has been added to the buffer, not yet written out
     * SET_LIMIT - value set is either below lolo limit or above hihi limit
     * SET_GROUP_LIMIT - value of other device in group is beyond limits
     * SET_SKIP - value added should not be written out
     * GET - point has been read out from the buffer
     * GET_SKIP - data was not used because either its value is beyond limits,
     * its timestamp is not valid, or forced to be skipped because other external
     * factors (e.g. other data points in same feedback loop were invalid).
     * WRITE - data has been written out using the communication channel
     * WRITE_SKIP - data has not been written out because value is beyond limits
     * or data was valid but forced to be skipped
     * WRITE_INVALID - failed to write to communication channel
     * WRITE_NAN - Tried to write a nan value
     * READ - data has been read in using the communication channel
     * READ_LIMIT - data has been read in using the communication channel and
     * value is beyond limits.
     * READ_INVALID - failed to read from communication channel
     * READ_TSMISMATCH - PulseId of the value read disagrees with PulseId from
     * the EVR pattern
     *
     * TODO: write a state diagram for values of status
     */
    enum Status {
        EMPTY = 5,
        SET = 10,
        SET_LIMIT = 11,
        SET_GROUP_LIMIT = 12,
	SET_SKIP = 13,
        GET = 20,
        GET_SKIP = 21,
        WRITE = 30,
        WRITE_SKIP = 31,
        WRITE_INVALID = 32,
	WRITE_NAN = 33,
        READ = 40,
        READ_LIMIT = 41,
        READ_INVALID = 42,
        READ_TSMISMATCH = 43,
    };

    /** The actual device value */
    double _value;

    /** Timestamp of the reading or setting */
    epicsTimeStamp _timestamp;

    /** Indicates if current entry has been committed or not */
    Status _status;
} DataPoint;

#define CIRCULAR_INCREMENT(X) (X + 1) % _bufferSize
#define CIRCULAR_DECREMENT(X) (_bufferSize + X - 1) % _bufferSize

/**
 * This class defines common behavior for Measurements, Actuators, States and
 * Setpoint States. It also contains the internal buffer where data points are
 * kept for use of the feedback system.
 *
 * The Device together with the CommunicationChannel classes allow data points
 * to be read from/write to Channel Access or FCOM.
 *
 * The get() and set() methods are used to access the internal buffer,
 * while the read() and write() methods are used to exchange data through the
 * proper CommunicationChannel (either FCOM, EPICS or File).
 *
 * A Device can be used for reading only or writing only, i.e. either the
 * set()/write() or the get()/read() operations are allowed. The Device is
 * defined to be READ or WRITE at construction time. (READ is the default
 * behavior).
 *
 * After a value is read or before a value is written the limits are checked.
 * If the value is below or above the specified limits an error is generated.
 *
 * The internal structure that temporarily holds data before it is sent to
 * actuators or read by the feedback algorithms is a circular buffer. The head
 * of the buffer contains the last value read using get() or writen using set().
 *
 * Subclasses of Device are: ActuatorDevice, MeasurementDevice and
 * StateDevice.
 *
 * Multiple Loops: A device *must not* be used by multiple feedback loops. A
 * new device with the same device name must be created.
 *
 * @author L.Piccoli
 */
class Device {
public:
    Device(std::string slotName, std::string name, int bufferSize,
            PatternMask patternMask = PatternMask::ZERO_PATTERN_MASK,
            int patternIndex = 1);
    //    Device(const Device& orig);
    virtual ~Device();

    virtual int set(double value) {
        return -1;
    };

    virtual int set(double value, epicsTimeStamp timestamp) {
        return -1;
    };

    virtual int get(double &value, epicsTimeStamp &timestamp) {
        return -1;
    };

    virtual int read() {
        return -1;
    };

    virtual int write(bool send = true) {
        return -1;
    };

    virtual double peek() {
        return -1;
    };

    virtual int peek(DataPoint &dataPoint) {
        return -1;
    }

    virtual int peek(DataPoint &dataPoint, int pos) {
        return -1;
    }

    virtual DataPoint::Status peekStatus() {
        return DataPoint::EMPTY;
    }

    virtual epicsUInt32 peekPulseId() {
        return 0;
    }
    virtual void clear();

    bool isUsed() {
        return _usedPv.getValue();
    };
    bool isFile();
    bool isFcom();
    bool isNull();
    virtual int configure(CommunicationChannel::AccessType accessType,
            int patternIndex = -1);
    virtual void disconnect();

    void setCommunicationChannel(CommunicationChannel *newChannel);
    bool hasCommunicationChannel() {
        if (_communicationChannel == NULL) {
            return false;
        }
        else {
            return true;
        }
    }
    void setPatternMask(PatternMask patternMask);
    PatternMask getPatternMask();
    virtual void show();
    int setFeedbackPv(bool state);
    bool getFeedbackPv();

    bool operator==(const Device &other);
    bool operator!=(const Device &other);
    bool operator<(const Device &other);

    std::string getName() {
        return _name;
    };
    std::string getDeviceName();
    bool getLongType();
    int getDeviceIndex();
    
    int getBufferSize() {
        return _bufferSize;
    };

    CommunicationChannel *getCommunicationChannel() {
        return _communicationChannel;
    };

    void setUsedBy(bool used);
    bool getUsedBy();

    double getHihi() {
      return _hihiPv.getValue();
    }

    double getLolo() {
      return _loloPv.getValue();
    }

    std::string getDeviceFileName();

    bool isSetFbckPvEnabled();

protected:
    /** Slot name (TR01, TR02, ...) */
    std::string _slotName;

    /** Device name (A1, A2, S1, M2, ...) */
    std::string _name;

    /** Internal device buffer */
    DataPoint *_buffer;

    /** Size of the internal buffer */
    int _bufferSize;

    /** Point to the next DataPoint to be used */
    int _next;

    /** CommunicationChannel used to read/write */
    CommunicationChannel *_communicationChannel;

    /** Pattern mask of interest for this device */
    PatternMask _patternMask;

    /**
     * Index of the _patternMask on the feedback loop where it is being
     * used. The index may vary from 1 to 4 (system currently supports up
     * to 4 patterns).
     */
    int _patternIndex;

    /**
     * Alarm high limit.
     *
     * This attribute maps to the $(LOOP):<dev>HIHI PV
     */
    PvData<double> _hihiPv;

    /**
     * Warning high limit.
     *
     * This attribute maps to the $(LOOP):<dev>HIGH PV
     */
    PvData<double> _highPv;

    /**
     * Warning low limit.
     *
     * This attribute maps to the $(LOOP):<dev>LOW PV
     */
    PvData<double> _lowPv;

    /**
     * Alarm low limit.
     *
     * This attribute maps to the $(LOOP):<dev>LOLO PV
     */
    PvData<double> _loloPv;

    /**
     * Defines whether the device should use CaChannel (ChannelAccess) or
     * FcomChannel (FCOM) to communicate with the real device.
     *
     * true = CaChannel (default in constructor)
     * false = FcomChannel
     *
     * This attribute maps into the $(LOOP):<dev>CAMODE PV.
     */
    PvData<bool> _caModePv;

    /**
     * Defines whether the device should be used in the Feedback Loop or not.
     * The LoopThread must check this attribute for every Device when the
     * Loop is enabled (STATE PV).
     *
     * MeasurementDevices defined as not used should not be updated by the
     * CollectorThread.
     *
     * true = the Device should be used in the Loop (default in constructor)
     * false = ignore the Device
     *
     * This attribute maps into the $(LOOP):<dev>USED PV.
     */
    PvData<bool> _usedPv;

    /**
     * User assigned name for the device.
     * 
     * This attribute maps to the $(LOOP):<dev>DEVNAME PV.
     */
    PvData<std::string> _devNamePv;

    PvData<bool> _longTypePv;

    /**
     * CommunicationChannel used to set the FBCK attribute of Measurements and
     * Actuators.
     *
     * FBCK is always OFF, except when the loop state is ON and mode ENABLED.
     * Actually, it is sufficient to turn FBCK ON when the loop state is ON
     * and mode is ENABLED and *not* turn it back off.
     *
     * There is an external application that is watching the FBCK PVs and turning
     * them off as needed.
     *
     * 0 -> FBCK OFF
     * 1 -> FBCK ON
     *
     * The _fbckCommunicationChannel is always in instance of CaCommunicationChannel,
     * which sets the data type to DBR_DOUBLE -> the FBCK PV is of type DBR_SHORT.
     * TODO: This could be a problem... DBR_DOUBLE -> DBR_SHORT
     */
    CommunicationChannel *_fbckCommunicationChannel;

    /**
     * Defines whether to set the FBCK PV when using the device or not
     */
    bool _setFbckPv;

    /**
     * Maps to the PV that indicates whethes this state is in use by
     * the loop or not (used by the Longitudital feedback)
     */
    PvData<bool> _usedByLoopPv;
    bool _usedByLoop;

    /**
     * Used to keep the number of successful read/write operations on the device
     */
    PvData<long> _channelCountPv;

    /**
     * Used to keep the number of errors from read/write operations.
     */
    PvData<long> _channelErrorCountPv;

    std::stringstream _stringStream;

    /**
     * Tells whether the device name contains the "FILE:" string.
     */
    bool _isFile;

    int createFileChannel(std::string name, int patternIndex,
			  CommunicationChannel::AccessType accessType);
    std::string getFileChannelName(std::string fileName, int patternIndex);
    int createFbckPv(CommunicationChannel::AccessType accessType);
};

struct DeviceCompare {
    bool operator() (Device *lhs, Device * rhs) const {
      return *lhs<*rhs;
    }
};

struct DevicePvNameCompare {
    bool operator() (Device *lhs, Device * rhs) const {
        return lhs->getDeviceName() < rhs->getDeviceName();
    }
};

FF_NAMESPACE_END

#endif	/* _DEVICE_H */


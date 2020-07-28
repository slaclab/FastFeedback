/* 
 * File:   Loop.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 3:29 PM
 */

#ifndef _LOOP_H
#define	_LOOP_H

#include <string>
#include <epicsTime.h>
#include "Pattern.h"
#include "PatternMask.h"
#include "LoopConfiguration.h"
#include "Algorithm.h"
#include "CommunicationChannel.h"
#include "Defs.h"
#include "TimeUtil.h"

class LoopTest;

FF_NAMESPACE_START

/**
 * The Loop coordinates the feedback calculation for a specific pattern mask.
 * For the majority of feedback loops there will be only one Loop. Feedbacks
 * that use multiple pattern masks use multiple Loops.
 *
 * Loops are invoked by a LoopThread when a PATTERN_EVENT and its related
 * MEASUREMENT_EVENT have been received and checked.
 *
 * @author L.Piccoli
 */
class Loop {
public:
    Loop(LoopConfiguration *configuration, PatternMask patternMask,
            Algorithm *algorithm);
    Loop(LoopConfiguration *configuration, PatternMask patternMask);

    virtual ~Loop();

    int calculate(Pattern pattern);

    PatternMask getPatternMask() {
        return _patternMask;
    };

    void setPatternMask(PatternMask patternMask) {
        _patternMask = patternMask;
    };
    int reconfigure(PatternMask patternMask);
    int resetActuators();
    int resetActuatorOffsets();
    int resetAlgorithm();
    void show();
    void showDebug();
    int setFeedbackDevices(bool measurements, bool actuators);
    void disconnectDevices();
    void checkFcom();

    friend class LoopTest;

private:
    int configureAlgorithm();
    int setDevices(bool skip = false);
    int checkTmit(Pattern &pattern);
    int checkMeasurementStatus(epicsUInt32 patternPulseId);
    void discardLatestMeasurements();
    void resetMeasurements();

    template<class DeviceMap, class DeviceMapIterator,
    class DeviceSet, class DeviceSetIterator>
      int selectUsedDevices(DeviceSet &deviceSet, DeviceMap &deviceMap,
			    CommunicationChannel::AccessType accessType, long maxDevices);

    template<class DeviceSet, class DeviceSetIterator>
      int configureUsedDevices(DeviceSet &deviceSet,
			       CommunicationChannel::AccessType accessType);

    template<class DeviceSet, class DeviceSetIterator>
      int clearUsedDevices(DeviceSet &deviceSet);

    template<class Device, class DeviceSet, class DeviceSetIterator>
      void setGroupLimit(DeviceSet &deviceSet);

    void setActuatorsReference();

    /**
     * This is the configuration for the Loop. It is shared among the Loops
     * of the same LoopThread, in case of multiple pattern masks
     */
    LoopConfiguration *_configuration;

    /** Pattern mask related to this Loop */
    PatternMask _patternMask;

    /** Algorithm used to calculate the feedback */
    Algorithm *_algorithm;

    /** Set of measurement devices used by this loop */
    MeasurementSet _measurements;

    /** Set of actuators used by this loop */
    //    ActuatorSet *_actuators;
    ActuatorSet _actuators;

    /** Set of states used by this loop */
    //    StateSet *_states;
    StateSet _states;

    /** Count of how many times calculate() has been called successfully */
    unsigned long _calculateCount;

    /** Count of how many times calculate() has been called unsuccessfully */
    unsigned long _calculateFailCount;

    /** Count of how many times calculate() failed to set devices */
    unsigned long _deviceFailCount;

    /**
     * Count of how many times at least one calculated actuators is outside
     * the limits
     */
    unsigned long _actuatorsOutsideLimitsCount;

    /** Count of how many times at least one measurement is outside the limits */
    unsigned long _measurementsOutsideLimitsCount;

    /** Count number of times actuators were *not* set -> MODE == Compute */
    unsigned long _actuatorsSkipCount;

    /** Count the number of cycles skiped because of low TMIT */
    unsigned long _lowTmitCount;

    /** Count number of cycles with PulseId mismatches */
    unsigned long _pulseIdMismatchCount;

    unsigned long _measBadStatus;

    /** Count number of PulseId between the current pattern and the actuator PulseId */
    unsigned long _actuatorMismatchCount;

    /** Statistics about the time consumed by the Loop->calculate() operation */
    TimeAverage _calculatePrepStats;
    TimeAverage _tmitCheckStats;
    TimeAverage _measCheckStats;
    TimeAverage _calculateStats;
    TimeAverage _calculateFailStats;

    /** Skip TMIT check - if measurements are coming from files */
    bool _skipTmit;

    /**
     * Indicates if calculate() is being called for the first time. In that case
     * the MeasurementDevices have stale read values discarded. This is to insure
     * calculate() starts reading the latest available measurements.
     */
    bool _firstPass;

    /**
     * Defines whether the last loop calculation had beam or not (pockcel_perm
     * present).
     */
    bool _previousHadBeam;

    char _timeString[256];
    epicsTimeStamp _lastActuatorTimestamp;
};

FF_NAMESPACE_END

#endif	/* _LOOP_H */


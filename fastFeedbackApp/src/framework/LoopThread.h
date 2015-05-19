/* 
 * File:   LoopThread.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 2:13 PM
 */

#ifndef _LOOPTHREAD_H
#define	_LOOPTHREAD_H

#include <string>
#include "Thread.h"
#include "LoopConfiguration.h"
#include "Pattern.h"
#include "Loop.h"
#include "Defs.h"
#include "PvData.h"
#include "TimeUtil.h"
#include "EventLogger.h"

class LoopThreadTest;
class ExecThreadTest;

FF_NAMESPACE_START

/**
 * LoopThread is the active entity for a feedback. Within an IOC there may be
 * multiple LoopThreads running. The priorities must be adjusted accordingly
 * by the ExecThread.
 *
 * In the event loop the LoopThread expects for a PATTERN_EVENT. If the pattern
 * is of interest then the next event to be received must be a MEASUREMENT_EVENT.
 *
 * @author L.Piccoli
 */
class LoopThread : public Thread {
public:

    /**
     * Enumeration of possible states for the LoopThread. It is usually
     * waiting for a Pattern or waiting for a Measurement at any given time.
     */
    enum State {
        IDLE = 25,
        WAITING_PATTERN = 35,
        WAITING_MEASUREMENT = 45
    };

    LoopThread(const std::string name, LoopConfiguration *configuration);
    virtual ~LoopThread();

    int configure();
    void show();
    void showConfig();
    virtual void showDebug();

    void enable() {
        _enabledPv = true;
    };

    void disable() {
        _enabledPv = false;
    }

    bool isConfigured() {
        return _configuration->_configured;
    }

    /**
     * Returns whether the LoopThread is enabled or not, i.e.
     * $(LOOP):STATE is ON or OFF.
     *
     * @return true if STATE is ON, false if STATE is OFF
     * @author L.Piccoli
     */
    bool isOn() {
        return _enabledPv == true;
    }

    /**
     * Returns whether the LoopThred is only computing values (false)
     * or if it is actually sending out actuator updates (true)
     * This is the value of the $(LOOP):MODE PV.
     *
     * @return true if MODE is ENABLED, false if MODE is COMPUTE
     * @author L.Piccoli
     */
    bool isActive() {
        return _modePv == true;
    }

    /**
     * Return the measured loop running rate.
     *
     * @return running rate
     * @author L.Piccoli
     */
    float getRate() {
      return _loopRateStats.getRate();
    }

    friend class LoopThreadTest;
    friend class ExecThreadTest;

protected:
    virtual int processEvent(Event &event);

private:
    /*
    LoopThread() : Thread("TestOnly"),
      _enabledPv("", false),//, this, RECONFIGURE_EVENT),
      _modePv("", false) {//, this, MODE_EVENT) {
    }; // Private constructor for tests only
    */
    int createLoops();
    int processPatternEvent(Event &event);
    int processMeasurementEvent(Event &event);

    int changeMode(bool statePv, bool modePv);
    int reconfigure(bool statePv, bool modePv);
    int disconnectDevices(bool turnFeedbackOff = true);
    int resetActuators();

    int checkMode();

    /** LoopThread state, either WAITING_PATTERN or WAITING_MEASUREMENT */
    State _state;

    /** Loop configuration */
    LoopConfiguration *_configuration;

    /** Current pattern, received from a PATTERN_EVENT */
    Pattern _pattern;

    /** Loops, there is one per each pattern */
    std::vector<Loop *> _loops;

    /** Pointer to the first Loop (used to set the FBCK PVs) */
    Loop *_firstLoop;

    /**
     * Defines whether the LoopThread is active or not. When _enable changes
     * between OFF -> ON then a new loop configuration is loaded if 
     * the INSTALLED PV (_configuration->_installed) is set to true.
     *
     * This attribute maps into the $(LOOP):STATE PV
     */
    PvDataBool _enabledPv;

    /** Holds the current STATE of the loop (true for ON, false for OFF) */
    PvDataBool _stateActPv;

    /**
     * Holds the previous STATE of the loop. This is used to find the
     * transition between ON/OFF & OFF/ON.
     */
    bool _previousState;

    /**
     * Defines whether the Loop is in compute only mode (i.e. all actuator
     * and state values are computed, but not set) or values are send out
     * to actuators.
     *
     * 0 -> COMPUTE
     * 1 -> ENABLE
     *
     * This attribute maps to the $(LOOP):MODE PV.
     */
    PvDataBool _modePv;

    /** Holds the current MODE of the loop (true for ENABLED, false for COMPUTE) */
    PvDataBool _modeActPv;

    /**
     * Holds the previous MODE of the loop. This is used to find the
     * transition between ENABLED/COMPUTE & COMPUTE/ENABLED.
     */
    bool _previousMode;

    /** Number of correct PATTERN_EVENTS received */
    unsigned long _patternCount;

    /** Number of correct MEASUREMENT_EVENTS received */
    unsigned long _measurementCount;

    /**
     * Counter of how many unexpected patterns were received. Unexpected patterns
     * are those known patterns (i.e. in the list of patterns of the configuration)
     * that are received while the LoopThread is in WAITING_MEASUREMENT state.
     */
    unsigned long _unexpectedPatternCount;

    /**
     * Counter of how many unknown patterns were received. Unknown patterns are
     * those that are not in the list of patterns of the configuration.
     * An unknown pattern received while the LoopThread is in WAITING_MEASUREMENT
     * is also counted as an unexpected pattern.
     */
    unsigned long _unknownPatternCount;

    /**
     * Counter of how many MEASUREMENT_EVENTS were received while the LoopThread
     * was in the WAITING_PATTERN state. MEASUREMENT_EVENTS must be received only
     * when the state is WAITING_MEASUREMENT.
     */
    unsigned long _unexpectedMeasurementCount;

    /**
     * Counter of how many MEASUREMENT_EVENTS with the wrong pattern were
     * received. The pattern within the MEASUREMENT_EVENT must match the pattern
     * previously received with the PATTERN_EVENT.
     */
    unsigned long _unexpectedMeasurementPatternCount;

    /**
     * Count the number of PulseId mismatches between a PATTERN_EVENT
     * and a MEASUREMENT_EVENT
     */
    unsigned long _pulseIdMismatchCount;

    /** Count how many received patterns do not have a Loop assigned */
    unsigned long _patternWitouthLoop;

    /**
     * Count total number of pending events after a cycle - this number
     * should be zero if calculation finishes before the next pattern.
     */
    unsigned long _pendingEventCount;

    /**
     * Keep statistics about the processing time, from the moment the
     * feedback calculation starts until all actuators are set.
     */
    TimeAverage _calculateStats;

    /**
     * Keep time statistics about patterns received by the LoopThread.
     */
    TimeAverage _patternStats;

    /**
     * Keep statistics about time between calculations, i.e. gives
     * the loop running rate.
     */
    TimeAverage _loopRateStats;

    /**
     * PV that has the loop running rate.
     */
    PvDataLong _ratePv;

    /**
     * Count how many patterns were received without a matching measurement.
     */
    int _patternWithoutMeasurementCount;

    bool _configured;
};

FF_NAMESPACE_END

#endif	/* _LOOPTHREAD_H */


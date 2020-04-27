/* 
 * File:   CollectorThread.h
 * Author: lpiccoli
 *
 * Created on June 4, 2010, 3:35 PM
 */

#ifndef _COLLECTORTHREAD_H
#define	_COLLECTORTHREAD_H

#include "Defs.h"
#include "Thread.h"
#include "Pattern.h"
#include "EventGenerator.h"
#include "MeasurementCollector.h"
#include "TimeUtil.h"

class CollectorThreadTest;

FF_NAMESPACE_START

/**
 * The CollectorThread is responsible for reading the latest measurements to be
 * used in the feedback calculations.
 *
 * It periodically receives MEASUREMENT_EVENTs generated by the hardware timer
 * (which is controlled by the PatternManager).
 *
 * On every MEASUREMENT_EVENT the MeasurementCollector::update() method is
 * called. This method reads the latest values making them available for the
 * LoopThreads.
 *
 * The CollectorThread informs the LoopThreads about the new values by also
 * sending MEASUREMENT_EVENTs.
 * 
 * @author L.Piccoli
 */
class CollectorThread : public Thread {
public:
    CollectorThread();
    virtual ~CollectorThread() {};

    int add(EventReceiver &receiver);
    int remove(EventReceiver &receiver);
    static CollectorThread &getInstance();

    void show();
    virtual void showDebug();

    friend class CollectorThreadTest;
    friend class ExecThreadTest;

protected:
    virtual int processEvent(Event &event);

private:
    /**
     * Enumeration of possible states for the LoopThread. It is usually
     * waiting for a Pattern or waiting for a Measurement at any given time.
     */
    enum State {
        IDLE = 25,
        WAITING_PATTERN = 35,
        WAITING_MEASUREMENT = 45
    };

    int processPatternEvent(Event &event);
    int processMeasurementEvent(Event &event);

    /** Current pattern of interest */
    Pattern _pattern;

    /**
     * List of LoopThreads that receive MEASUREMENT_EVENTS. All registered
     * LoopThreads receive the event, even if the LoopThread is not expecting
     * data for the current pattern. The LoopThread must verify if the
     * pattern of the MEASUREMENT_EVENT matches its expected pattern.
     */
    EventGenerator _generator;

    /** Count the number of PATTERN_EVENTS received */
    long long _patternCount;

    /** Count the number of MEASUREMENT_EVENTS received (from timer) */
    long long _measurementCount;

    /**
     * Counter of how many unexpected patterns were received. Unexpected patterns
     * are those known patterns (i.e. in the list of patterns of the configuration)
     * that are received while the CollectorThread is in WAITING_MEASUREMENT state.
     */
    long _unexpectedPatternCount;

    /**
     * Counter of how many MEASUREMENT_EVENTS were received while the CollectorThread
     * was in the WAITING_PATTERN state. MEASUREMENT_EVENTS must be received only
     * when the state is WAITING_MEASUREMENT.
     */
    long _unexpectedMeasurementCount;

    /**
     * Count the number of PulseId mismatches between a PATTERN_EVENT
     * and a MEASUREMENT_EVENT
     */
    long _pulseIdMismatchCount;

    /**
     * Count pending events after processing an event -> this must always be
     * zero
     */
    long _pendingEventsCount;

    /** CollectorThread state, either WAITING_PATTERN or WAITING_MEASUREMENT */
    State _state;

    /** Stats about the PATTERN_EVENT processing time */
    TimeAverage _patternStats;

    /** Stats about the MEASUREMENT_EVENT processing time */
    TimeAverage _measurementStats;
};

FF_NAMESPACE_END

#endif	/* _COLLECTORTHREAD_H */


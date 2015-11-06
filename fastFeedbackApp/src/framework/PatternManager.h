/* 
 * File:   PatternManager.h
 * Author: lpiccoli
 *
 * Created on May 26, 2010, 11:59 AM
 */

#ifndef _PATTERNMANAGER_H
#define	_PATTERNMANAGER_H

#include <map>
#include <sstream>
#include "Defs.h"
#include "Pattern.h"
#include "PatternMask.h"
#include "EventGenerator.h"
#include "Timer.h"
#include "TimeUtil.h"
#include <epicsMutex.h>

class PatternManagerTest;

FF_NAMESPACE_START

/**
 * The PatternManager class is responsible for receiving Patterns from the EVR
 * on every fiducial and broadcasting them as PATTERN_EVENTs to all LoopThreads.
 *
 * Pattern masks can be added and removed while the system is running.
 *
 * The PatternManager is also responsible to start the hardware timer that
 * wakes up the CollectorThread.
 *
 * The timer period allows measurements for the current fiducial to be read out
 * and sent off through FCOM. An example of measurement is a BPM value, which
 * takes some time to be processed and passed on to FCOM.
 *
 * In order to send events to interested threads the PatternManager has
 * one or more EventGenerators. Pattern receivers must register to get
 * periodic updates.
 *
 * Additionally, the PatternManager sends PATTERN_EVENTS only to those
 * EventReceivers that are interested, requiring the PatternManager to have
 * multiple EventGenerators.
 *
 * @author L.Piccoli
 */
class PatternManager {
public:
    PatternManager();
    PatternManager(const PatternManager &);
    PatternManager & operator=(const PatternManager &);
    virtual ~PatternManager();

    int configure();
    int fiducialCallback(Pattern pattern);
    int testFiducialCallback(Pattern pattern);
    void updateMeasurements();

    int add(PatternMask patternMask);
    int add(PatternMask patternMask, EventReceiver &receiver);
    int addHeartBeat(EventReceiver &receiver);
    int remove(PatternMask patternMask);
    int remove(PatternMask patternMask, EventReceiver &receiver);

    void enable();
    void disable();
    bool enabled() {
      return _enabled;
    }


    long long getFiductialCallbackCount() {
        return _fiducialCallbackCount;
    };

    void setTimerMonitorCallback();

    void lock();
    void unlock();

    void show();
    void showDebug();

    int getNumPatterns() {
        return _generators.size();
    };

    Pattern getCurrentPattern() {
        return _pattern;
    };

    static PatternManager &getInstance();

    friend class PatternManagerTest;

    void calculateMeasurementDelay();

private:
    int process(Pattern pattern);

    /**
     * For each registered pattern mask, the PatternManager keeps an EventGenerator.
     * Each EventGenerator has a list of EventReceivers, which are the
     * LoopThreads registered for receiving a given pattern mask.
     * 
     * This is an std::map, where the Key is the pattern mask and the value is an
     * EventGenerator.
     */
    std::map<PatternMask, EventGenerator *> _generators;

    /** Count how many times the fiducialCallback has been invoked */
    long long _fiducialCallbackCount;

    /** Count how many patterns were not found */
    long long _patternsNotFoundCount;

    /** Count how many times there were no matches */
    long long _patternsNotMatchedCount;

    /** Count how many pattern matches */
    long long _patternMatchedCount;

    /**
     * Delay between the fiducial callback and the generation of a
     * MEASUREMENT_EVENT
     */
    double _measurementDelay;

    /** Timer used to generate the callbacks to produce MEASUREMENT_EVENTS */
    Timer *_timer;

    /**
     * Latest pattern received by the fiducialCallback. The pattern is also
     * sent to the CollectorThread along with the MEASUREMENT_EVENT
     */
    Pattern _pattern;

    /**
     * Defines if PatternManager responds to fiducial callbacks or not.
     * The initial state is set to false, i.e. no PATTERN_EVENT/MEASUREMENT_EVENT
     * will be generated.
     */
    bool _enabled;

    /** Keep track of time between patterns */
    Time _patternTime;

    /** Time between patterns (in milliseconds) */
    long _timeBetweenPatterns;

    /** Time between fiducial and measurement */
    TimeAverage _delayStats;

    /** There is only one PatternManager instance */
    static PatternManager _instance;

    /** Statistics about the fiducial callback processing time */
    TimeAverage _fiducialStats;
    TimeAverage _fiducialMatchStats;
    TimeAverage _timeBetweenFiducialStats;

    /**
     * Statistics about timer callback time -> for sending a 
     * MEASUREMENT_EVENT to the CollectorThread
     */
    TimeAverage _timerCallbackStats;

    /** Mutex to controll access to registered patterns */
    epicsMutex *_mutex;

    /**
     * Pattern mask used to generate heartbeat events. Loop threads increase
     * its counter when a heartbeat event is received.
     */
    PatternMask _heartbeatMask;

    /** EventGenerator for the HEARTBEAT_EVENT */
    EventGenerator _heartbeatGenerator;

    std::stringstream _stringStream;
};

FF_NAMESPACE_END

#endif	/* _PATTERNMANAGER_H */


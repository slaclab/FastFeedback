/* 
 * File:   PatternManager.cc
 * Author: lpiccoli
 * 
 * Created on May 26, 2010, 11:59 AM
 */

#include <cadef.h>
#include "PatternManager.h"
#include "Pattern.h"
#include "Event.h"
#include "CollectorThread.h"
#include "PatternGenerator.h"
#include "Log.h"
#include <iostream>
#include <sstream>
#include <evrTime.h>
#include <evrPattern.h>
#include <time.h>

#include "debugPrint.h"

USING_FF_NAMESPACE

#ifdef  DEBUG_PRINT
extern int DEBUG_PULSEID_FLAG;
#endif

long DELAY_START = 0;
long DELAY_END = 0;
long DELAY_COLLECT = 0;
long DELAY_MISMATCHES = 0;
long DELAY_COLLECT_MISMATCHES = 0;

/**
 * Callback function invoked by the PatternManager timer, it is used
 * to generate MEASUREMENT_EVENTS.
 *
 * MEASUREMENT_EVENTS produced by the PatternManager are sent to the
 * CollectorThread, signaling that measurements for the current fiducial
 * can be read.
 *
 * @author L.Piccoli
 */
void PatternManagerCallback(void *arg) {
    PatternManager::getInstance().updateMeasurements();
    DELAY_END++;
    if (DELAY_START != DELAY_END) {
        DELAY_MISMATCHES++;
        DELAY_END = DELAY_START; // Re-sync
    }
}

/**
 * Callback function invoked by the EVR.
 *
 * @author L.Piccoli
 */
void PatternManagerFiducialCallback(void *arg) {
#ifdef RTEMS
    epicsTimeStamp patternTime;
    evrModifier_ta patternWord;
    epicsUInt32 patternStatus; /* see evrPattern.h for values */
    int status = evrTimeGetFromPipeline(&patternTime, evrTimeCurrent,
					patternWord, &patternStatus, 0, 0, 0);

    if (status == 0) {
        Pattern pattern(patternWord, patternTime);
        PatternManager::getInstance().fiducialCallback(pattern);
    }
#endif
}

/**
 * PatternManager singleton initialization
 */
PatternManager PatternManager::_instance;

PatternManager::PatternManager() :
_fiducialCallbackCount(0),
_patternsNotFoundCount(0),
_patternsNotMatchedCount(0),
_patternMatchedCount(0),
_measurementDelay(1),
_enabled(false),
_delayStats(50, "Measured Delay Stats"),
_fiducialStats(50, "Fiducial Processing Stats"),
_fiducialMatchStats(50, "Fiducial Processing Stats (Match)"),
_timeBetweenFiducialStats(50, "Stats between fiducials"),
_timerCallbackStats(50, "Timer Callback Stats") {
    _timer = new Timer(&PatternManagerCallback);
    _mutex = new epicsMutex();

    // Add a 0.5Hz heartbeat event.
    // 0 0 0 0 0 0 0 256 0 0 0 0 0 0 0 0 0 0 0 0 0
    std::vector<unsigned short> pattern;
    for (int i = 0; i < 21; ++i) {
        pattern.push_back(0);
    }
    pattern[7] = 256; // 0.5 Hz bit
    pattern[10] = 55; // TS4 only

    _heartbeatMask = pattern;
}

PatternManager::~PatternManager() {
    if (_timerType == Timer::EPICS_TIMER) {
        delete _timer;
    }
    delete _mutex;
}

/**
 * @author L.Piccoli
 */
int PatternManager::configure() {
#ifdef LINUX
    return 0;
#else
    std::cout << "--- Configuring EVR...";
    int status = evrTimeRegister(PatternManagerFiducialCallback, 0);
    if (status != 0) {
        Log::getInstance() << "ERROR: evrTimeRegister failed (status="
                << status << ")" << Log::flush;
        return -1;
    }
    std::cout << " done." << std::endl;
    return 0;
#endif
}

/**
 * This method creates a PATTERN_EVENT passing along all information about the
 * received pattern. The EventReceiver::update() method is called so all
 * interested LoopThreads receive the event.
 *
 * TODO: Pre-select those patterns that should be sent to process(). Don't
 * need to have every fiducial call generate a process() call.
 *
 * @return 0 if a PATTERN_EVENT was generated, -1 otherwise
 * @author L.Piccoli
 */
int PatternManager::fiducialCallback(Pattern pattern) {
    _fiducialCallbackCount++;
    if (_enabled) {
        return process(pattern);
    }
    return -1;
}

/**
 * This method is used for testing only. It is called from the Timer set
 * up by the PatternGenerator.
 *
 * It creates a PATTERN_EVENT passing along all information about the
 * received pattern. The EventReceiver::update() method is called so all
 * interested LoopThreads receive the event.
 *
 * No EVR code is invoked here, the pattern for this testFiducial is
 * generated by the PatternGenerator class, which reads patterns from
 * a configuration file.
 *
 * @param pattern the pattern created by the PatternGenerator
 * @return 0 if a PATTERN_EVENT was generated, -1 otherwise
 * @author L.Piccoli
 */
int PatternManager::testFiducialCallback(Pattern pattern) {
    _fiducialCallbackCount++;
    return process(pattern);
}

/**
 * This method creates a PATTERN_EVENT passing along all information about the 
 * received pattern. The EventReceiver::update() method is called so all interested
 * LoopThreads receive the event.
 *
 * The timer to generate a MEASUREMENT_EVENT is started by this method.
 *
 * @param pattern Pattern just received by the EVR
 * @return 0 if pattern was expected, -1 if pattern is not on the list
 * @author L.Piccoli
 */
int PatternManager::process(Pattern evrPattern) {
    _fiducialStats.restart();
    _fiducialMatchStats.restart();
    bool match = false;
    Event event;

    // If the receive pattern matches the _heartbeatPattern, sent out
    // the HEARTBEAT_EVENT
    if (evrPattern.match(_heartbeatMask)) {
        event._type = HEARTBEAT_EVENT;
        event._pattern = evrPattern;
        _heartbeatGenerator.send(event);
    }

    std::map<PatternMask, EventGenerator *>::iterator iterator;

    PatternManager::getInstance().lock();
    iterator = _generators.begin();
    while (iterator != _generators.end()) {
        PatternMask patternMask = (*iterator).first;
        if (evrPattern.match(patternMask)) {
            event._type = PATTERN_EVENT;
            event._pattern = evrPattern;
            match = true;
            iterator->second->send(event);
        }
        ++iterator;
    };
    PatternManager::getInstance().unlock();

    _fiducialStats.end();

    // Calculate the time between matched patterns
    if (match) {
        Time currentTime;
        Time difference;
        currentTime.now();
        difference = currentTime - _patternTime;
        _timeBetweenPatterns = difference.toMicros();

        _patternTime.now();
    }

    if (!match) {
        _patternsNotMatchedCount++;
        //        _timer->cancel();
        return -1;
    }

    DELAY_START++;
    _timeBetweenFiducialStats.end();

    // If the PatternManager is disabled at this point, then do not
    // start the timer
    if (!_enabled) {
      return 0;
    }

    // Start the timer
    _timeBetweenFiducialStats.start();
    _timer->start(_measurementDelay);    

    // Starts recording the time between this point and the processing of 
    // the timer generated MEASUREMENT_EVENT by the CollectorThread
    _delayStats.start();

    // Save the received pattern
    _pattern = evrPattern;

    // Also sends the event to the CollectorThread
    event._type = PATTERN_EVENT;
    event._pattern = evrPattern;
    CollectorThread::getInstance().send(event);

    Log::getInstance() << Log::flagPulseId << Log::dpInfo
		       << "PatternManager::process(), PULSEID="
		       << (int) event._pattern.getPulseId() << Log::dp;

    _patternMatchedCount++;

    // Record time to measure the actual timer delay
    _fiducialMatchStats.end();

    return 0;
}

/**
 * If the specified pattern mask has not yet been added, an EventGenerator is
 * created and the pair PatternMask/EventGenerator is added to the generators
 * map.
 *
 * @param patternMask PatternMask to be added to the list of observed patterns
 * @return 0 if pattern mask was added successfully, -1 if pattern mask
 * was already defined
 * @author L.Piccoli
 */
int PatternManager::add(PatternMask patternMask) {
    std::map<PatternMask, EventGenerator *>::iterator iterator;

    lock();

    // If Pattern is already present, don't add new generator
    iterator = _generators.find(patternMask);
    if (iterator != _generators.end()) {
      unlock();
      return -1;
    }

    // If PatternMask is not present, create a generator and add to the map
    EventGenerator *generator = new EventGenerator();
    _generators.insert(std::pair<PatternMask, EventGenerator *>
            (patternMask, generator));

    unlock();

    return 0;
}

/**
 * Add a receiver to the list of receivers for the specified pattern mask.
 *
 * If pattern mask is not yet on the _generators list, it is added.
 *
 * @param patternMask PatternMask to be monitored by the receiver
 * @param receiver EventReceiver that should be called whenever the specified
 * patternMask matches a pattern received
 * @return always returns 0
 * @author L.Piccoli
 */
int PatternManager::add(PatternMask patternMask, EventReceiver &receiver) {
    add(patternMask);

    std::map<PatternMask, EventGenerator *>::iterator iterator;
    iterator = _generators.find(patternMask);

    if (iterator != _generators.end()) {
      iterator->second->add(receiver);
    }
    return 0;
}

/**
 * Add a receiver to the heartbeat event.
 *
 * @param receiver EventReceiver that will process the HEARTBEAT_EVENT (generated
 * at 0.5 Hz)
 * @return always 0
 * @author L.Piccoli
 */
int PatternManager::addHeartBeat(EventReceiver &receiver) {
    _heartbeatGenerator.add(receiver);
    return 0;
}

/**
 * Remove the specified pattern mask and its respective EventGenerator from the
 * generators map.
 *
 * The effect of invoking this method is that the specified pattern mask will not
 * generate any processing in the system.
 *
 * @param patternMask PatternMask to be removed from the watched list.
 * @return 0 if pattern mask was removed, -1 if pattern mask was not on the list
 * @author L.Piccoli
 */
int PatternManager::remove(PatternMask patternMask) {
    std::map<PatternMask, EventGenerator *>::iterator iterator;

    lock();

    // If Pattern is not present, don't do anything
    iterator = _generators.find(patternMask);
    if (iterator == _generators.end()) {
      unlock();
      return -1;
    }

    // If found, delete the generator and remove pattern mask from list
    if (iterator->second != NULL) {
      delete(iterator->second);
    }
    _generators.erase(iterator);

    unlock();
    return 0;
}

/**
 * Remove the specified receiver from the list of receivers for the given
 * pattern mask. If no more receivers are listed for the pattern mask,
 * the generator and the pattern mask are removed.
 *
 * @param patternMask PatternMask to be removed from monitored receiver list
 * @param receiver EventReceiver that should be removed from the pattern mask
 * list
 * @return 0 if receiver is removed from the list of EventReceivers listening
 * for the pattern mask, -1 if pattern mask is not being monitored or if
 * receiver is* not register to receive events from the given pattern mask
 * @author L.Piccoli
 */
int PatternManager::remove(PatternMask patternMask, EventReceiver &receiver) {
    std::map<PatternMask, EventGenerator *>::iterator iterator;

    iterator = _generators.find(patternMask);
    if (iterator == _generators.end()) {
        return -1; // Pattern mask not found
    }

    // If pattern mask is found, remove receiver
    if (iterator->second->remove(receiver) != 0) {
        return -1;
    }

    // If receiver was removed, check if there are no more receivers,
    // in this case, remove the pattern mask and its generator
    if (iterator->second->isEmpty()) {
        return remove(patternMask);
    }

    return 0;
}

/**
 * Return the PatternManager singleton
 *
 * @return the PatternManager
 * @author L.Piccoli
 */
PatternManager &PatternManager::getInstance() {
    return _instance;
}

/**
 * Generate a MEASUREMENT_EVENT sent out to the CollectorThread singleton.
 *
 * @author L.Piccoli
 */
void PatternManager::updateMeasurements() {
  if (_enabled) {
    Event event;
    event._type = MEASUREMENT_EVENT;
    event._pattern = _pattern;
    CollectorThread::getInstance().send(event);
  }
}

/**
 * Allow PatternManager to generate PATTERN_EVENTS and MEASUREMENT_EVENTS, by
 * setting _enabled to true.
 *
 * @author L.Piccoli
 */
void PatternManager::enable() {
    _enabled = true;
}

/**
 * Disables generation of PATTERN_EVENTS and MEASUREMENT_EVENTS by the
 * PatternManager. The attribute _enabled is set to false.
 *
 * @author L.Piccoli
 */
void PatternManager::disable() {
  _timer->cancel();
  _enabled = false;
}

void PatternManager::showDebug() {
    _fiducialStats.show("  ");
    _fiducialMatchStats.show("  ");
    _timeBetweenFiducialStats.show("  ");
    _delayStats.show("  ");
    _timer->showDebug();
    std::cout << "DELAY START=" << DELAY_START
            << ", END=" << DELAY_END
            << ", COL=" << DELAY_COLLECT
            << ", MISMATCH=" << DELAY_MISMATCHES
            << ", MISMATCH COL=" << DELAY_COLLECT_MISMATCHES
            << std::endl;
}

void PatternManager::show() {
    std::cout << "  delay:   " << _measurementDelay << std::endl;
    std::cout << "  enabled: ";
    if (_enabled) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
    std::cout << "  fiducials: " << _fiducialCallbackCount << std::endl;
    std::cout << "  patterns: " << std::endl;
    std::cout << "  -> not found:   " << _patternsNotFoundCount << std::endl;
    std::cout << "  -> not matched: " << _patternsNotMatchedCount << std::endl;
    std::cout << "  -> matched:     " << _patternMatchedCount << std::endl;
    std::cout << "  time between patterns: ";
    if (_timeBetweenPatterns > 100000) {
        _timeBetweenPatterns *= 1e-3;
        std::cout << _timeBetweenPatterns << " ms" << std::endl;
    } else {
        std::cout << _timeBetweenPatterns << " usec" << std::endl;
    }

    lock();
    if (_generators.size() == 0) {
        std::cout << "  * there are no patterns registered" << std::endl;
    }
    std::map<PatternMask, EventGenerator *>::iterator it;
    int i = 0;
    for (it = _generators.begin(); it != _generators.end(); ++it) {
        PatternMask patternMask = it->first;
        EventGenerator *generator = it->second;
	//        std::stringstream s;
	_stringStream.str(std::string()); // Clear the stream
        _stringStream << "P" << i;
        ++i;
        std::cout << "  " << &patternMask
                << " has " << generator->getEventCount()
                << " events. ";
        std::cout << "(" << generator->getReceiverCount() << " listeners)"
                << std::endl;
    }
    std::cout << "--- Heartbeat ---" << std::endl;
    std::cout << "  -> " <<  &_heartbeatMask << std::endl;
    std::cout << "  -> Generator has " << _heartbeatGenerator.getEventCount()
	      << " events. ";
    std::cout << "(" << _heartbeatGenerator.getReceiverCount() << " listeners)"
	      << std::endl;
    std::cout << "  -> mutex info: " << std::endl;
    _mutex->show(4);
    unlock();
}

void PatternManager::setTimerMonitorCallback() {
    _timer->setCallback(&PatternManagerCallback);
}

// Compute the time taken between the fiducial and this point

void PatternManager::calculateMeasurementDelay() {
    DELAY_COLLECT++;
    if (DELAY_START != DELAY_COLLECT) {
        DELAY_COLLECT_MISMATCHES++;
        DELAY_COLLECT = DELAY_START; // Re-sync
    }
    _delayStats.end();
}

void PatternManager::lock() {
    _mutex->lock();
}

void PatternManager::unlock() {
    _mutex->unlock();
}

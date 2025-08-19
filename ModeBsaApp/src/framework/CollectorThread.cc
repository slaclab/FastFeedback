/* 
 * File:   CollectorThread.cc
 * Author: lpiccoli
 * 
 * Created on June 4, 2010, 3:35 PM
 */

#include <iostream>

#include "CollectorThread.h"
#include "PatternManager.h"
#include "Log.h"

USING_FF_NAMESPACE

/**
 * Class constructor. Thread name is set to "Collector".
 *
 * @author L.Piccoli
 */
CollectorThread::CollectorThread() :
Thread("Collector", 100),
_patternCount(0),
_measurementCount(0),
_unexpectedPatternCount(0),
_unexpectedMeasurementCount(0),
_pulseIdMismatchCount(0),
_pendingEventsCount(0),
_state(WAITING_PATTERN),
_patternStats(50, "Pattern Processing"),
_measurementStats(50, "Measurement Processing") {
}

/**
 * Process PATTERN_EVENTS and MEASUREMENT_EVENTS only.
 *
 * @param event Event received by the CollectorThread
 * @return 0 if event was recognized and processed, -1 on failure
 * @author L.Piccoli
 */
int CollectorThread::processEvent(Event& event) {
  int result = 0;
  switch (event._type) {
    _pendingEventsCount += getPendingEvents();
        case PATTERN_EVENT:
            _patternCount++;
	    return processPatternEvent(event);
        case MEASUREMENT_EVENT:
	  MeasurementCollector::getInstance().lock();
	  result = processMeasurementEvent(event);
	  MeasurementCollector::getInstance().unlock();
	  return result;
        default:
            return -1;
    }
}

/**
 * This method is called by processEvent() whenever a PATTERN_EVENT is received.
 * The pattern carried with the event is copied locally such that on the next
 * MEASUREMENT_EVENT, this pattern is passed down to the MeasurementCollector.
 *
 * @param event a PATTERN_EVENT
 * @returns always 0
 * @author L.Piccoli
 */
int CollectorThread::processPatternEvent(Event &event) {
  if (_state == WAITING_PATTERN) {
    _patternStats.start();

    Log::getInstance() << Log::flagEvent << Log::dpInfo
		       << "INFO: CollectorThread::processPatternEvent() PULSEID="
		       << (int) event._pattern.getPulseId() << Log::dp;

    _pattern = event._pattern;
    _state = WAITING_MEASUREMENT;
    _patternStats.end();
    return 0;
  }
  else {
    _unexpectedPatternCount++;
    return -1;
  }
}

/**
 * This method is called when a MEASUREMENT_EVENT is detected in the
 * processEvent() method.
 *
 * The reception of a MEASUREMENT_EVENT by the CollectorThread means that
 * all measurement devices have captured data for this cycle and new values
 * are available through ChannelAccess or FCOM for reading.
 *
 * First all the MeasurementDevices managed by the MeasurementCollector are
 * updated for the last pattern received.
 *
 * Second, a MEASUREMENT_EVENT is broadcast to all LoopThreads, so they are
 * able to start calculating the feedback values with the new measurements.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int CollectorThread::processMeasurementEvent(Event &event) {
  if (_state == WAITING_MEASUREMENT) {
    _measurementStats.start();
    // Update measurement delay time in the PatternManager
    PatternManager::getInstance().calculateMeasurementDelay();

    // Check if pulseId from PATTERN_EVENT is the same received in the
    // MEASUREMENT_EVENT
    if (event._pattern.getPulseId() != _pattern.getPulseId()) {
      _pulseIdMismatchCount++;
    }

    // Update devices
    int status = 0;
    if (!PatternManager::getInstance().enabled()) {
      Log::getInstance() << "ERROR: Updating measurements with PatternCollector disabled!"
			 << " (measurements="
			 << MeasurementCollector::getInstance().getMeasurementCount() << ")"
			 <<  Log::cout;
      status = -1;
    }
    else {
      status = MeasurementCollector::getInstance().update(event._pattern);
    }
    if (status == 0) {
        _measurementCount++;

#ifdef DEBUG_EVENT
        Log::getInstance() << "DEBUG: CollectorThread broadcasting MEASUREMENT_EVENT pulseId="
                << (int) event._pattern.getPulseId() << Log::flush;
#endif

        // Broadcast evest
	_generator.send(event);
    }

    _state = WAITING_PATTERN;
    _measurementStats.end();
    return 0;
  }
  else {
    _unexpectedMeasurementCount++;
    return -1;
  }
}

/**
 * Add an EventReceiver (usually a LoopThread) to the list of receivers
 * for the MEASUREMENT_EVENTS. The event is sent to all listeners once data
 * for all devices (for the current pattern) has been collected. Once the
 * LoopThreads receive the MEASUREMENT_EVENT they can retrieve the latest
 * data from the devices themselves.
 *
 * @param receiver a LoopThread interested in receiving MEASUREMENT_EVENTS
 * from the CollectorThread.
 * @author L.Piccoli
 */
int CollectorThread::add(EventReceiver &receiver) {
    return _generator.add(receiver);
}

int CollectorThread::remove(EventReceiver &receiver) {
    return _generator.remove(receiver);
}

/**
 * Return the CollectorThread singleton
 *
 * @return the CollectorThread
 * @author L.Piccoli
 */
CollectorThread &CollectorThread::getInstance() {
    static CollectorThread _instance;
    return _instance;
}

void CollectorThread::show() {
    std::cout << "--- Collector Thread " << std::endl;
    std::cout << "  pattern events:         " << _patternCount << std::endl;
    std::cout << "  measurement events:     " << _measurementCount << std::endl;
    std::cout << "  measurement events out: " << _generator.getEventCount() << std::endl;
    std::cout << "  number of receivers:    " << _generator.getReceiverCount() << std::endl;
    MeasurementCollector::getInstance().show();
}

void CollectorThread::showDebug() {
  Thread::showDebug();
  _patternStats.show("  ");
  _measurementStats.show("  ");
  std::cout << "  pending events count:    " << _pendingEventsCount << std::endl;
  std::cout << "  pulseId mismatches:      " << _pulseIdMismatchCount << std::endl;
  std::cout << "  unexpected patterns:     " << _unexpectedPatternCount << std::endl; 
  std::cout << "  unexpected measurements: " << _unexpectedMeasurementCount << std::endl; 
  std::cout << "  fail sends:              " << getFailedSendCount() << std::endl;   
  std::cout << "  fail receives:           " << getFailedReceiveCount() << std::endl;   
  MeasurementCollector::getInstance().showDebug();
}


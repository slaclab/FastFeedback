/* 
 * File:   LoopThread.cc
 * Author: lpiccoli
 * 
 * Created on June 7, 2010, 2:13 PM
 */

#include "LoopThread.h"
#include "PatternManager.h"
#include "CollectorThread.h"
#include "TimeUtil.h"
#include "Log.h"
#include "EventLogger.h"
#include "ExecConfiguration.h"
#include <epicsMutex.h>

USING_FF_NAMESPACE

LoopThread::LoopThread(const std::string name,
        LoopConfiguration *configuration) :
Thread(name, 80),
_state(WAITING_PATTERN),
_configuration(configuration),
_firstLoop(NULL),
//_enabledPv(name + " STATE", true, this, RECONFIGURE_EVENT),
_enabledPv(name + " STATE", true),
_stateActPv(name + " ACT_STATE"),
_previousState(false),
//_modePv(name + " MODE", false, this, MODE_EVENT),
_modePv(name + " MODE", false),
_modeActPv(name + " ACT_MODE"),
_previousMode(false),
_patternCount(0),
_measurementCount(0),
_unexpectedPatternCount(0),
_unknownPatternCount(0),
_unexpectedMeasurementCount(0),
_unexpectedMeasurementPatternCount(0),
_pulseIdMismatchCount(0),
_patternWitouthLoop(0),
_pendingEventCount(0),
_calculateStats(50, name + " Processing Time (algorithm time)"),
_patternStats(50, name + " Pattern Stats (time between POIs)"),
_loopRateStats(1, name + " Loop rate stats (time between calculations"),
_ratePv(name + " RATE", 0),
_patternWithoutMeasurementCount(0),
_configured(false) {
    createLoops(); // TODO: replace with configure()
    _configuration->_eventReceiver = this;
    _previousState = _enabledPv.getValue();
    _previousMode = _modePv.getValue();
}

LoopThread::~LoopThread() {
}

/**
 * (Re)configure the Loops of a LoopThread based on the current configuration.
 * This method is invoked when the LoopThead is created and when the system
 * is reconfigured.
 *
 * Currently if there are Loops configured, they are deleted and created again.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LoopThread::configure() {
    // First delete all Loops
    std::vector<Loop *>::iterator it;
    for (it = _loops.begin(); it != _loops.end(); ++it) {
        Loop *loop = (*it);
	if (loop != NULL) {
	  delete loop;
	}
    }
    _loops.clear();

    if (createLoops() != 0) {
        return -1;
    }

    return 0;
}

/**
 * Create the loops based on the patterns and algorithm in the configuration.
 * There is one loop per pattern - usually feedbacks respond to a single pattern
 * only.
 *
 * @return 0 if success, -1 on failure
 * @author L.Piccoli
 */
int LoopThread::createLoops() {
  _firstLoop = NULL;
    std::vector<PatternMask>::iterator it;
    for (it = _configuration->_patternMasks.begin();
            it != _configuration->_patternMasks.end(); ++it) {
        // Create the Loop
        PatternMask patternMask = (*it);
        Loop *loop = new Loop(_configuration, patternMask);
	if (_firstLoop == NULL) {
	  _firstLoop = loop;
	}
        _loops.push_back(loop);
    }
    return 0;
}

/**
 * Process PATTERN_EVENTS, MEASUREMENT_EVENTS and RECONFIGURE_EVENT.
 *
 * @param event Event received by the LoopThread
 * @return 0 if event was recognized and processed, -1 on failure
 * @author L.Piccoli
 */
int LoopThread::processEvent(Event& event) {
  int value = 0;
    switch (event._type) {
    case HEARTBEAT_EVENT:
      _configuration->_loopCounter = _configuration->_loopCounter.getValue() + 1;
      checkMode();
      if (_firstLoop != NULL) {
	_firstLoop->checkFcom();
      }
      return 0;
    case PATTERN_EVENT:
      value = -1;
      // Only process a PATTERN if _stateActPV is true, i.e.
      // Loop has been through a checkMode and it is really enabled.
      if (_stateActPv == true && _configuration->_configured) {
    	  value = processPatternEvent(event);
      }
      return value;
    case MEASUREMENT_EVENT:
      value = -1;
      checkMode();
      if (_stateActPv == true && _configuration->_configured) {
    	  value = processMeasurementEvent(event);
      }
      return value;
    case DISCONNECT_EVENT:
      return disconnectDevices();
    default:
      return -1;
    }
}

/**
 * This method is invoked by processEvent() whenever it gets a
 * HEARTBEAT_EVENT or a MEASUREMENT_EVENT.
 *
 * The state of the _enabledPv and _modePv are checked and the loop
 * is configured accordingly.
 *
 * Since this method is invoked frequently it must make sure the loop
 * is turned ON/OFF or the mode changes to ENABLED/COMPUTE only once.
 * 
 * The transition ON=>OFF OFF=>ON ENABLED=>COMPUTE COMPUTE=>ENABLED
 * needs to be identified.
 * 
 * @returns 0 if no change or successful change, -1 if failure,
 * @author L.Piccoli
 */
int LoopThread::checkMode() {
  bool statePv = _enabledPv.getValue();
  bool modePv = _modePv.getValue();

  int value = 0;
  // If the STATE PV is still the same, check if MODE has changed
  if (statePv == _previousState) {
    // If MODE is still the same, just return
    if (modePv == _previousMode) {
      return 0;
    }
    else { //MODE changed, STATE did not
      if (_configured) {
	EventLogger::getInstance().add(MODE_EVENT, RECEIVED, _configuration->_slotName, modePv);
	value = changeMode(statePv, modePv);
	EventLogger::getInstance().add(MODE_EVENT, PROCESSED, _configuration->_slotName, value);
      }
      else {
	Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
			   << "INFO: LoopThread::checkMode() Can't changeMode because reconfigure() failed"
			   << Log::dp;
      }
    }
  }
  else { //STATE changed, mode could have changed or not
    EventLogger::getInstance().add(RECONFIGURE_EVENT, RECEIVED, _configuration->_slotName, statePv);
    value = reconfigure(statePv, modePv);
    _configured = true;
    if (value != 0) {
      _configured = false;
    }
    EventLogger::getInstance().add(RECONFIGURE_EVENT, PROCESSED, _configuration->_slotName, value);
  }

  if (value == 0) {
    _previousState = statePv;
    _previousMode = modePv;
    
    _stateActPv = statePv;
    _modeActPv = modePv;

    _stateActPv.scanIoRequest();
    _modeActPv.scanIoRequest();
  }

  return value;
}

/**
 * This method is invoked when the Loop STATE goes from ON to OFF. All
 * Devices are disconnected. New connections will be made when STATE goes
 * back to ON.
 *
 * @author L.Piccoli
 */
int LoopThread::disconnectDevices(bool turnFeedbackOff) {
    // Turn all FBCK PVs OFF when loop is turned OFF
    std::vector<Loop *>::iterator it;
    Loop *loop;

    // When this mutex is lock the CollectorThread will stop responding
    // to fiducials!
    PatternManager::getInstance().lock();
    PatternManager::getInstance().disable();
    MeasurementCollector::getInstance().lock();

    if (turnFeedbackOff) {
      if (_firstLoop->setFeedbackDevices(false, false) != 0) {
	  MeasurementCollector::getInstance().unlock();
	  PatternManager::getInstance().enable();
	  PatternManager::getInstance().unlock();

	  return -1;
        }
      /*
      for (it = _loops.begin(); it != _loops.end(); ++it) {
        loop = (*it);
        if (loop->setFeedbackDevices(false, false) != 0) {
	  MeasurementCollector::getInstance().unlock();
	  PatternManager::getInstance().enable();
	  PatternManager::getInstance().unlock();

	  return -1;
        }
      }
      */
    }

    for (it = _loops.begin(); it != _loops.end(); ++it) {
        loop = (*it);
        loop->disconnectDevices();
    }
    MeasurementCollector::getInstance().unlock();
    PatternManager::getInstance().enable();
    PatternManager::getInstance().unlock();

    return 0;
}

/**
 * Change mode of operation -> either to ENABLE or to COMPUTE
 *
 * If STATE is ON and MODE is ENABLE:
 *   o Measurements FBCK ON
 *   o Actuators FBCK ON
 *
 * If STATE is ON and MODE is COMPUTE:
 *   o Measurements FBCK ON
 *   o Actuators FBCK OFF - do not set actuators
 *
 * If STATE is OFF and MODE is ENABLE *or* COMPUTE
 *   o Measurements FBCK OFF
 *   o Actuators FBCK OFF
 *
 * If STATE is ON and MODE changes from COMPUTE to ENABLE:
 *   o Must retrieve current actuator values before calculating
 *     the next feedback value
 * @param statePv true if feedback is ON
 * @param modePv true if feedback is ENABLEd
 * @author L.Piccoli
 */
int LoopThread::changeMode(bool statePv, bool modePv) {
    std::vector<Loop *>::iterator it;

    bool actuatorFbck = false;
    bool measurementFbck = false;

    // The generation of PATTERN_EVENTS is temporarily disabled,
    // only enable again if currently enabled!
    PatternManager::getInstance().lock();
    bool patternEnabled = PatternManager::getInstance().enabled();
    PatternManager::getInstance().disable();

    if (statePv == true) {
      Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			 << "LoopThread::changeMode() STATE=ON";
    }
    else {
      Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			 << "LoopThread::changeMode() STATE=OFF";
    }
    if (modePv == true) {
      Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			 << " MODE=ENABLED";
    }
    else {
      Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			 << " MODE=COMPUTE";
    }
    Log::getInstance() << Log::dp;

    // mode changed from compute to enabled and feedback is ON
    if (statePv == true) {
      if (modePv == true) {
        actuatorFbck = true;
        measurementFbck = true;
        if (_configuration->_mode == false) {
          // if the mode has changed -
          // State is ON, and MODE was previously in COMPUTE then
          // we must update the actuators with new initial values
          if (resetActuators() != 0) {
            modePv = false;
            statePv = false;
            _configuration->_logger
                      << Log::showtime
		      << "ERROR: Failed to get current actuator values, "
                      << "feedback remains in COMPUTE"
                      << Log::flushpv;
            //TODO dmf be sure properly cleaned up from a failure here
	    // Reenable patterns if it was enabled before
	    if (patternEnabled) {
	      PatternManager::getInstance().enable();
	    }
	    PatternManager::getInstance().unlock();
            return -1;
          }

          // Reset Loop algorithms
          for (it = _loops.begin(); it != _loops.end(); ++it) {
            (*it)->resetAlgorithm();
          }

          // Also clear all pending events in the queue and start fresh
          if (getPendingEvents() > 0) {
            clear();
          }
        }
        // state is ON, mode was already ENABLED, do nothing
      } else {
    	// State is OFF, changed from ON
        measurementFbck = true;
      }
      // MODE READBACK - ON
    }


    MeasurementCollector::getInstance().clearStats();

    _configuration->_mode = modePv;
    _configuration->_enabled = statePv;

    _configuration->_logger << Log::showtime << "Current mode is ";
    if (_configuration->_mode) {
      _configuration->_logger << "ENABLE";
    }
    else {
      _configuration->_logger << "COMPUTE";
    }
    _configuration->_logger << Log::flushpvnoalarm;

    //    _configuration->_logger << Log::clearpv;

    // Set FBCK for all actuator and measurement devices.
    // If feedback is off (STATE = OFF) then this call will not 
    // result in setting the FBCK PV, because the devices are not
    // connected.
    // Even when there are multiple patterns, i.e. multiple loops
    // only one of them must set the feedback devices
    //    loop = (*_loops.begin());
    if (_firstLoop->setFeedbackDevices(measurementFbck, actuatorFbck) != 0) {
      _configuration->_logger
      	  << Log::showtime <<
      	  "ERROR: Failed set FBCK PVs, feedback remains OFF"
      	  << Log::flushpvonly;
      // Reenable patterns if it was enabled before
      if (patternEnabled) {
	PatternManager::getInstance().enable();
      }
      PatternManager::getInstance().unlock();
      return -1;
    }

    // Reenable patterns if it was enabled before
    if (patternEnabled) {
      PatternManager::getInstance().enable();
    }
    PatternManager::getInstance().unlock();
    return 0;
}


/**
 * This method reconfigures the LoopThread when the STATE PV transitions
 * from ON to OFF or from OFF to ON.
 *
 * All configuration is reloaded from the current values of the Loop PVs.
 *
 * TODO: method too long!
 *
 * @return 0 if Loop thread is ready, or -1 if there is a problem during
 * reconfiguration, in which case the Loop will remain OFF.
 * @author L.Piccoli
 */
int LoopThread::reconfigure(bool statePv, bool modePv) {

    if (statePv == false) {
    	//Feedback was turned off
        _configuration->_logger << Log::showtime << "Feedback stopped." << Log::flushpvnoalarm;

        // If Loop was configured, then mark it not configured, so the next
        // time _enable is true ($(LOOP):STATE = On) the configuration is
        // reloaded.
        if (_configuration->_configured == true) {
            // At this point disconnect all Devices, exclude them
            // from MeasurementCollector as well
            disconnectDevices();

            // Remove this thread from the CollectorThread list
	    MeasurementCollector::getInstance().lock();
            CollectorThread::getInstance().remove(*this);
	    MeasurementCollector::getInstance().unlock();

            // Remove this thread from the PatternManager list
            long numPatterns = _configuration->_totalPoiPv.getValue();
            PatternManager::getInstance().lock();
            for (int i = 0; i < numPatterns; ++i) {
                PatternManager::getInstance().remove(_configuration->_patternMasks[i], *this);
            }
            PatternManager::getInstance().unlock();

            _configuration->_configured = false;
            _loopRateStats.clear();
        }
	//        _configuration->_logger << Log::clearpv;
        return 0;
    }

    // otherwise state changed to ON, if feedback is already configured, don't do it again
    if (_configuration->_configured == true) {
        _configuration->_logger <<  Log::showtime << "Loop "
                << _configuration->_loopNamePv.getValue().c_str()
                << " configured." << Log::flushpvnoalarm;

        return 0;
    }

    // also, configure if and only if the INSTALLED PV is set to true
    if (_configuration->_installedPv == false) {
        statePv = false;
        _configuration->_logger << Log::showtime
				<< "INSTALLED PV set to NO, cannot configure. "
				<< Log::flushpv;
        return -1;
    }

    //ok state was turned ON, it's already installed but not configured; now configure this feedback
    _configuration->_logger << Log::showtime << "Reconfiguring Loop " << Log::flushpvnoalarm;
    //            _configuration->_loopNamePv.getValue().c_str() << Log::flushpvnoalarm;

    if (_configuration->configure() != 0) {
    	//configuration failed
        _configuration->_logger << "ERROR: Failed to reconfigure Loop." << Log::cout;
        statePv = false;
        //TODO dmf if we fail here, how do we clean up and leave it in the OFF state?
        // delete communication channels? return -1?
        disconnectDevices(); //??
        return -1;
    } else {
      //configuration successful, now configure loops
      long numPatterns = _configuration->_totalPoiPv.getValue();

      // now disable the patternManager and the MeasurementCollector to add this feedback
      // When this mutex is lock the CollectorThread will stop responding
      // to fiducials! The generation of PATTERN_EVENTS is temporarily disabled
      PatternManager::getInstance().lock();
      PatternManager::getInstance().disable();
      MeasurementCollector::getInstance().lock();

      for (int i = 0; i < numPatterns; ++i) {
        // Reconfigure Loop - which configures the algorithms
        if (_loops[i]->reconfigure(_configuration->_patternMasks[i]) != 0) {
          _configuration->_logger << "ERROR: Failed to reconfigure Loop " << i
				  << "(# patterns: " << numPatterns
				  << ")" << Log::cout;
          // we failed, set feedback to OFF and release the other threads
          statePv = false;
	  //disconnectDevices(); 
          MeasurementCollector::getInstance().unlock();
          PatternManager::getInstance().enable();
	  PatternManager::getInstance().unlock();

	  _configuration->_logger << "Returning -1 from LoopThread::reconfigure()" << Log::cout;
          return -1;
        }

        // Add the LoopThread as a receiver for the PATTERN_EVENT containing
        // the configured patterns
        int status = PatternManager::getInstance().add(_configuration->_patternMasks[i], *this);
        if (status != 0) {
          _configuration->_logger
                      << "ERROR: Failed to add pattern to PatternManager."
                      << _configuration->_patternMasks[i].toString().c_str()
                      << Log::cout;
          // we failed, set feedback to OFF and release the other threads
          statePv = false;
          disconnectDevices();
          MeasurementCollector::getInstance().unlock();
          PatternManager::getInstance().enable();
	  PatternManager::getInstance().unlock();
          return -1;
        }

	/*	Log::getInstance() << "Reconfigured (measurements="
			   << MeasurementCollector::getInstance().getMeasurementCount() << ")"
			   <<  Log::cout; */
      }

      // Clear collected statistics
      _calculateStats.clear();
      _patternStats.clear();
      _loopRateStats.clear();

      // Turn on the FBCK PV on Actuators and Measurements
      if (changeMode(statePv, modePv) != 0) {
	// we failed, set feedback to OFF and release the other threads
	statePv = false;
	disconnectDevices(false);
	MeasurementCollector::getInstance().unlock();
	_configuration->_logger << "ERROR: Failed on changeMode()" << Log::cout;
	PatternManager::getInstance().enable();
        PatternManager::getInstance().unlock();
      	return -1;
      }

      // At this point the actuators have read the initial values from
      // the devices, so for loops with multiple patterns (i.e. LTU and
      // Longitudinal) we have to compute the initial actuator offsets
      for (int i = 0; i < numPatterns; ++i) {
	_loops[i]->resetActuatorOffsets();
      }

      _configuration->_configured = true;

      _configuration->_logger << "INFO: Loop reconfigured successfully" << Log::flush;
    }

    //    _configuration->_logger << Log::clearpv;

    // After configuration has finished, clear all pending events in the queue
    // and start fresh :)
    if (getPendingEvents() > 0) {
      //        std::cout << "WARN: discarding " << getPendingEvents()
      //                << " events... (reconfiguring)";
        clear();
	//        std::cout << " done." << std::endl;
    }

    // Reset loop counter
    _configuration->_loopCounter = 0;

    // This used to be called before changeMode() allow updates at the very end
    CollectorThread::getInstance().add(*this);
    MeasurementCollector::getInstance().unlock();
    PatternManager::getInstance().enable();
    PatternManager::getInstance().unlock();

    return 0;
}

/**
 * When a PATTERN_EVENT is received, the processEvent() method is called.
 * processEvent() is a virtual function defined in the class Thread that is
 * implemented by the LoopThread. It is basically a switch statement that routes
 * the PATTERN_EVENT to the processPattern() method.
 *
 * Only the reception of a PATTERN_EVENT that contains a pattern of interest
 * will signal the LoopThread to process the feedback on the next
 * MEASUREMENT_EVENT. A LoopThread can expect multiple patterns.
 *
 * A PATTERN_EVENT received after another PATTERN_EVENT indicates problems with
 * getting measurements. If too many PATTERN_EVENTS are received in a row
 * the Collector thread may be having problems.
 *
 * Multiple POI: If two or more patterns are used then the feedback loop
 * must keep track the offsets between actuator values. These offsets are
 * used when the beam rate changes, e.g. going from 2 patterns to 1 pattern.
 * In this case when the second pattern has beam again the actuators must start
 * from the position last calculated to pattern 1 minus the last calculated
 * offset (when there was beam on pattern 2). THIS LAST COMMENT IS OBSOLETE
 *
 * @param event a PATTERN_EVENT
 * @returns 0 if pattern was processed, -1 if pattern was unexpected or unknown
 * @author L.Piccoli
 */
int LoopThread::processPatternEvent(Event &event) {
  Log::getInstance() << Log::flagPulseId << Log::dpInfo
		     << "INFO: LoopThread::processPatternEvent() PULSEID="
		     << (int) event._pattern.getPulseId() << Log::dp;

    std::vector<Loop *>::iterator it;
    bool knownPattern = false;
    Loop *loop;
    int matchCounter = 0;
    for (it = _loops.begin(); it != _loops.end(); ++it) {
        loop = (*it);
        PatternMask mask = loop->getPatternMask();
        if (event._pattern.match(mask)) {
            knownPattern = true;
            matchCounter++;
        }
    }

    if (!knownPattern) {
        _unknownPatternCount++;
        if (_state == WAITING_PATTERN) {
            return -1;
        }
    }

    if (matchCounter != 1) {
        _configuration->_logger << "ERROR: EVR pattern matched multiple POIs"
                << Log::flush;
        _configuration->_logger << Log::showtime << "Suspicious POI configuration" << Log::flushpvonly;
        return -1;
    }

    if (_state != WAITING_PATTERN) {
      Log::getInstance() << Log::flagEvent << Log::dpWarn
			 << "WARN: LoopThread::processPatternEvent() was waiting for Meas PULSEID="
			 << (int) _pattern.getPulseId()
			 << ", got Patt PULSEID=" << (int) event._pattern.getPulseId() << Log::dp;

        // TODO: If the pattern is known, it means the MEASUREMENT_EVENT for the
        // previous PATTERN_EVENT is late.
        _unexpectedPatternCount++;
	_patternWithoutMeasurementCount++;
	if (_patternWithoutMeasurementCount >= 5 * _configuration->_rate) {
	  _configuration->_logger << Log::showtime << "ERROR: Feedback getting patterns with no measurements. IOC may need a REBOOT."  << Log::flushpvonly;
	}

        //        return -1;
    }

    // Keep pattern statistics (elapsed time between valid patterns)
    _patternStats.end();
    _patternStats.start();

    // Save the pattern for later, to make sure next MEASUREMENT_EVENT is from
    // the same pattern
    _pattern = event._pattern;
    _configuration->_pulseIdPv = _pattern.getPulseId();

    _patternCount++;
    _state = WAITING_MEASUREMENT;
    return 0;
}

/**
 * This method is called when a MEASUREMENT_EVENT is received by the LoopThread.
 *
 * When a MEASUREMENT_EVENT is received, the processEvent() method is called.
 * processEvent() is a virtual function defined in the class Thread that is
 * implemented by the LoopThread. processEvent() is basically a switch statement
 * that routes the MEASUREMENT_EVENT to the processEvent() method.
 *
 * If a MEASUREMENT_EVENT is received after a PATTERN_MEASUREMENT then the
 * pattern timestamp and pulse id must be checked against timestamp and pulse id
 * of the measurements. If there is a match then the feedback calculation can
 * proceed. In case of a mismatch this event should be logged and an alarm
 * dispatched if necessary.
 *
 * If the pattern within the MEASUREMENT_EVENT is not the expected pattern
 * saved in the _pattern attribute the LoopThread should go back to the
 * WAITING_PATTERN state and get the next PATTERN_EVENT.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
bool MULTIPLE_MATCHES = false;

int LoopThread::processMeasurementEvent(Event &event) {
  Log::getInstance() << Log::flagEvent << Log::dpInfo
		     << "INFO: LoopThread::processMeasurementEvent() PULSEID="
		     << (int) event._pattern.getPulseId() << Log::dp;

    if (_state == WAITING_PATTERN) {
      Log::getInstance() << Log::flagEvent << Log::dpWarn
			 << "WARN: LoopThread::processPatternEvent() was waiting for Patt, got Meas PULSEID="
			 << (int) event._pattern.getPulseId() << Log::dp;

      _unexpectedMeasurementCount++;
      return -1;
    }

    // If pattern received here is not the same received previously,
    // then the latest measurements may not correspond to the correct pattern,
    // skip this pattern/measurement and wait for the next matching pattern
    if (_pattern != event._pattern) {
      /*
        Log::getInstance() << "LoopThread got *unexpected pattern* MEAS_EVENT pulseId="
                << (int) event._pattern.getPulseId() << Log::flush;
      */
        _unexpectedMeasurementPatternCount++;
        _state = WAITING_PATTERN;
        return -1;
    }

    // Check if pulseId from PATTERN_EVENT is the same received in the
    // MEASUREMENT_EVENT
    if (_pattern.getPulseId() != event._pattern.getPulseId()) {
        _pulseIdMismatchCount++;
    }

    // Make sure the PulseId from the PATTERN_EVENT is the same
    // one received in the MEASUREMENT_EVENT.
    if (_pattern.getPulseId() != event._pattern.getPulseId()) {
        _unexpectedMeasurementPatternCount++;
        _state = WAITING_PATTERN;
        return -1;
    }

    _measurementCount++;

    // Reset number of patterns received without measurements
    _patternWithoutMeasurementCount = 0;

    std::vector<Loop *>::iterator it;
    Loop *loop;
    bool match = false;
    int loopStatus = 0;
    for (it = _loops.begin(); it != _loops.end(); ++it) {
        loop = (*it);
        PatternMask mask = loop->getPatternMask();
        if (event._pattern.match(mask)) {
            if (match) {
                _configuration->_logger << Log::showtime << "ERROR: Multiple pattern matches"
                        << Log::flushpvonly;
                if (MULTIPLE_MATCHES == false) {
                    std::cout << &event._pattern << std::endl;
                    std::vector<Loop *>::iterator _it = _loops.begin();
                    Loop *l = (*_it);
                    PatternMask m = l->getPatternMask();
                    std::cout << &m;
                    if (event._pattern.match(m)) {
                        std::cout << "MATCH" << std::endl;
                    } else {
                        std::cout << "NO MATCH" << std::endl;
                    }
                    _it++;
                    l = (*_it);
                    PatternMask m2 = l->getPatternMask();
                    std::cout << &m2;
                    if (event._pattern.match(m2)) {
                        std::cout << "MATCH" << std::endl;
                    } else {
                        std::cout << "NO MATCH" << std::endl;
                    }
                    MULTIPLE_MATCHES = true;
                }
            } else {
                // Update _ratePv and _calculateStats only if pattern has beam.
#ifdef CHECK_BEAM
                if (event._pattern.hasBeam()) {
#endif
                    _calculateStats.start();
                    _loopRateStats.end();
                    _ratePv = _loopRateStats.getRate();
		    _configuration->_rate = _ratePv.getValue();
#ifdef CHECK_BEAM
                }
#endif
                loopStatus = loop->calculate(event._pattern);
#ifdef CHECK_BEAM
                if (event._pattern.hasBeam()) {
#endif
                    _calculateStats.end();
                    _loopRateStats.start();
#ifdef CHECK_BEAM
                }
#endif
                match = true;
            }
        }
    }
    if (!match) {
        _patternWitouthLoop++;
    }

    // A pulse mismatch is indicated when a -2 status is returned from 
    // the loop->calculate() method.
    if (loopStatus == -2) {
        if (getPendingEvents() > 0) {
	  PatternManager::getInstance().lock();
	  PatternManager::getInstance().disable();
	  clear();
	  PatternManager::getInstance().enable();
	  PatternManager::getInstance().unlock();
        }
    }

    _state = WAITING_PATTERN;

    // Let EPICS know that PVs can be updated now
    scanIoRequest(_configuration->_scanlist);

    // Count pending events in the LoopThread input queue
    _pendingEventCount += getPendingEvents();

    return 0;
}

/**
 * Read current actuator values. Used when feedback loop returns to ENABLE mode
 * from COMPUTE mode.
 *
 * @return 0
 * @author L.Piccoli
 */
int LoopThread::resetActuators() {
  int status = 0;
  try {
    std::vector<Loop *>::iterator it;
    Loop *loop;
    for (it = _loops.begin(); it != _loops.end(); ++it) {
        loop = (*it);
        if (loop->resetActuators() != 0) {
            status = -1;
        }
    }
  } catch (Exception &e) {
    status = -1;
  }

  return status;
}

void LoopThread::show() {
    //    std::cout << "--- LoopThread " << _name << std::endl;
    std::cout << "  name:          " << _configuration->_loopNamePv.getValue() << std::endl;
    std::cout << "  installed:     ";
    if (_configuration->_installedPv == true) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no (not configured)" << std::endl;
    }
    std::cout << "  LCLS Mode:     ";
    if (ExecConfiguration::getInstance()._lclsModePv.getValue()){
        std::cout << "SC" << std::endl;
    }
    else {
        std::cout << "NC" << std::endl;
    }
    std::cout << "  state:         ";
    if (_enabledPv == true) {
        std::cout << "on" << std::endl;
    } else {
        std::cout << "off" << std::endl;
    }
    std::cout << "  mode:          ";
    if (_modePv == true) {
        std::cout << "enable" << std::endl;
    } else {
        std::cout << "compute" << std::endl;
    }
    //    std::cout << "  algorithm:     " << _configuration->_algorithmNamePv.getValue()
    //            << std::endl;
    std::cout << "  # patterns:    " << _configuration->_totalPoiPv.getValue()
            << std::endl
            << std::endl;
    std::cout << "  -- Loops: " << std::endl;
    int size = _configuration->_totalPoiPv.getValue();
    for (int i = 0; i < size; ++i) {
        std::cout << "  " << i << ") ";
        _loops[i]->show();
    }
    std::cout << "---" << std::endl;
}

void LoopThread::showConfig() {
    std::cout << "  name: " << _configuration->_loopNamePv.getValue() << std::endl;
    std::cout << "  -- Matrices: " << std::endl;
    _configuration->showMatrices();
    std::cout << std::endl;

    std::cout << "  -- Reference Orbit: " << std::endl;
    _configuration->showReferenceOrbit();
    std::cout << std::endl;

    std::cout << "  -- Actuator Energy: " << std::endl;
    _configuration->showActuatorEnergy();
    std::cout << std::endl;

    std::cout << "  -- Dispersion: " << std::endl;
    _configuration->showDispersion();
    std::cout << std::endl;

    std::cout << "---" << std::endl;
}

void LoopThread::showDebug() {
    Thread::showDebug();
    std::cout << "  --" << std::endl;
    std::cout << "  events rcvd:   " << _eventCount << std::endl;
    std::cout << "  events ignored:" << getDiscardedEvents() << std::endl;
    std::cout << "  pending events:" << _pendingEventCount << std::endl;
    std::cout << "  patterns rcvd: " << _patternCount << ", "
            << _unexpectedPatternCount << " (unexpected), "
            << _unknownPatternCount << " (unknown)"
            << std::endl;
    std::cout << "  measurements:  " << _measurementCount << ", "
            << _unexpectedMeasurementCount << " (unexpected), "
            << _unexpectedMeasurementPatternCount << " (unexpected pattern)"
            << std::endl;
    std::cout << "  pulseId mismatches: " << _pulseIdMismatchCount << std::endl;
    std::cout << "  patternWitouthLoop: " << _patternWitouthLoop << std::endl;
    _calculateStats.show("  ");
    _patternStats.show("  ");
    std::cout << "  -- Loops: " << std::endl;
    int size = _configuration->_totalPoiPv.getValue();
    for (int i = 0; i < size; ++i) {
        std::cout << "  " << i << ") ";
        _loops[i]->showDebug();
    }
}

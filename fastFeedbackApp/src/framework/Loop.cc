/* 
 * File:   Loop.cc
 * Author: lpiccoli
 * 
 * Created on June 7, 2010, 3:29 PM
 * Revision on February 18th, 2016 at 3:23 PM (ababbitt)
 * Removing all previous reference to LINUX compiler flag in previous 
 * linux/RTEMS #ifdef/#else token structure
 *
 */

#include "Loop.h"
#include "TimeUtil.h"
#include "ExecConfiguration.h"
#include "PatternManager.h"
#include <evrPattern.h>


USING_FF_NAMESPACE


/**
 * Loop constructor specifying the LoopConfiguration, PatternMask and Algorithm.
 * The set of measurements, actuators and states is retrieved from the
 * LoopConfiguration in the constructor.
 *
 * TODO: remove this constructor
 *
 * @param configuration configuration for this loop
 * @param patternMask the pattern mask for this loop
 * @param algorithm algorithm used by this loop for the specified pattern mask
 *        to calculate new actuator and state values
 * @author L.Piccoli
 */
Loop::Loop(LoopConfiguration *configuration, PatternMask patternMask,
        Algorithm *algorithm) :
_configuration(configuration),
_patternMask(patternMask),
_algorithm(algorithm),
_calculateCount(0),
_calculateFailCount(0),
_deviceFailCount(0),
_actuatorsOutsideLimitsCount(0),
_measurementsOutsideLimitsCount(0),
_actuatorsSkipCount(0),
_lowTmitCount(0),
_pulseIdMismatchCount(0),
_ncMeasBadStatus(0),
_scMeasBadStatus(0),
_actuatorMismatchCount(0),
_calculatePrepStats(50, "Loop Calculate Prep"),
_tmitCheckStats(50, "Loop TMIT check Stats"),
_measCheckStats(50, "Loop Meas check Stats"),
_calculateStats(50, "Loop Processing Stats (calculate)"),
_calculateFailStats(50, "Loop Processing Stats (calculate FAIL)"),
_skipTmit(false),
_firstPass(true),
_previousHadBeam(false) {
  epicsTimeGetCurrent(&_lastActuatorTimestamp);
  _lastActuatorTimestamp.secPastEpoch = 0;
}

/**
 * Loop constructor specifying the LoopConfiguration and PatternMask.
 * The set of measurements, actuators and states is retrieved from the
 * LoopConfiguration in the constructor.
 *
 * The Algorithm used by this loop is retrieved from the LoopConfiguration
 * based on the PatternMask.
 *
 * @param configuration configuration for this loop
 * @param patternMask the pattern mask for this loop
 * @author L.Piccoli
 */
Loop::Loop(LoopConfiguration *configuration, PatternMask patternMask) :
_configuration(configuration),
_patternMask(patternMask),
_calculateCount(0),
_calculateFailCount(0),
_deviceFailCount(0),
_actuatorsOutsideLimitsCount(0),
_measurementsOutsideLimitsCount(0),
_actuatorsSkipCount(0),
_lowTmitCount(0),
_pulseIdMismatchCount(0),
_ncMeasBadStatus(0),
_scMeasBadStatus(0),
_actuatorMismatchCount(0),
_calculatePrepStats(50, "Loop Calculate Prep"),
_tmitCheckStats(50, "Loop TMIT check Stats"),
_measCheckStats(50, "Loop Meas check Stats"),
_calculateStats(50, "Loop Processing Stats (calculate)"),
_calculateFailStats(50, "Loop Processing Stats (calculate FAIL)"),
_skipTmit(false),
_firstPass(true),
_previousHadBeam(false) {
    // Get the algorithm and configure it
    configureAlgorithm();
  epicsTimeGetCurrent(&_lastActuatorTimestamp);
}

Loop::~Loop() {
  clearUsedDevices<MeasurementSet, MeasurementSet::iterator >(_measurements);
  clearUsedDevices<ActuatorSet, ActuatorSet::iterator >(_actuators);
}

/**
 * Configure the loop - calls Loop::reconfigure() passing its own _patternMask
 * as parameter.
 *
 * @return 0 on success -1 on failure
 * @author L.Piccoli
 */
int Loop::configureAlgorithm() {
    return reconfigure(_patternMask);
}

/**
 * Configure the loop using the passed patternMask. This method selects only
 * those devices that are enabled (USED PV) to be added to the list of
 * measurements, actuators and states used for the feedback calculations.
 *
 * The selected measurements are also added to the MeasurementCollector
 * singleton, which reads new values at the beginning of each feedback cycle.
 *
 * Finally, the algorithm is also configured, i.e. the selected devices are
 * passed down to the algorithm.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int Loop::reconfigure(PatternMask patternMask) {
    _patternMask = patternMask;

    _configuration->_logger << Log::showtime << "Reconfiguring Loop "
			    << _configuration->_loopNamePv.getValue().c_str()
			    << Log::flushpvnoalarm;

    // If the selected algorithm is the Longitudinal or the 
    if (_configuration->_algorithmNamePv == LONGITUDINAL_ALGORITHM ||
	_configuration->_algorithmNamePv == LONGITUDINAL_CHIRP_ALGORITHM ||
	_configuration->_algorithmNamePv == INJECTOR_LAUNCH_ALGORITHM) {
      _configuration->_selectAllDevicesPv = true;
    }

    // Select only the subset of devices that are actually used
    // Select MeasurementDevices
    if (selectUsedDevices<MeasurementMap, MeasurementMap::iterator,
            MeasurementSet, MeasurementSet::iterator >
            (_measurements, _configuration->_measurements,
            CommunicationChannel::READ_ONLY,
            _configuration->_measNumPv.getValue()) != 0) {

      clearUsedDevices<MeasurementSet, MeasurementSet::iterator >(_measurements);

      _configuration->_logger << "ERROR: Select used measurement devices" << Log::cout;

        return -1;
    }

    // Select ActuatorDevices
    if (selectUsedDevices<ActuatorMap, ActuatorMap::iterator,
            ActuatorSet, ActuatorSet::iterator >
            (_actuators, _configuration->_actuators,
            CommunicationChannel::WRITE_ONLY,
            _configuration->_actNumPv.getValue()) != 0) {

      clearUsedDevices<MeasurementSet, MeasurementSet::iterator >(_measurements);
      clearUsedDevices<ActuatorSet, ActuatorSet::iterator >(_actuators);

      _configuration->_logger << "ERROR: Select used actuator devices" << Log::cout;
        return -1;
    }
    
    // Configure MeasurementDevices
    if (configureUsedDevices<MeasurementSet,
	MeasurementSet::iterator >(_measurements, CommunicationChannel::READ_ONLY) != 0) {
      _configuration->_logger << "ERROR: Failed to configure measurement devices" << Log::cout;

      clearUsedDevices<MeasurementSet, MeasurementSet::iterator >(_measurements);
      clearUsedDevices<ActuatorSet, ActuatorSet::iterator >(_actuators);

      return -1;
    }

    // Configure ActuatorDevices
    if (configureUsedDevices<ActuatorSet,
	ActuatorSet::iterator >(_actuators, CommunicationChannel::WRITE_ONLY) != 0) {
      _configuration->_logger << "ERROR: Failed to configure actuator devices" << Log::cout;

      clearUsedDevices<MeasurementSet, MeasurementSet::iterator >(_measurements);
      clearUsedDevices<ActuatorSet, ActuatorSet::iterator >(_actuators);

      return -1;
    }

    // Select StateDevices
    // All code now needs to be able to use FCOM channel to output state values
    // with linux and linuxRT operating systems
    CommunicationChannel::AccessType accessType = CommunicationChannel::WRITE_ONLY;

    if (selectUsedDevices<StateMap, StateMap::iterator,
	StateSet, StateSet::iterator >
	(_states, _configuration->_states, accessType,
	 _configuration->_stateNumPv.getValue()) != 0) {

      clearUsedDevices<MeasurementSet, MeasurementSet::iterator >(_measurements);
      clearUsedDevices<ActuatorSet, ActuatorSet::iterator >(_actuators);

      _configuration->_logger << "ERROR: Select used state devices" << Log::cout;
        return -1;
    }

    // Configure StateDevices
    if (configureUsedDevices<StateSet,
	StateSet::iterator >(_states, CommunicationChannel::WRITE_ONLY) != 0) {
      _configuration->_logger << "ERROR: Failed to configure state devices" << Log::cout;

      clearUsedDevices<MeasurementSet, MeasurementSet::iterator >(_measurements);
      clearUsedDevices<ActuatorSet, ActuatorSet::iterator >(_actuators);

      return -1;
    }

    // Add selected measurement devices to the MeasurementCollector list
    // TODO: can't we remove only those not used and add only the new ones?
    MeasurementSet::iterator it;
    for (it = _measurements.begin(); it != _measurements.end(); ++it) {
        if (MeasurementCollector::getInstance().add(*it) != 0) {
            _configuration->_logger << "ERROR: Failed to add device to collector"
                    << Log::flush;
        }
    }

    // Get the algorithm and configure it
    AlgorithmMap::iterator algoIterator;
    algoIterator = _configuration->_algorithms.find(patternMask);
    if (algoIterator == _configuration->_algorithms.end()) {
        _algorithm = NULL;
    } else {
        _algorithm = algoIterator->second;
	try {
	  _algorithm->configure(_configuration, &_measurements,
				&_actuators, &_states);
	} catch (Exception &e) {
	  _configuration->_logger << Log::showtime << "Failed to configure ("
				  << e.what() << "). "
				  << "Please check configuration." << Log::flushpv;
	  _configuration->_logger << "ERROR: Failed to configure" << Log::cout;
	  return -1;
	}
    }

    // If measurements are read from files, skip TMIT checking
    if (_measurements.size() > 0) {
        MeasurementSet::iterator it;
        it = _measurements.begin();
        MeasurementDevice *measurementDevice;
        measurementDevice = *it;
        if (measurementDevice->isFile()) {
            _skipTmit = true;
        }
    }

    // Flag calculate() to get the latest measurements
    _firstPass = true;

    return 0;
}

/**
 * Template method used for selecting only those devices that have been selected
 * durning configuration time. The selected devices are added to the specified
 * device set.
 *
 * The selected devices also are configured, i.e. the proper communication
 * channel is instantiated.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
template<class DeviceMap, class DeviceMapIterator, class DeviceSet,
class DeviceSetIterator>
int Loop::selectUsedDevices(DeviceSet &deviceSet, DeviceMap &deviceMap,
CommunicationChannel::AccessType accessType, long maxDevices) {
    DeviceMapIterator mapIterator;
    mapIterator = deviceMap.find(_patternMask);
    if (mapIterator == deviceMap.end()) {
      _configuration->_logger << "ERROR: Empty map" << Log::cout;
      return -1;
    }
    DeviceSet *fullDeviceSet = mapIterator->second;

    // Find what is the pattern index for the pattern mask of this Loop
    // -> Needed for devices using FileChannel
    int patternIndex = 1;
    if (_configuration->_totalPoiPv == 1) {
        patternIndex = -1;
    }

    Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
		       << "INFO: Loop::selectUsedDevices() selecting devices form set (with "
		       << (int)fullDeviceSet->size() << ") DEVNUM="
		       << (int)maxDevices << Log::dp;
    Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
		       << "INFO: Loop::selectUsedDevices() number of devices in set is "
		       << (int)deviceSet.size() << Log::dp;


    // Step 1: Select the used devices from the full list
    long deviceCount = 0;
    int i = 0;
    for (DeviceSetIterator devIterator = fullDeviceSet->begin();
            devIterator != fullDeviceSet->end(); ++devIterator, ++i) {
      if (deviceCount < maxDevices) {
        // If Loop is configured with longitudital algorithm, then
        // select all devices available, even if xxUSED PV is set to false
        // Maximum allowed number of selected devices is given by the
        // xxxNUM PVs (e.g. MEASUSED)
	bool selectDevice = false;
	// Only use devices that have a non-empty name
	Log::getInstance() << Log::flagConfig << Log::dpInfo
			   << "INFO: Loop::selectUsedDevices() device [" << i
			   << " - " << (*devIterator)->getDeviceName().c_str() << "] ";
	
	if ((*devIterator)->getDeviceName().size() > 2) {
	  selectDevice = (*devIterator)->isUsed();
	  if (!selectDevice) {
	    Log::getInstance() << " ALL=";
	    if (_configuration->_selectAllDevicesPv.getValue() == true) {
	      Log::getInstance() << "TRUE ";
	      selectDevice = true;
	    }
	    else {
	      Log::getInstance() << "FALSE ";
	      selectDevice = false;
	    }
	  }
	}
	
	if (selectDevice) {
	  Log::getInstance() << "USED=" << "YES" << Log::dp;
	}
	else {
	  Log::getInstance() << "USED=" << "NO" << Log::dp;
	}
	
	if (selectDevice) {
	  deviceSet.insert(*devIterator);
          deviceCount++;
	}
      }
    }

    return 0;
}

// Step 2: configure selected devices
template<class DeviceSet, class DeviceSetIterator>
int Loop::configureUsedDevices(DeviceSet &deviceSet,
															 CommunicationChannel::AccessType accessType) {
	Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
			<< "INFO: Loop::configureUsedDevices() number of devices in set is "
			<< (int)deviceSet.size() << Log::dp;

	// Find what is the pattern index for the pattern mask of this Loop
	// -> Needed for devices using FileChannel
	int patternIndex = 1;
	if (_configuration->_totalPoiPv == 1) {
		patternIndex = -1;
	}

	for (DeviceSetIterator devIterator = deviceSet.begin();
			devIterator != deviceSet.end(); ++devIterator) {
		Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
				<< "INFO: Loop::configureUsedDevices() device ["
				<< (*devIterator)->getDeviceName().c_str() << "] ";

		_configuration->_logger << Log::showtime << "Connecting to "
				<< (*devIterator)->getDeviceName().c_str();


		if (!(*devIterator)->isFcom() && !(*devIterator)->isFile()) {
			_configuration->_logger << " (CA timeout of 2 minutes...) ";
			Log::getInstance() << " (CA timeout of 2 minutes...) ";
		} else if (!(*devIterator)->isFile()) {
			_configuration->_logger << " (Fcom) ";
			Log::getInstance() << " (Fcom) ";
		} else {
			_configuration->_logger << " (File) ";
			if ((*devIterator)->isFile()) {
				Log::getInstance() << " (File) ";
			}
		}
		_configuration->_logger << Log::flushpvnoalarm;
		Log::getInstance() << Log::dp;

		try {
			(*devIterator)->configure(accessType, patternIndex);
		} catch (Exception &e) {
			_configuration->_logger << Log::showtime << e.what() << "("
					<< (*devIterator)->getDeviceName().c_str()
					<< "). "
					<< "Push \"Start\" to retry." << Log::flushpv;
			_configuration->_logger << Log::showtime << "ERROR: Failed to configure device: "
					<< e.what() << " "
					<< (*devIterator)->getDeviceName().c_str()
					<< Log::cout;
			return -1;
		}
	}

	return 0;
}

template<class DeviceSet, class DeviceSetIterator>
int Loop::clearUsedDevices(DeviceSet &deviceSet) {
  Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
		     << "INFO: Loop::clearUsedDevices() number of devices in set is "
		     << (int)deviceSet.size() << Log::dp;

  // Must remove devices from the deviceSet and disconnect them!
  while (!deviceSet.empty()) {
    DeviceSetIterator devIt = deviceSet.begin();

    Log::getInstance() << Log::flagConfig << Log::dpInfo << Log::showtime
		       << "INFO: disconnection/erasing device "
		       << (*devIt)->getDeviceName().c_str()
		       << Log::dp;

    (*devIt)->disconnect();
    deviceSet.erase(devIt);
  }
  return 0;
}

template<class Device, class DeviceSet, class DeviceSetIterator>
void Loop::setGroupLimit(DeviceSet &deviceSet) {
  /*
  DeviceSetIterator it;
  for (it = deviceSet.begin();
       it != deviceSet.end(); ++it) {
    Device *device = (*it);
    device->setGroupLimit();
  }
  */
}

/**
 * This check is called periodically to make sure FCOM is properly
 * configured in case the loop is supposed to use it to set actuators.
 * The LTU and Longitudinal are the two feedbacks that control actuators
 * using FCOM.
 *
 * The reason for this method is that for two times at least a
 * production version was released with the wrong compiler flags,
 * that end up disabling FCOM, therefore making the feedbacks
 * useless.
 */
void Loop::checkFcom() {
    ActuatorSet::iterator actIt;
    bool pulseMismatch = false;
    for (actIt = _actuators.begin();
            actIt != _actuators.end(); ++actIt) {
        ActuatorDevice *actuator = (*actIt);
	if (actuator->isFcom() && !actuator->isSetFbckPvEnabled()) {
	  _configuration->_logger << Log::showtime << "Possible feedback FCOM configuration problem (contact L.Piccoli)" << Log::flushpv;
	  return;
	}
    }
}

/**
 * Invoke the algorithm to calculate new actuator and state values. The new
 * values are set() by the algorithm but are not sent to the actual devices.
 *
 * The Loop is responsible for making sure the new values are correct and
 * can be sent out.
 *
 * @param pattern the pattern received from the EVR and passed along the
 * PATTERN_EVENT
 * @return 0 on success, -1 on failure or -2 if there is an error due to a
 * pulse id mismatch.
 * @author L.Piccoli
 */
int Loop::calculate(Pattern pattern) {
    //    _configuration->_loopCounter = _configuration->_loopCounter.getValue() + 1;

  _calculatePrepStats.start();

    // If there is no beam, this loop must be running on the second pattern.
    // In this case, the actuator will receive the last value calculated for
    // the first pattern plus the offset between patterns. If there is beam,
    // then the actuator value is calculated.
    if (!pattern.hasBeam() && !_configuration->_fakeBeam) {
        _algorithm->setActuatorsReference();
        return 0;
    }

    if (_algorithm == NULL) {
        _configuration->_logger << "ERROR: Loop has NULL algorithm" << Log::flush;
        return -1;
    }

    if (_firstPass) {
        resetMeasurements();
        _firstPass = false;
    }

    _calculatePrepStats.end();
    _tmitCheckStats.start();

    // TMIT is not checked if Longitudital/BunchCharge is the used algorithm
    // _skipTmitCheck is set when the Loop is (re)configured
    if (!_configuration->_skipTmitCheck) {
        if (checkTmit(pattern) != 0) {	  
	  _lowTmitCount++;
	  return -1;
        }
    }

    _tmitCheckStats.end();
    _measCheckStats.start();

    // Check if there is at least one Measurement with status different than
    // DataPoint::READ. In that case the latest measurements are read out,
    // discarded and the calculation returns an error.
    if (checkMeasurementStatus(pattern.getPulseId()) != 0) {
        discardLatestMeasurements();
        return -2;
    }

    _measCheckStats.end();
    _calculateStats.start();
    _calculateFailStats.start();
    int status = 0;
    try {
      status = _algorithm->calculate(*_configuration, _measurements,
				     _actuators, _states);
      _calculateStats.end();
    } catch (Exception &e) {
      _calculateFailStats.end();
      _configuration->_logger << Log::showtime << e.what() << Log::flushpvonly;
      switch (e.getCode()) {
      case Exception::STATE_LIMIT:
	setGroupLimit<StateDevice, StateSet, StateSet::iterator>(_states);
      case Exception::ACTUATOR_LIMIT:
	setGroupLimit<ActuatorDevice, ActuatorSet, ActuatorSet::iterator>(_actuators);
      default:
	status = 100;
      }
    }


    // At this point, if _algorithm->calculate() took too long to finish then
    // there may be several PATTERN_EVENT and MEASUREMENT_EVENT queued up.
    // Writing to actuators must be skipped in this case, otherwise they will
    // receive values based on measurements from some cycles in the past.
    //
    // A potential problem with the clear() call is that it is not atomic,
    // the queue may not be totally cleared because PATTERN_EVENT and
    // MEASUREMENT_EVENT are still being generated.
    //
    // The current solution is to DISABLE the PatternManager while the 
    // queue is cleaned up. THIS AFFECTS ALL LOOPS IN THE IOC! 
    bool skip = false;
    if (_configuration->_eventReceiver->getPendingEvents() > 0) {
        skip = true;
	PatternManager::getInstance().lock();
	PatternManager::getInstance().disable();
        _configuration->_eventReceiver->clear();
	PatternManager::getInstance().enable();
	PatternManager::getInstance().unlock();
    }

    if (status == 0) {
        status = setDevices(skip);
        if (status == 0) {
            _calculateCount++;
            return 0;
        } else {
            _deviceFailCount++;
        }
    } else {
        // If STATE_LIMIT was hit, do not apply correction
        if (status == STATE_LIMIT) {
	  _configuration->_logger << Log::showtime << "State values out of range, no change"
				  << Log::flushpvonly;
	    /*
            StateSet::iterator sIt;
            for (sIt = _states.begin();
                    sIt != _states.end(); ++sIt) {
                StateDevice *state = (*sIt);
                state->setGroupLimit();
            }
            ActuatorSet::iterator it;
            for (it = _actuators.begin();
                    it != _actuators.end(); ++it) {
                ActuatorDevice *actuator = (*it);
                actuator->setGroupLimit();
            }
	    */
        } else {
            // If ACTUATOR_LIMIT was hit do not apply correction
            if (status == ACTUATOR_LIMIT) {
                _configuration->_logger << Log::showtime << "Actuator values out of range, no change"
                        << Log::flushpvonly;
                _actuatorsOutsideLimitsCount++;
		/*
                ActuatorSet::iterator it;
                for (it = _actuators.begin();
                        it != _actuators.end(); ++it) {
                    ActuatorDevice *actuator = (*it);
                    actuator->setGroupLimit();
                }
		*/
            }
        }
    }
    _calculateFailCount++;
    return status;
}

/**
 * Check whether there is any measurument device whose TMIT is below the
 * acceptable threshold. This means there is no beam and the feedback calculation
 * should not take place.
 *
 * @return 0 if TMIT values are above limit, -1 if there is at least one value
 * below the threshold (which is given by SIOC:SYS0:FB00:TMITLOW PV)
 * @author L.Piccoli
 */
int Loop::checkTmit(Pattern &pattern) {
    if (_skipTmit) {
        return 0;
    }

    MeasurementSet::iterator it;
    MeasurementDevice *measurementDevice;
    double lastLowTmit = -1;
    double firstLowTmit = -1;
    epicsUInt32 firstPulseId = 0;
    std::string firstLowDevice = "";

    int lowTmitBpms = 0;
    for (it = _measurements.begin(); it != _measurements.end(); ++it) {
        measurementDevice = *it;
        if (measurementDevice->getTmit() <=
                ExecConfiguration::getInstance()._tmitLowPv.getValue()) {
	  if (lowTmitBpms == 0) {
	    firstLowDevice = measurementDevice->getDeviceName();
	  }
	  lowTmitBpms++;
	  lastLowTmit = measurementDevice->getTmit();
	  if (firstLowTmit < 0) {
	    firstLowTmit = lastLowTmit;
	    firstPulseId = pattern.getPulseId();
	  }
        }
    }
    if (lowTmitBpms > 0) {
	  _configuration->_logger << Log::showtime << "TMIT too low on " 
				  << firstLowDevice.c_str() << ", no change (";
	  if (pattern.isTs1()) {
	    _configuration->_logger << "TS1";
	  }
	  else {
	    _configuration->_logger << "TS4";
	  }
	  _configuration->_logger << ") " << firstLowTmit << " " 
				  << (int) firstPulseId << Log::flushpvonly;

        // must skip Measurements on this cycle
        discardLatestMeasurements();
        return -1;
    }
    return 0;
}

/**
 * This method is called by calculate() after the algorithm has updated the
 * actuators and measurements.
 *
 * Values are sent out to the devices using the default communication channel.
 *
 * The PulseId of the actuators are checked against the current PulseId.
 *
 * @return 0 on success, -1 if there is a problem sending data out to at least
 * one device.
 * @author L.Piccoli
 */
int Loop::setDevices(bool skip) {
    // If Loop MODE is set to Compute (false), then skip actuator setting.
    bool send = true;

    // If Look is configured to skip only if in compute mode, then set 
    // skip to false and let the _configuration->_mode control whether
    // the actuator will be skip'd or not
    if (_configuration->_skipOnlyCompute) {
      skip = false;
    }

    if (_configuration->_mode == false || skip) {
        send = false;
        _actuatorsSkipCount++;
    }

    // Check if PulseId is current (RTEMS only)
    ActuatorSet::iterator actIt;
//#ifdef RTEMS
    bool pulseMismatch = false;
    for (actIt = _actuators.begin();
            actIt != _actuators.end(); ++actIt) {
        ActuatorDevice *actuator = (*actIt);
        // Only check the PulseId if datum is going to be written out
        if (actuator->peekStatus() == DataPoint::SET) {
            if (actuator->peekPulseId() != _configuration->_pulseIdPv.getValue()) {
                pulseMismatch = true;
                std::cout << actuator->peekPulseId() << " != " << _configuration->_pulseIdPv.getValue() << std::endl;
                _configuration->_logger << Log::showtime << "Actuator pulse id mismatch, no change"
                        << Log::flushpvonly;
                // TODO: If there is a mismatch, then we better check for LoopThread
                // input queue. There may be events not handled and out of sync
            }
        }
    }

    if (pulseMismatch) {
        send = false;
        _actuatorMismatchCount++;
    }
//#endif

    epicsTimeStamp timestamp;
    epicsTimeGetCurrent(&timestamp);

    epicsTimeStamp bsaTS;
	  evrTimeGet(&bsaTS, 0);    

    bool actuatorSettingFailed = false;

    for (actIt = _actuators.begin();
            actIt != _actuators.end(); ++actIt) {
      if ((*actIt)->write(send) == 0) {
	if (_lastActuatorTimestamp.secPastEpoch == 0) {
	  _lastActuatorTimestamp = timestamp;
	}
	else {
	  // Update PV if last set happened after 5 seconds...
	  if (_lastActuatorTimestamp.secPastEpoch < (timestamp.secPastEpoch - 5)) {
	    epicsTimeToStrftime(_timeString, sizeof(_timeString), "%Y/%m/%d %H:%M:%S", &timestamp);
	    _configuration->_actuatorLastUpdatePv = _timeString;
	    _configuration->_actuatorLastUpdatePv.scanIoRequest();
	    _lastActuatorTimestamp = timestamp;
	  }

	  // If last update happened more then 30 seconds ago set an alarm...
	  if (_lastActuatorTimestamp.secPastEpoch < (timestamp.secPastEpoch - 30)) {
	    epicsTimeToStrftime(_timeString, sizeof(_timeString), "%Y/%m/%d %H:%M:%S", &_lastActuatorTimestamp);
	    _configuration->_logger << Log::showtime << "Last actuator set on " << _timeString << ". Feedback may be stuck."
				    << Log::flushpvonly;
	  }
	}
      }
    }

    // Write state values to FcomBlob
    StateSet::iterator stateIt;
    for (stateIt = _states.begin();
            stateIt != _states.end(); ++stateIt) {
        (*stateIt)->write(bsaTS); // TODO: need to implement skip in StateDevice::write (see ActuatorDevice::write(bool))
    }

    // Send the Fcom blob with states
    stateIt = _states.begin();

    epicsTimeGetCurrent(&timestamp);
    evrTimePutPulseID(&timestamp, _configuration->_pulseIdPv.getValue());
    (*stateIt)->writeFcom(timestamp);	

    // If actuators were set, then clear stale status message (after 5 seconds)
    if (!actuatorSettingFailed) {
      _configuration->_clearStatusCounter++;
      if (_configuration->_clearStatusCounter >= 5 * _configuration->_rate) {
	_configuration->_logger << Log::clearpv;
	_configuration->_clearStatusCounter = 0;
      }
    }

    return 0;
}

/**
 * Prints out information about the Loop.
 * 
 * @author L.Piccoli
 */
void Loop::showDebug() {
    std::cout << &_patternMask << std::endl;
    _calculatePrepStats.show("    ");
    _tmitCheckStats.show("    ");
    _measCheckStats.show("    ");
    _calculateStats.show("    ");
    _calculateFailStats.show("    ");
    std::cout << "     -> Algorithm: ";
    if (_algorithm != NULL) {
        std::cout << _algorithm->getName() << std::endl;
        _algorithm->showDebug();
    } else {
        std::cout << "none" << std::endl;
    }
}

/**
 * Prints out information about the Loop.
 * 
 * @author L.Piccoli
 */
void Loop::show() {
    std::cout << &_patternMask << std::endl;
    std::cout << "     -> Algorithm: ";
    if (_algorithm != NULL) {
        std::cout << _algorithm->getName() << std::endl;
        _algorithm->show();
    } else {
        std::cout << "none" << std::endl;
    }
    std::cout << "     -> Counters (ok: " << _calculateCount
            << ", fail: " << _calculateFailCount
            << ", set fail: " << _deviceFailCount
            << ", act lim: " << _actuatorsOutsideLimitsCount
            << ", act skip: " << _actuatorsSkipCount
            << ", low tmit: " << _lowTmitCount
            << ", NC meas status: " << _ncMeasBadStatus
            << ", SC meas status: " << _scMeasBadStatus
            << ", ts mismatch: " << _pulseIdMismatchCount
            << ", act mismatch: " << _actuatorMismatchCount
            << ")" << std::endl;
    _calculateStats.show("    ");
    _calculateFailStats.show("    ");
}

/**
 * Set the FBCK PV to ON or OFF for all selected Actuators and Measurements,
 * according to the specified parameters.
 *
 * TODO: the setting of the FBCK PV should go in the Algorithm class.
 *
 * @param measurements true indicates measurement FBCK PVs should be turned ON,
 * if false FBCK PV is turned OFF
 * @param actuators true indicates actuator FBCK PVs should be turned ON,
 * if false FBCK PV is turned OFF
 * @return 0 if all PVs were set, -1 if there was an error setting one PV.
 * @author L.Piccoli
 */
int Loop::setFeedbackDevices(bool measurements, bool actuators) {
  if (_algorithm != NULL) {
    return _algorithm->setFeedbackDevices(measurements, actuators);
  }
  return -1;
}

/** 
 * Disconnect all devices used by the Loop. This method is used when
 * the STATE of a Loop goes from ON to OFF.
 *
 * Devices will be reconnected when the STATE transitions from OFF to ON.
 *
 * MeasurementDevices are also removed from the MeasurementCollector.
 *
 * @author L.Piccoli
 */
void Loop::disconnectDevices() {
    ActuatorSet::iterator actIt;
    for (actIt = _actuators.begin(); actIt != _actuators.end(); ++actIt) {
        ActuatorDevice *actuator = *actIt;
        actuator->disconnect();
    }

    MeasurementSet::iterator measIt;
    for (measIt = _measurements.begin(); measIt != _measurements.end(); ++measIt) {
        MeasurementDevice *measurement = *measIt;
        measurement->disconnect();
    }

    MeasurementSet::iterator it;
    for (it = _measurements.begin(); it != _measurements.end(); ++it) {
        MeasurementCollector::getInstance().remove(*it);
    }
}

/**
 * Check if at least one of the latest available measurements have a status
 * indicating a problem with the value.
 *
 * @param patternPulseId the pulseId for the current pattern, which must match
 * the pulse id of the measurement.
 * @return 0 if all measurements have DataPoint::READ status (i.e. ready to
 * be read), -1 if there is at least one value with status other than
 * DataPoint::READ or there is a pulseId mismatch
 * @author L.Piccoli
 */
int Loop::checkMeasurementStatus(epicsUInt32 patternPulseId) {

  Log::getInstance() << Log::flagPulseId << Log::dpInfo
		     << "Loop::checkMeasStatus(PULSEID=" << (int) patternPulseId
		     << ", # measurements=" << (int) _measurements.size() << Log::dp;

    MeasurementSet::iterator measIt;
    for (measIt = _measurements.begin(); measIt != _measurements.end(); ++measIt) {
        MeasurementDevice *measurement = *measIt;
	// Do not check PULSEID if NULL communication channel is used
    if (!measurement->getFacMode()){
        measurement->setMeasCheckInclusion(true);
	    if (!measurement->isNull()) {
	        if (measurement->peekStatus() != DataPoint::READ) {
	            Log::getInstance() << Log::flagBuffer << Log::dpInfo
			           << "ERROR: Measurement "
			           << measurement->getDeviceName().c_str()
			           << " status is "
			           << (int) measurement->peekStatus() << " (PULSEID="
			           << (int) measurement->peekPulseId() << "/" 
			           << (int) patternPulseId << ")" << Log::dp;
                measurement->setMeasStatus(false);
	            _ncMeasBadStatus++;
                return -1;
	        }

            else {
              measurement->setMeasStatus(true);
            }
	    if (measurement->isFcom() && !measurement->isFile()) {
            if (measurement->peekPulseId() != patternPulseId) {
                measurement->setMeasStatus(false);
	            _pulseIdMismatchCount++;	     
	            //	      _configuration->_logger << Log::showtime << "PulseId mismatch, skipping this cycle."
	            //				      << Log::flushpvonlynoalarm;
	      
	            return -1;
            }
        
            else {
                measurement->setMeasStatus(true);
            }
	    }
	}
    }
    else {
        measurement->setMeasCheckInclusion(false);
        //_scMeasBadStatus++;
    }
    }

    return 0;
}

/**
 * This method is invoked if at least one of the latest measurements has
 * invalid status.
 *
 * @author L.Piccoli
 */
void Loop::discardLatestMeasurements() {
    MeasurementSet::iterator it;
    for (MeasurementSet::iterator it = _measurements.begin();
            it != _measurements.end(); ++it) {
        double meas;
        epicsTimeStamp timestamp;
        MeasurementDevice *measurement = (*it);
	measurement->get(meas, timestamp);
	//measurement->clear(); // CAREFUL HERE
    }
}

/**
 * This method is called by calculate() when it is invoked for the first
 * time. MeasurementDevices may have been read several times before
 * Loop::calculate() runs, meaning that old measurements my be used
 * for the calculations.
 *
 * @author L.Piccoli
 */
void Loop::resetMeasurements() {
    MeasurementSet::iterator it;
    for (MeasurementSet::iterator it = _measurements.begin();
            it != _measurements.end(); ++it) {
        MeasurementDevice *measurement = (*it);
        measurement->resetNextRead();
    }
}

/**
 * This method is invoked when the $(LOOP):MODE PV switches from
 * COMPUTE to ENABLE, iff $(LOOP):STATE is ON.
 * 
 * Current actuator values are retrieved from the respective IOCs
 * such that the feedback loop starts controlling actuators from their
 * current values.
 * 
 * @return 0
 * @author L.Piccoli
 */
int Loop::resetActuators() {
    ActuatorSet::iterator it;
    int status = 0;
    for (ActuatorSet::iterator it = _actuators.begin();
            it != _actuators.end(); ++it) {
        ActuatorDevice *actuator = (*it);
        if (actuator->getInitialSetting() < 0) {
            status = -1;
        }
    }
    return status;
}

int Loop::resetActuatorOffsets() {
    for (ActuatorSet::iterator it = _actuators.begin();
            it != _actuators.end(); ++it) {
        ActuatorDevice *actuator = (*it);
        actuator->setInitialOffset();
    }
    return 0;
}

/**
 * This method invokes the reset() method of the used algorithm. It is
 * invoked by the LoopThread when the $(LOOP):MODE PV switches from
 * COMPUTE to ENALE.
 *
 * @return 0
 * @author L.Piccoli
 */
int Loop::resetAlgorithm() {
    if (_algorithm != NULL) {
        _algorithm->reset();
    }

    return 0;
}

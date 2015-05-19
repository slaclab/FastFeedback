/* 
 * File:   BunchCharge.cc
 * Author: lpiccoli
 * 
 * Created on June 7, 2010, 3:59 PM
 */

#include <iostream>
#include "BunchCharge.h"
#include "ExecConfiguration.h"

USING_FF_NAMESPACE

BunchCharge::BunchCharge() :
  Algorithm("BunchCharge"),
  _m1Average(10, "M1 Measurement Average"),
  _m1(NULL),
  _s1(NULL),
  _a1(NULL),
  _goodMeasurementsCount(0),
  _lowTmitMeasurementsCount(0),
  _actuatorSkipCount(0),
  _mismatchPulseIdMeasurements(0),
  _laserWaveplateStatusChannel(NULL),
  _laserWaveplateDoneStatusChannel(NULL),
  _cameraWaveplateStatusChannel(NULL),
  _cameraWaveplateDoneStatusChannel(NULL),
  _laserPowerReadbackChannel(NULL) {
  // First connect to waveplate PVs
  if (_laserWaveplateStatusChannel == NULL) {
    _laserWaveplateStatusChannel = new CaChannel(CommunicationChannel::READ_ONLY,
						 "WPLT:LR20:116:WP2_ANGLEXPS_STATUS");
  }

  if (_laserWaveplateDoneStatusChannel == NULL) {
    _laserWaveplateDoneStatusChannel = new CaChannel(CommunicationChannel::READ_ONLY,
						     "WPLT:LR20:116:WP2_ANGLE.DMOV");
  }

  if (_cameraWaveplateStatusChannel == NULL) {
    _cameraWaveplateStatusChannel = new CaChannel(CommunicationChannel::READ_ONLY,
						  "WPLT:IN20:181:VCC_ANGLEXPS_STATUS");
  }

  if (_cameraWaveplateDoneStatusChannel == NULL) {
    _cameraWaveplateDoneStatusChannel = new CaChannel(CommunicationChannel::READ_ONLY,
						      "WPLT:IN20:181:VCC_ANGLE.DMOV");
  }

  if (_laserPowerReadbackChannel == NULL) {
    _laserPowerReadbackChannel = new CaChannel(CommunicationChannel::READ_ONLY,
					       "IOC:IN20:LS11:LASER_PWR_READBACK");
  }
};

BunchCharge::~BunchCharge() {
  if (_laserWaveplateStatusChannel != NULL) {
    delete _laserWaveplateStatusChannel;
  }
  if (_laserWaveplateDoneStatusChannel != NULL) {
    delete _laserWaveplateDoneStatusChannel;
  }
  if (_cameraWaveplateStatusChannel != NULL) {
    delete _cameraWaveplateStatusChannel;
  }
  if (_cameraWaveplateDoneStatusChannel != NULL) {
    delete _cameraWaveplateDoneStatusChannel;
  }
  if (_laserPowerReadbackChannel != NULL) {
    delete _laserPowerReadbackChannel;
  }
};

/**
 * @param configuration pointer to the LoopConfiguration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int BunchCharge::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) throw (Exception) {

  Algorithm::configure(configuration, measurements, actuators, states);

  _m1Average.clear();

  _m1 = (*(_measurements->begin()));
  _s1 = (*(_states->begin()));
  _a1 = (*(_actuators->begin()));

  /** Allows the Loop to set actuator even when calculate takes a long time */
  configuration->_skipOnlyCompute = true;

  //  _a1->setInitialChannel(_laserPowerReadbackChannel);
  _a1->setInitialChannel(&(ExecConfiguration::getInstance()._laserPowerReadbackPv));

  return 0;
}

/**
 * Check if both laser and camera waveplates are done moving.
 *
 * @return true if both waveplates are done moving, false if one or both
 * are moving
 */
bool BunchCharge::doneMoving() {
  double laserDoneMoving = 0;
  double cameraDoneMoving = 0;
  double laserStatus = 0;
  double cameraStatus = 0;
  epicsTimeStamp timestamp;

  _laserWaveplateDoneStatusChannel->read(laserDoneMoving, timestamp);
  _cameraWaveplateDoneStatusChannel->read(cameraDoneMoving, timestamp);
  _laserWaveplateStatusChannel->read(laserStatus, timestamp);
  _cameraWaveplateStatusChannel->read(cameraStatus, timestamp);

  if (laserDoneMoving == 1 && cameraDoneMoving == 1 &&
      (laserStatus >= 10 || laserStatus <= 18) &&
      (cameraStatus >= 10 || cameraStatus <= 18)) {
    return true;
  }
  else {
    return false;
  }
}

/**
 * Get measurements and copy them to internal vectors.
 */
int BunchCharge::getMeasurements() {
    if (_measurements == NULL || _m1 == NULL || _measurements->size() == 0) {
      throw Exception("TrajectoryFit: no measurement devices.");
    }

    double meas = 0;
    epicsTimeStamp timestamp;
    // Make sure the measurement has expected PulseId
    if (_m1->isFcom() && !_m1->isFile()) {
      if (_m1->peekPulseId() != (epicsUInt32) _loopConfiguration->_pulseIdPv.getValue()) {
	return -1;
	// Don't need to throw exception, just don't use this pulse
	//	throw Exception("Sensor pulse id mismatch, no change");
      }
    }

    if (_m1->get(meas, timestamp) != 0) {
      throw Exception("Cannot read measurements, no change");
      return -1;
    }


    if (meas > ExecConfiguration::getInstance()._tmitLowPv.getValue()) {
      _goodMeasurementsCount++;
      _lowTmitMeasurementsCount = 0;
      _m1Average.add(meas);
    }
    else {
      _loopConfiguration->_logger << Log::showtime << "TMIT too low on " 
				  << _m1->getDeviceName().c_str() << ", no change"
				  << Log::flushpvonly;
    }

    // If a good measurement was received, clear the counter of mismatched
    // pulseids
    _mismatchPulseIdMeasurements = 0;

    if (_goodMeasurementsCount == 1) {
      _loopConfiguration->_logger << Log::showtime << "Starting average"
				  << Log::flushpvonlynoalarm;
    }

    return _goodMeasurementsCount;
}

/**
 * @param configuration contains all information about the loop configuration
 * @param measurements set of measurements to be used by this calculation
 * @param actuators set of actuators to be calculated
 * @param states ses of states to be calculated
 * @return always 0
 * @author L.Piccoli
 */
int BunchCharge::calculate(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) throw (Exception) {
  if (!doneMoving()) {
    _loopConfiguration->_logger << Log::showtime << "Starting average again (waveplates moved)"
				<< Log::flushpvonlynoalarm;
    _goodMeasurementsCount = 0;
    _lowTmitMeasurementsCount = 0;
    return 0;
  }

  // If there are enough good measurements, try changing actuator
  int goodMeasurements = getMeasurements();
  if (goodMeasurements >= (int) MAX_GOOD_MEASUREMENTS) {
    // Waveplates are done moving, a new actuator value can be sent!
    if (doneMoving()) {
      double newAct = calculateActuator();
      _goodMeasurementsCount = 0;
      updateActuator(newAct);
      _actuatorSkipCount = 0;
      _loopConfiguration->_logger << Log::showtime << "Feedback acting"
				  << Log::flushpvonlynoalarm;
    }
    else {
      _actuatorSkipCount++;
      _m1Average.clear();

      _loopConfiguration->_logger << Log::showtime << "Starting average again (waveplates moved)"
				  << Log::flushpvonlynoalarm;

      _goodMeasurementsCount = 0;
      if (_actuatorSkipCount >= MAX_ACTUATOR_SKIP) {
	double laserDoneMoving = 0;
	double cameraDoneMoving = 0;
	epicsTimeStamp timestamp;
	
	_laserWaveplateDoneStatusChannel->read(laserDoneMoving, timestamp);
	_cameraWaveplateDoneStatusChannel->read(cameraDoneMoving, timestamp);

	if (laserDoneMoving == 0 && cameraDoneMoving == 1) {
	  throw Exception("Laser waveplate moving, feedback not actuating.");
	}

	if (laserDoneMoving == 1 && cameraDoneMoving == 0) {
	  throw Exception("Camera waveplate moving, feedback not actuating.");
	}

	if (laserDoneMoving == 0 && cameraDoneMoving == 0) {
	  throw Exception("Laser and camera waveplate moving, feedback not actuating");
	}
      }
    }
  }
  else if (goodMeasurements == -1) {
    _mismatchPulseIdMeasurements++;
    if (_mismatchPulseIdMeasurements >= MAX_MISMATCHED_PULSEID_MEASUREMENTS) {
      _mismatchPulseIdMeasurements = 0;
      throw Exception("Not actuating, too many mismatched pulseids. Please Stop/Start feedback.");
    }
  }

  return 0;
}

/**
 * This method checks the status of the laser and camera waveplates. If waveplates
 * are not moving then the feedback can be applied.
 * 
 * @return true if feedback can be applied
 * throws Exception if one of the conditions is not met
 * @author L.Piccoli
 */
bool BunchCharge::checkWaveplate() throw (Exception) {
  double laserMoving = 1;
  double laserDoneMoving = 0;
  double cameraMoving = 1;
  double cameraDoneMoving = 0;
  epicsTimeStamp timestamp;

  /*
  // Is the laser moving?
  _laserWaveplateStatusChannel->read(laserMoving, timestamp);
  if (laserMoving != 0) {
    throw Exception("Laser waveplate is still moving");
  }

  // Is the camera moving?
  _cameraWaveplateStatusChannel->read(cameraMoving, timestamp);
  if (cameraMoving != 0) {
    throw Exception("Camera waveplate is still moving");
  }
  
  // Is the laser done?
  _laserWaveplateDoneStatusChannel->read(laserDoneMoving, timestamp);
  if (laserDoneMoving != 1) {
    throw Exception("Last laser move was not successful");
  }

  // Is the camera done?
  _cameraWaveplateDoneStatusChannel->read(cameraDoneMoving, timestamp);
  if (cameraDoneMoving != 1) {
    throw Exception("Last camera move was not successful");
  }
  */
  // Error if not moving and not done moving are set
  _laserWaveplateStatusChannel->read(laserMoving, timestamp);
  _laserWaveplateDoneStatusChannel->read(laserDoneMoving, timestamp);
  if (laserMoving == 0 && laserDoneMoving == 0) {
    throw Exception("Cannot move laser waveplate");
  }

  _cameraWaveplateStatusChannel->read(cameraMoving, timestamp);
  _cameraWaveplateDoneStatusChannel->read(cameraDoneMoving, timestamp);
  if (cameraMoving == 0 && cameraDoneMoving == 0) {
    throw Exception("Cannot move camera VCC waveplate");
  }

  return true;
}

/**
 * Calculate new actuator value based on its previous value and the current
 * measurement average.
 *
 * @return new actuator value
 * @author L.Piccoli
 */
double BunchCharge::calculateActuator() {
  double measurement = _m1Average.getAverage();

  measurement *= 1.602e-10;

  updateState(measurement);

  double error = measurement - _s1->getSetpoint();
  double correction = error * _loopConfiguration->_pGainPv.getValue();

  if (correction > 1) {
    correction *= _s1->getSetpoint();
  }
  /*
  if (std::abs(1 - correction) < ACTUATOR_DEADBAND) {
    correction = 1;
  }
  */
  return _a1->getLastValueSet() * (1 - correction);
}

void BunchCharge::updateActuator(double newValue) {
    std::string exceptionMessage = "Actuator out of range";

    // Fill in the current time and pulseId
    epicsTimeStamp timestamp;
    epicsTimeGetCurrent(&timestamp);
#ifdef RTEMS
    evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());
#endif

    // Change the actuator value only if MODE PV is ENABLE (true), otherwise set to
    // same previous value
    if (_loopConfiguration->_mode == false) {
      newValue = _a1->getLastValueSet();
    }
    if (_a1->set(newValue, timestamp) != 0) {
      throw Exception(exceptionMessage, Exception::ACTUATOR_LIMIT);
    }
}

void BunchCharge::updateState(double newValue) {
    std::string exceptionMessage = "State out of range";

    // Fill in the current time and pulseId
    epicsTimeStamp timestamp;
    epicsTimeGetCurrent(&timestamp);
#ifdef RTEMS
    evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());
#endif

    if (_s1->set(newValue, timestamp) != 0) {
      throw Exception(exceptionMessage, Exception::STATE_LIMIT);
    }
}

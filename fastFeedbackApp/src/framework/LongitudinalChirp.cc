/*
 * File:   Longitudinal.cc
 * Author: lpiccoli
 * 
 * Created on November 18, 2010, 13:30
 */

#include "LongitudinalChirp.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <cmath>
#include <vector>
#include "epicsTime.h"
#include "ExecConfiguration.h"
#include "MeasurementAverage.h"
#include "PatternManager.h"

USING_FF_NAMESPACE

using namespace boost::numeric::ublas;

const double PI = 3.14159265;

/**
 * Class constructor. Only initializes attributes.
 *
 * @author L.Piccoli
 */
LongitudinalChirp::LongitudinalChirp(std::string algorithmName) :
  Longitudinal(algorithmName),
  _previousBc2Energy(0),
  _previousBc2Chirp(0),
  _previousBc2FbckPv(false),
  _chirpControl(false),
  _bc2EnergyState(NULL),
  _bc2BunchState(NULL),
  _sequencerDelayCounter(0),
  _chirpToggle(false) {
  _previousActuator.resize(LONGITUDINAL_MATRIX_DIMENSION);
  _actuatorValue.resize(LONGITUDINAL_MATRIX_DIMENSION);
  for (unsigned int i = 0; i < _previousActuator.size(); ++i) {
    _previousActuator[i] = 0;
    _actuatorValue[i] = 0;
  }
}

LongitudinalChirp::~LongitudinalChirp() {
}

/**
 * Initialize algorithm parameters based on data stored in the LoopConfiguration.
 *
 * @param configuration pointer to the LoopConfiguration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LongitudinalChirp::configure(LoopConfiguration *configuration,
				 MeasurementSet *measurements,
				 ActuatorSet *actuators, StateSet *states)
  throw (Exception) {
  _firstCalculate = true;

  if (Longitudinal::configure(configuration, measurements, actuators, states)
      != 0) {
    return -1;
  }

  _chirpControl = configuration->_chirpControlPv.getValue();

  StateSet::iterator sIt = states->begin();
  std::advance(sIt, 3);
  _bc2EnergyState = (*sIt);
  std::advance(sIt, 1);
  _bc2BunchState = (*sIt);

  ActuatorSet::iterator aIt = actuators->begin();
  std::advance(aIt, 3);
  _bc2Amplitude = (*aIt);
  std::advance(aIt, 1);
  _bc2Phase = (*aIt);

  return 0;
}

/*
 * Must override this method in order to have the bc2State USEDBYLOOP Pv
 * set if the chirp is ON!
 */
int LongitudinalChirp::selectStates() throw (Exception) {
  bool rethrow = false;
  Exception except("");
  int val = 0;
  try {
    val = Longitudinal::selectStates();
  } catch (Exception &e) {
    rethrow = true;
    except = e;
  }

  if (_chirpControl) {
    _bc2BunchState->setUsedBy(true); // This UNSET by Longitudinal::calculate()!
  }

  // FBCK PV of BC2 Amplitude and Phase must be set to ON if:
  // _chirpControl is true or
  // _bc2EnergyState->isUsed() is true or
  // _bc2BunchState->isUsed() is true
  bool fbckPv = true;

  // FBCK PV for BC2 Amplitude and Phase must be set to OFF iff:
  // _bc2BunchState->isUsed() is false AND
  // _bc2EnergyState->isUsed() is false
  if (!_bc2EnergyState->isUsed() && !_bc2BunchState->isUsed()) {
    fbckPv = false;
  }

  bool fbckToggle = false;
  if (fbckPv != _actuatorFbckState[4]) {
    fbckToggle = true;
    _actuatorFbckState[4] = fbckPv;
  }

  if (_chirpToggle || fbckToggle) {
    if (_loopConfiguration->_mode == true) {
      Log::getInstance() << Log::flagFbckPv << Log::showtime << Log::dpInfo
			 << "LongitudinalChirp::selectStates() FBCK PV toggle for BC2"
			 << Log::dp;
      _bc2Phase->setFeedbackPv(fbckPv);
    }
  }
  // At this point we also must reset the _previousActuator[] elements
  // for those states that have been just turned on!
  resetActuators();

  if (rethrow) {
    throw except;
  }
  return val;
}

/**
 * Implementation of the Longitudinal algorithm.
 *
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LongitudinalChirp::calculate() throw (Exception) {
  checkChirpControl();

  // Calculate initial Energy/Chirp from the initial actuator
  // values.
  // The value of _firstCalculate is set to true in the
  // Longitudital::configure() method.
  if (_firstCalculate) {
    Log::getInstance() << Log::flagAlgo << Log::dpInfo
		       << "LongitudinalChirp::calculate() - First Cycle" << Log::dp;
    calculateInitialStates();
  }

  // Proceed with feedback iff CHIRPSTATE (_chirpControl) is ON
  // and CHIRPREADY is YES  *or* CHIRPSTATE is OFF and CHIRPREADY is NO.
  // CHIRPREADY is set by the sequencer based on the CHIRPSTATE PV.
  if ((_loopConfiguration->_chirpReadyPv.getValue() == true
       && _chirpControl == true)
      || (_loopConfiguration->_chirpReadyPv.getValue() == false
	  && _chirpControl == false)) {
    return Longitudinal::calculate();
  } else {
    if (_chirpControl) {
      if (_loopConfiguration->_chirpReadyPv.getValue() == false) {
	_sequencerDelayCounter++;
	if (_sequencerDelayCounter > 10) {
	  _loopConfiguration->_logger << Log::showtime
				      << "WARNING: There may be a sequencer problem - CHIRPREADY has not been set yet!"
				      << Log::flushpv;
	}
      }
    }
  }

  return 0;
}

/**
 * Make sure the BLEN measurement is valid if chirp is
 * enabled.
 *
 * @return 0 on success, -1 if Longitudinal::checkMeasurementStatus()
 * fails.
 * @author L.Piccoli
 */
int LongitudinalChirp::checkMeasurementStatus() {
  if (Longitudinal::checkMeasurementStatus() != 0) {
    return -1;
  }

  if (_chirpControl) {
    _measurementStatus[M6] = VALID;
  }

  return 0;
}

/**
 * Set the CHIRPSTATE PV.
 *
 * @return 0
 * @author L.Piccoli
 */
int LongitudinalChirp::checkChirpControl() {
  bool chirp = false;

  if (_bc2EnergyState != NULL && _bc2BunchState != NULL) {
    if (_bc2EnergyState->isUsed() && !_bc2BunchState->isUsed()) {
      chirp = true;
    }
  }

  _loopConfiguration->_chirpStatePv = chirp;
  _loopConfiguration->_chirpStatePv.scanIoRequest();

  // Set STATUSSTR message if chirpControl changed value
  if (chirp != _chirpControl) {
    _sequencerDelayCounter = 0;
    _loopConfiguration->_logger << Log::showtime << "Chirp is currently ";
    if (chirp) {
      _loopConfiguration->_logger << "ON";
    } else {
      _loopConfiguration->_logger << "OFF";
    }
    _loopConfiguration->_logger << Log::flushpvnoalarm;

    // Get initial actuator values after chirp control changes
    getInitialValues();
  }

  // Set the CHIRPCTRL PV according to S4USED and S5USED PVs
  // It is ON iff S4USED=1 and S5USED=0
  _chirpControl = chirp;

  return 0;
}

void LongitudinalChirp::getInitialValues() {
  // Get initial actuator value if state was just turned on
  // and feedback is in the ENABLE mode.
  if (_bc2EnergyState->getUsedBy() == false && _bc2EnergyState->isUsed()
      && _loopConfiguration->_mode == true) {
    Log::getInstance() << Log::flagFbckPv << Log::dpInfo
		       << "LongitudinalChirp::selectStates() " << "reading initial values for "
		       << _bc2Amplitude->getDeviceName().c_str() << Log::dp;
    _loopConfiguration->_logger << Log::showtime << "Reading "
				<< _bc2Amplitude->getDeviceName().c_str() << Log::flushpvonlynoalarm;
    int settingResult = _bc2Amplitude->getInitialSetting();
    double initial = _bc2Amplitude->getLastValueSet();
    if (settingResult < 0) {
      throw Exception("Failed to get initial value for A4 (BC2 energy)");
    } else {
      _loopConfiguration->_logger << Log::showtime
				  << "BC2 energy setpoint read. "
				  << _bc2Amplitude->getDeviceName().c_str() << " = " << initial
				  << " (prev=" << _previousBc2Energy << ")" << Log::flushpvonlynoalarm;
      _loopConfiguration->_logger << Log::showtime
				  << "* BC2 energy setpoint read. "
				  << _bc2Amplitude->getDeviceName().c_str() << " = " << initial
				  << " (prev=" << _previousBc2Energy << ")" << Log::cout;
    }
  }

  // Get initial actuator value if state was just turned on
  // and feedback is in the ENABLE mode.
  if (/*_bc2BunchState->getUsedBy() == false &&*/
      _bc2BunchState->isUsed() && _loopConfiguration->_mode == true) {
    Log::getInstance() << Log::flagFbckPv << Log::dpInfo
		       << "LongitudinalChirp::selectStates() " << "reading initial values for "
		       << _bc2Phase->getDeviceName().c_str() << Log::dp;
    _loopConfiguration->_logger << Log::showtime << "Reading "
				<< _bc2Phase->getDeviceName().c_str() << Log::flushpvonlynoalarm;
    int settingResult = _bc2Phase->getInitialSetting();
    double initial = _bc2Phase->getLastValueSet();
    if (settingResult < 0) {
      throw Exception("Failed to get initial value for A5 (BC2 current)");
    } else {
      _loopConfiguration->_logger << Log::showtime
				  << "BC2 current setpoint read. " << _bc2Phase->getDeviceName().c_str()
				  << " = " << initial << Log::flushpvonlynoalarm;
      _loopConfiguration->_logger << Log::showtime
				  << "* BC2 current setpoint read. "
				  << _bc2Phase->getDeviceName().c_str() << " = " << initial
				  << Log::cout;
    }
  }
}

/**
 * After the states have been selected based on where beam is we need
 * to reset any previous actuator values in case a given state
 * was just turned back on.
 */
void LongitudinalChirp::resetActuators() {
  ActuatorSet::iterator it = _actuators->begin();

  // Fill in the current time and pulseId
  epicsTimeStamp timestamp;
  epicsTimeGetCurrent(&timestamp);
  evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());

  double bc2Amplitude = 0;
  double bc2Phase = 0;

  for (int i = 0; i < MAX_LONGITUDINAL_STATES; ++i) {
    (*it)->getInitialSettingFromReference(false, timestamp);
    double lastValueSet = (*it)->getLastValueSet();

    if (i == STATE_BC2_ENERGY) {
      bc2Amplitude = lastValueSet;
      // BC_ENERGY comes before BC_CURRENT, therefore the
      // bc2Phase will be zero as initialized outside the
      // loop. The last bc2Phase value must be retrieved here.
      ActuatorSet::iterator bc2It = it;
      ++bc2It;
      (*bc2It)->getInitialSettingFromReference(false, timestamp);
      bc2Phase = (*bc2It)->getLastValueSet();
    }
    if (i == STATE_BC2_CURRENT) {
      bc2Phase = lastValueSet;
    }

    if (_stateStatus[i] == STATE_JUST_ON) {
      if (i == STATE_BC2_ENERGY) {
	lastValueSet = bc2Amplitude * std::cos(bc2Phase * (PI / 180));
      }
      if (i == STATE_BC2_CURRENT) {
	lastValueSet = bc2Amplitude * std::sin(bc2Phase * (PI / 180));
      }

      Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			 << "LongitudinalChirp::resetActuators() "
			 << " (i=" << i << ") bc2Amplitude=" << bc2Amplitude
			 << "; bc2Phase=" << bc2Phase << Log::dp;

      Log::getInstance() << Log::flagFbckPv << Log::dpInfo
			 << "LongitudinalChirp::resetActuators() "
			 << "setting previous actuator values from " << _previousActuator[i]
			 << " to " << lastValueSet << " (i=" << i << ")" << Log::dp;
      _previousActuator[i] = lastValueSet;
    }
    ++it;
  }
}

/**
 * Calculate initial Energy/Chirp for values from initias Phase/Amplitude
 * values read from the actuator devices for BC2.
 *
 * DL1, BC1 and DL2 use Phase/Amplitude.
 *
 * @return always 0
 * @author L.Piccoli
 */
int LongitudinalChirp::calculateInitialStates() {
  ActuatorSet::iterator it = _actuators->begin();

  // Fill in the current time and pulseId
  epicsTimeStamp timestamp;
  epicsTimeGetCurrent(&timestamp);
  evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());

  // DL - Must make sure the initial actiator setting is valid, so read
  // the latest from its reference. That is done for all actuators.
  (*it)->getInitialSettingFromReference(false, timestamp);
  _previousActuator[DL1] = (*it)->getLastValueSet();

  // BC1 Amplitude
  ++it;
  (*it)->getInitialSettingFromReference(false, timestamp);
  double bc1Amplitude = (*it)->getLastValueSet();
  _previousActuator[BC1_ENERGY] = bc1Amplitude;

  // BC1 Phase
  ++it;
  (*it)->getInitialSettingFromReference(false, timestamp);
  double bc1Phase = (*it)->getLastValueSet();
  _previousActuator[BC1_CHIRP] = bc1Phase;

  // BC2 Energy
  ++it;
  (*it)->getInitialSettingFromReference(false, timestamp);
  double bc2Amplitude = (*it)->getLastValueSet();
  ++it;
  (*it)->getInitialSettingFromReference(false, timestamp);
  double bc2Phase = (*it)->getLastValueSet();
  _previousActuator[BC2_ENERGY] = bc2Amplitude
    * std::cos(bc2Phase * (PI / 180));

  // BC2 Chirp - _previousActuator[BC2_CHIRP] must be
  // equal to _loopConfiguration->_bc2ChirpPv.getValue();
  _previousActuator[BC2_CHIRP] = bc2Amplitude * std::sin(bc2Phase * (PI / 180));

  // DL2
  ++it;
  (*it)->getInitialSettingFromReference(false, timestamp);
  _previousActuator[DL2] = (*it)->getLastValueSet();

  Log::getInstance() << Log::flagAlgo << Log::dpInfo
		     << "LongitudinalChirp::calcInitialStates() [" << _previousActuator[DL1]
		     << ", " << _previousActuator[BC1_ENERGY] << ", "
		     << _previousActuator[BC1_CHIRP] << ", " << _previousActuator[BC2_ENERGY]
		     << " (" << bc2Amplitude << "), " << _previousActuator[BC2_CHIRP] << " ("
		     << bc2Phase << "), " << _previousActuator[DL2] << "]" << Log::dp;

  _previousBc2FbckPv = true;
  if (!_bc2EnergyState->isUsed() && !_bc2BunchState->isUsed()) {
    _previousBc2FbckPv = false;
  }
  if (_chirpControl) {
    _previousBc2FbckPv = true;
  }

  return 0;
}

/**
 * Update the values for the actuators using latests Energy/Chirps
 *
 * Changes in actuator values are added to the last values successfully written
 * to the ActuatorDevices.
 *
 * @return 0 if all actuator values are within limits, -1 if at least
 * one of the values is outside the limits
 * @author L.Piccoli
 */
int LongitudinalChirp::updateActuators() throw (Exception) {
  unsigned int limitFlag = 0;
  double values[10];

  // Calculate values stored in the vector _delta.
  updateActuatorsDelta();

  // _delta contains Energy/Chirp values, these must be converted
  // to Amplitude/Phase before being sent to actuators
  calculateAmplitudeAndPhase();

  // Fill in the current time and pulseId
  epicsTimeStamp timestamp;
  epicsTimeGetCurrent(&timestamp);
  evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());

  // Update actuators
  bool actuatorLimit = false;
  StateSet::iterator stateIt = _states->begin();
  int i = 0;
  for (ActuatorSet::iterator it = _actuators->begin(); it != _actuators->end();
       ++it, ++i, ++stateIt) {
    ActuatorDevice *actuator = (*it);
    double value = 0;
    if ((*stateIt)->getUsedBy()) {
      // Change the actuator value only if MODE PV is ENABLE (true)
      if (_loopConfiguration->_mode == true) {
	value = _actuatorValue[i] + (*stateIt)->getOffset();
	values[i] = value;
	if (actuator->set(value, timestamp) != 0) {
	  limitFlag |= (0x01 << i);
	  actuatorLimit = true;
	}
      }
    } else {
      value = actuator->getLastValueSet();
      actuator->set(value, DataPoint::SET_SKIP);
    }
  }

  if (actuatorLimit) {
    i = 0;
    _exceptionMessage = "Actuator(s) out of range:";
    for (ActuatorSet::iterator it = _actuators->begin();
	 it != _actuators->end(); ++it, ++i) {
      if (limitFlag & (0x01 << i)) {
	std::ostringstream strstream;
	strstream << values[i];
	_exceptionMessage += ", ";
	_exceptionMessage += (*it)->getDeviceName();
	_exceptionMessage += " (";
	_exceptionMessage += strstream.str();
	_exceptionMessage += ")";
      }
    }
    throw Exception(_exceptionMessage, Exception::ACTUATOR_LIMIT);
    return -1;
  }

  // Save new actuator values (phase/amplitude) to be used on the next iteration
  _previousActuator = _actuatorValue;

  // Restore the Energy/Chirp for BC2 - we don't want to save the phase/amplitude
  // for BC2, therefore the Energy/Chirp are restored here.
  _previousActuator[BC2_ENERGY] = _previousBc2Energy;
  _previousActuator[BC2_CHIRP] = _previousBc2Chirp;

  Log::getInstance() << Log::flagAlgo << Log::dpInfo
		     << "LongitudinalChirp::updateActuators(P" << _patternIndex << ") ["
		     << _actuatorValue[0] << ", " << _actuatorValue[1] << ", "
		     << _actuatorValue[2] << ", " << _actuatorValue[3] << " [e="
		     << _previousBc2Energy << "], " << _actuatorValue[4] << " [c="
		     << _previousBc2Chirp << "], " << _actuatorValue[5] << ", " << Log::dp;
  return 0;
}

int LongitudinalChirp::calculateAmplitudeAndPhase() {
  double chirp = _loopConfiguration->_bc2ChirpPv.getValue();
  if (_chirpControl) {
    if (!isChirpValid(chirp)) {
      return -1;
    }
  }

  // OVER/UNDER COMPRESSED - change signal here depending on the setpoint signal
  // 09/30/2011 - Isn't this done already in the Longitudinal.cc?

  // Amplitude at DL1
  _actuatorValue[DL1] = _previousActuator[DL1] - _delta[DL1];

  // Amplitude at BC1
  _actuatorValue[BC1_AMPLITUDE] = _previousActuator[BC1_ENERGY]
    - _delta[BC1_ENERGY];

  // Phase at BC1
  _actuatorValue[BC1_PHASE] = _previousActuator[BC1_CHIRP] + _delta[BC1_CHIRP];

  // Amplitude at BC2 = sqrt(E_bc2^2 + C_bc2^2)
  /*
    std::cout << ">>> DELTAS: " << std::endl;
    std::cout << " _previousActuator[BC2_ENERGY]= " << _previousActuator[BC2_ENERGY]  << std::endl;
    std::cout << " _previousActuator[BC2_CHIRP]= " << _previousActuator[BC2_CHIRP]  << std::endl;
    std::cout << " _delta[BC2_ENERGY]= " << _delta[BC2_ENERGY]  << std::endl;
    std::cout << " _delta[BC2_CHIRP]= " << _delta[BC2_CHIRP]  << std::endl;
  */

  //  std::cout << " chirp= " << chirp  << std::endl;
  _previousBc2Energy = _previousActuator[BC2_ENERGY] - _delta[BC2_ENERGY];
  if (_chirpControl) {
    _actuatorValue[BC2_AMPLITUDE] = std::sqrt(
					      ((_previousActuator[BC2_ENERGY] - _delta[BC2_ENERGY])
					       * (_previousActuator[BC2_ENERGY] - _delta[BC2_ENERGY]))
					      + (chirp * chirp));
    /*
      std::cout << " _actuatorValue[BC2_AMPLITUDE]= " << _actuatorValue[BC2_AMPLITUDE]  << std::endl;
    */
  } else {
    _actuatorValue[BC2_AMPLITUDE] = std::sqrt(
					      ((_previousActuator[BC2_ENERGY] - _delta[BC2_ENERGY])
					       * (_previousActuator[BC2_ENERGY] - _delta[BC2_ENERGY]))
					      + ((_previousActuator[BC2_CHIRP] + _delta[BC2_CHIRP])
						 * (_previousActuator[BC2_CHIRP] + _delta[BC2_CHIRP])));
    //    std::cout << " _actuatorValue[BC2_AMPLITUDE]= " << _actuatorValue[BC2_AMPLITUDE]  << std::endl;
  }

  // Phase at BC2 = 180/pi * (atan2 (C_bc2, E_bc2))
  if (_chirpControl) {
    _previousBc2Chirp = chirp;
    _actuatorValue[BC2_PHASE] =
      180 / PI
      * (std::atan2(chirp,
		    _previousActuator[BC2_ENERGY] - _delta[BC2_ENERGY]));
    //    std::cout << " _actuatorValue[BC2_PHASE]= " << _actuatorValue[BC2_PHASE]  << std::endl;
  } else {
    _previousBc2Chirp = _previousActuator[BC2_CHIRP] + _delta[BC2_CHIRP];
    _actuatorValue[BC2_PHASE] = 180 / PI
      * (std::atan2(_previousActuator[BC2_CHIRP] + _delta[BC2_CHIRP],
		    _previousActuator[BC2_ENERGY] - _delta[BC2_ENERGY]));
    //    std::cout << " _actuatorValue[BC2_PHASE]= " << _actuatorValue[BC2_PHASE]  << std::endl;
  }

  // Amplitude at DL2
  _actuatorValue[DL2] = _previousActuator[DL2] - _delta[DL2];

  return 0;
}

/**
 * Check if the user input chirp is within the S5 (BC2 Current) state limits.
 */
bool LongitudinalChirp::isChirpValid(double chirp) {
  if (_bc2BunchState == NULL) {
    _exceptionMessage = "BC2 Bunch State not valid, something wrong!";
    throw Exception(_exceptionMessage, Exception::STATE_LIMIT);
    return false;
  }

  if (chirp < _bc2BunchState->getLolo()) {
    std::ostringstream strstream;
    strstream << "Chirp out of range: " << chirp
	      << ", smaller than BC2 State LOLO limit (" << _bc2BunchState->getLolo()
	      << ")";
    throw Exception(strstream.str(), Exception::STATE_LIMIT);

    return false;
  }

  if (chirp > _bc2BunchState->getHihi()) {
    std::ostringstream strstream;
    strstream << "Chirp out of range: " << chirp
	      << ", greater than BC2 State HIHI limit (" << _bc2BunchState->getHihi()
	      << ")";
    throw Exception(strstream.str(), Exception::STATE_LIMIT);
    return false;
  }

  return true;
}

int LongitudinalChirp::reset() {
  Longitudinal::reset();

  return 0;
}

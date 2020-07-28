/* 
 * File:   Algorithm.cc
 * Author: lpiccoli
 * 
 * Created on June 7, 2010, 3:46 PM
 */

#include "Algorithm.h"

USING_FF_NAMESPACE

Algorithm::Algorithm(std::string name) :
  _name(name),
  _exceptionMessage(256, ' '),
  _patternIndex(0) {
}

Algorithm::~Algorithm() {
}

int Algorithm::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) {
    _loopConfiguration = configuration;
    _measurements = measurements;
    _actuators = actuators;
    _states = states;

    return 0;
}

/**
 * Set the FBCK PV to ON or OFF for all selected Actuators and Measurements,
 * according to the specified parameters.
 *
 * The FBCK PV is set to ON or OFF only if the USEDBYLOOP PV is true. In most
 * feedbacks the valued of USEDBYLOOP is true by default. Only in special 
 * cases, such as the Longitudinal feedback, the USEDBYLOOP may be false.
 *
 * @param measurements true indicates measurement FBCK PVs should be turned ON,
 * if false FBCK PV is turned OFF
 * @param actuators true indicates actuator FBCK PVs should be turned ON,
 * if false FBCK PV is turned OFF
 * @return 0 if all PVs were set, -1 if there was an error setting one PV.
 * @author L.Piccoli
 */
int Algorithm::setFeedbackDevices(bool measurements, bool actuators) {
    ActuatorSet::iterator actIt;
    for (actIt = _actuators->begin(); actIt != _actuators->end(); ++actIt) {
        ActuatorDevice *actuator = *actIt;
	if (actuator != NULL) {
	  if (actuator->setFeedbackPv(actuators && actuator->getUsedBy()) != 0) {
	    Log::getInstance() << "Cannot set "
			       << actuator->getDeviceName().c_str() << " FBCK PV"
			       << Log::flush;
	    return -1;
	  }
	}
    }

    MeasurementSet::iterator measIt;
    for (measIt = _measurements->begin(); measIt != _measurements->end(); ++measIt) {
        MeasurementDevice *measurement = *measIt;
	if (measurement != NULL) {
	  if (measurement->setFeedbackPv(measurements && measurement->getUsedBy()) != 0) {
            Log::getInstance() << "Cannot set "
			       << measurement->getDeviceName().c_str() << " FBCK PV"
			       << Log::flush;
            return -1;
	  }
	}
    }

    return 0;
}

/**
 * This method is invoked when patterns with POCKCEL_PERM set start to 
 * be received. It was initially implemented by the Loop class, but was moved
 * in here because of the Longitudinal feedback needs. There is an
 * additional check, which can only be done in the Longitudinal class.
 * 
 * Actuators for this pattern have values set to the values calculated for
 * the P1 pattern plus the offset between patterns.
 * 
 * @author L.Piccoli
 */
void Algorithm::setActuatorsReference() {
  // Fill in the current time and pulseId
  epicsTimeStamp timestamp;
  epicsTimeGetCurrent(&timestamp);
  evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());

    ActuatorSet::iterator it;
    bool setActuatorWithNoBeam = true;
    for (it = _actuators->begin(); it != _actuators->end(); ++it) {
        ActuatorDevice *actuator = (*it);
        actuator->getInitialSettingFromReference(setActuatorWithNoBeam, timestamp);
    }
}

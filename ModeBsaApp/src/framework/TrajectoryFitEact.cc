/*
 * File:   TrajectoryFitEact.cc
 * Author: lpiccoli
 * 
 * Created on May 10, 2011, 14:06 AM
 */

#include "TrajectoryFitEact.h"
#include "ExecConfiguration.h"

USING_FF_NAMESPACE

/**
 * Class constructor. Only initializes attributes
 *
 * @author L.Piccoli
 */
TrajectoryFitEact::TrajectoryFitEact(std::string algorithmName) :
  TrajectoryFitStatic(algorithmName) {
}

TrajectoryFitEact::~TrajectoryFitEact() {
	disconnect();
}

/**
 * @param configuration pointer to the LoopConfiguration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int TrajectoryFitEact::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) {
  _loopConfiguration = configuration;
  _measurements = measurements;
  _actuators = actuators;
  _states = states;

  connect();

  return reallyConfig();
}

void TrajectoryFitEact::connect() {
  if (_actuators == NULL) {
    throw Exception("There are no Actuators defined");
  }

  for (ActuatorSet::iterator it = _actuators->begin();
       it != _actuators->end(); ++it) {
    std::string deviceName = (*it)->getDeviceName();

    // Replace the last field with EACT
    size_t lastColon = deviceName.find_last_of(":");
    if (lastColon == std::string::npos) {
        std::stringstream s;
        s << "Failed to create EACT PV for " << deviceName;
        disconnect();
        throw Exception(s.str());
    }

    std::string eactName = deviceName.substr(0, lastColon);
    eactName += ":EACT";

    CaChannel *eactChannel = new CaChannel(CommunicationChannel::READ_ONLY, eactName);
    actuatorEnergy.push_back(eactChannel);
  }
}

void TrajectoryFitEact::disconnect() {
  for (std::vector<CaChannel *>::iterator it = actuatorEnergy.begin(); 
       it != actuatorEnergy.end(); ++it) {
    delete (*it);
  }
  actuatorEnergy.clear();
}

int TrajectoryFitEact::reset() {
  // The mutex is used here make the actuator energy won't be used while
  // it is being updated
  //_loopConfiguration->_mutex->lock();

  // Read current EACT values and populate loop configuration
  int i = 0;
  for (std::vector<CaChannel *>::iterator it = actuatorEnergy.begin(); 
       it != actuatorEnergy.end(); ++it, ++i) {
    double value;
    epicsTimeStamp timestamp;
    (*it)->read(value, timestamp);
    _loopConfiguration->_actEnergy[i] = value;
  }
  
  //_loopConfiguration->_mutex->unlock();

  return 0;
}

/**
 * Addition of mutex around of the calculate method to avoid 
 * race conditions when the feedback mode changes from compute 
 * to enable.
 */
// MUTEX should not be necessary - the algorithm and configuration are both
// owned in the same thread - they will always process sequentially

int TrajectoryFitEact::calculate(LoopConfiguration &configuration,
				 MeasurementSet &measurements,
				 ActuatorSet &actuators,
				 StateSet &states) {
  //configuration._mutex->lock();
  try {
    return TrajectoryFitStatic::calculate(configuration, measurements,
					  actuators, states);
  } catch (Exception &e) {
    //configuration._mutex->unlock();
    throw e;
  }
  //configuration._mutex->unlock();
  return -1;
}

/**
 * Addition of mutex around of the calculate method to avoid 
 * race conditions when the feedback mode changes from compute 
 * to enable.
 */
int TrajectoryFitEact::calculate() {
  //_loopConfiguration->_mutex->lock();
  try {
    return TrajectoryFitStatic::calculate();
  } catch (Exception &e) {
    //_loopConfiguration->_mutex->unlock();
    throw e;
  }
  //_loopConfiguration->_mutex->unlock();
  return -1;
}


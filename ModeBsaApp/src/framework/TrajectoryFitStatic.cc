/*
 * File:   TrajectoryFitStatic.cc
 * Author: lpiccoli
 * 
 * Created on March 28, 2011, 11:09 AM
 */

#include "TrajectoryFitStatic.h"

USING_FF_NAMESPACE

/**
 * Class constructor. Only initializes attributes
 *
 * @author L.Piccoli
 */
TrajectoryFitStatic::TrajectoryFitStatic(std::string algorithmName) :
  TrajectoryFitGeneral(algorithmName) {
}

TrajectoryFitStatic::~TrajectoryFitStatic() {
}

/**
 * Initialize algorithm parameters based on data stored in the LoopConfiguration
 * (e.g. F Matrix).
 *
 * The transportR1X matrix is composed by the first rows of the
 * F matrix. The number of rows is defined by the number of BPMs
 * (i.e.) measurements used in the loop. The number of BPMs can
 * be considered as the number of MeasurementDevices / 2.
 *
 * @param configuration pointer to the LoopConfiguration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int TrajectoryFitStatic::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) {
  _loopConfiguration = configuration;
  _measurements = measurements;
  _actuators = actuators;
  _states = states;

  return reallyConfig();
}

int TrajectoryFitStatic::checkConfig() {
  return 0;
};

/*
 * File:   TrajectoryFitBy1.cc
 * Author: lpiccoli
 * 
 * Created on July 16, 2010, 11:09 AM
 */

#include "TrajectoryFitBy1.h"
#include "ExecConfiguration.h"

USING_FF_NAMESPACE

/**
 * Class constructor. Only initializes attributes
 *
 * @author L.Piccoli
 */
TrajectoryFitBy1::TrajectoryFitBy1(std::string algorithmName) :
  TrajectoryFitStatic(algorithmName) {
}

TrajectoryFitBy1::~TrajectoryFitBy1() {
}

/**
 * Convert calculated actuator values to BDES values, applying energies
 * from BY1DBES PV and some magic numbers...
 *
 * @param actuatorDelta calculated changes in the actuator values
 * @author L.Piccoli
 */
void TrajectoryFitBy1::convertActuatorBdes(Vector& actuatorDelta) {
    for (Vector::iterator it = actuatorDelta.begin();
            it != actuatorDelta.end(); ++it) {
        *it = *it * 33.3564 * ExecConfiguration::getInstance()._by1BdesPv.getValue();
    }
}


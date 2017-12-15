/* 
 * File:   SineWave.cc
 * Author: lpiccoli
 * 
 * Created on June 7, 2010, 3:59 PM
 */

#include <iostream>
#include "SineWave.h"

USING_FF_NAMESPACE

/**
 * Set the actuators and states to a sine wave. The LoopConfiguration
 * and MeasurementSet are not used.
 *
 * @param configuration contains all information about the loop configuration
 * @param measurements set of measurements to be used by this calculation
 * @param actuators set of actuators to be calculated
 * @param states ses of states to be calculated
 * @return always 0
 * @author L.Piccoli
 */
int SineWave::calculate(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) throw (Exception) {
    switch (_mode) {
        case COUNTER_MODE:
            return calculateCounter(configuration, measurements,
                    actuators, states);
            break;
        case ONE_MEAS_ONE_ACT:
            return calculateMode1(configuration, measurements,
                    actuators, states);
            break;
        case TWO_MEAS_ONE_ACT:
            return calculateMode2(configuration, measurements,
                    actuators, states);
            break;
        case TWO_MEAS_TWO_ACT:
            return calculateMode3(configuration, measurements,
                    actuators, states);
            break;
        case N_MEAS_N_ACT:
            return calculateMode4(configuration, measurements,
                    actuators, states);
            break;
        case NOOP:
            return 0;
        default:
            return -1;
    }
    return 0;
}

int SineWave::calculateMode1(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) {
    if (measurements.size() != 1) {
        return -1;
    }
    if (actuators.size() != 1) {
        return -1;
    }
    if (states.size() != 1) {
        return -1;
    }
    MeasurementSet::iterator measurementIt = measurements.begin();
    double meas;
    epicsTimeStamp timestamp;
    MeasurementDevice *measurement = (*measurementIt);
    measurement->get(meas, timestamp);
    double value = sin(meas);

    ActuatorSet::iterator actuatorIt = actuators.begin();
    ActuatorDevice *actuator = (*actuatorIt);
    actuator->set(value);

    StateSet::iterator stateIt = states.begin();
    StateDevice *state = (*stateIt);
    state->set(value);

    return 0;
}

int SineWave::calculateMode2(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) {
    if (measurements.size() != 2) {
        std::cerr << "Measurements: " << measurements.size() << ", expected 2"
                << std::endl;
        return -1;
    }
    if (actuators.size() != 1) {
        std::cerr << "Actuators: " << actuators.size() << ", expected 1"
                << std::endl;
        return -1;
    }
    if (states.size() != 1) {
        std::cerr << "States: " << states.size() << ", expected 1"
                << std::endl;
        return -1;
    }
    MeasurementSet::iterator measurementIt;
    double measSum = 0;
    for (measurementIt = measurements.begin();
            measurementIt != measurements.end(); ++measurementIt) {
        double meas;
        epicsTimeStamp timestamp;
        MeasurementDevice *measurement = (*measurementIt);
        measurement->get(meas, timestamp);
        measSum += meas;
    }
    double value = sin(measSum);

    ActuatorSet::iterator actuatorIt = actuators.begin();
    ActuatorDevice *actuator = (*actuatorIt);
    actuator->set(value);

    StateSet::iterator stateIt = states.begin();
    StateDevice *state = (*stateIt);
    state->set(value);

    return 0;
}

// A1,S1 = sin(M1);
// A2,S2 = sin(M2);

int SineWave::calculateMode3(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) {
    if (measurements.size() != 2) {
        std::cerr << "Measurements: " << measurements.size() << ", expected 2"
                << std::endl;
        return -1;
    }
    if (actuators.size() != 2) {
        std::cerr << "Actuators: " << actuators.size() << ", expected 1"
                << std::endl;
        return -1;
    }
    if (states.size() != 2) {
        std::cerr << "States: " << states.size() << ", expected 1"
                << std::endl;
        return -1;
    }

    // A1,S1 = sin(M1);
    MeasurementSet::iterator measurementIt = measurements.begin();
    double meas;
    epicsTimeStamp timestamp;
    MeasurementDevice *measurement = (*measurementIt);
    measurement->get(meas, timestamp);
    double value = sin(meas);

    ActuatorSet::iterator actuatorIt = actuators.begin();
    ActuatorDevice *actuator = (*actuatorIt);
    actuator->set(value);

    StateSet::iterator stateIt = states.begin();
    StateDevice *state = (*stateIt);
    state->set(value);

    // A2,S2 = sin(M2);
    ++measurementIt;
    measurement = (*measurementIt);
    measurement->get(meas, timestamp);
    value = sin(meas);

    ++actuatorIt;
    actuator = (*actuatorIt);
    actuator->set(value);

    ++stateIt;
    state = (*stateIt);
    state->set(value);

    return 0;
}

// Number of MEAS must be equal to ACTS
// States are assigned sin(measN)

int SineWave::calculateMode4(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) {
    if (measurements.size() != actuators.size()) {
        std::cerr << "Number of measurements and actuators is not the same" << std::endl;
        std::cerr << "-> Measurements: " << measurements.size() << std::endl;
        std::cerr << "-> Actuators:" << actuators.size() << std::endl;
        return -1;
    }

    // A_n = sin(M_n);
    MeasurementSet::iterator measurementIt = measurements.begin();
    ActuatorSet::iterator actuatorIt = actuators.begin();
    double value = 0;
    for (; measurementIt != measurements.end(); ++measurementIt, ++actuatorIt) {
        double meas;
        epicsTimeStamp timestamp;
        MeasurementDevice *measurement = (*measurementIt);
        measurement->get(meas, timestamp);
        value = sin(meas);

        // Fill in the current time and pulseId
        epicsTimeGetCurrent(&timestamp);
        evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());

        ActuatorDevice *actuator = (*actuatorIt);
        actuator->set(value, timestamp);
    }

    StateSet::iterator stateIt = states.begin();
    for (; stateIt != states.end(); ++stateIt) {
        StateDevice * state = (*stateIt);
        state->set(value);
    }

    return 0;
}

int SineWave::calculateCounter(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) {
    double value = sin(_counter);

    MeasurementSet::iterator measurementIterator;
    double measValue;
    epicsTimeStamp timestamp;
    for (measurementIterator = measurements.begin();
            measurementIterator != measurements.end(); ++measurementIterator) {
        (*measurementIterator)->get(measValue, timestamp);
    }

    // Fill in the current time and pulseId
    epicsTimeGetCurrent(&timestamp);
#ifdef RTEMS
    evrTimePutPulseID(&timestamp, configuration._pulseIdPv.getValue());
#endif

    ActuatorSet::iterator actuatorIterator;
    for (actuatorIterator = actuators.begin();
            actuatorIterator != actuators.end(); ++actuatorIterator) {
        (*actuatorIterator)->set(value, timestamp);
    }

    StateSet::iterator stateIterator;
    for (stateIterator = states.begin(); stateIterator != states.end(); ++stateIterator) {
        (*stateIterator)->set(value);
    }

    // Miss some cycles!
    /*
    if (_counter == 4000) {
      usleep(1000000); // 100 msec
    }
     */

    _counter++;
    return 0;
}

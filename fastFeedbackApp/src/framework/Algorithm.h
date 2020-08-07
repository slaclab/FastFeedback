/* 
 * File:   Algorithm.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 3:46 PM
 */

#ifndef _ALGORITHM_H
#define	_ALGORITHM_H

#include "Defs.h"
#include "LoopConfiguration.h"

FF_NAMESPACE_START

/**
 * List of return values for the Algorithm::calculate() method::
 *
 * - OK: devices have values within limits and actuators can be set
 * - ERROR: generic error
 * - MEASUREMENT_LIMIT: read measurements are outside limits
 * - ACTUATOR_LIMIT: calculated actuators are outside limits
 * - STATE_LIMIT: calculated states are outside limits
 * - ...
 *
 * TODO: Remove this list, and migrate to Exceptions
 */
enum CalculateStatus {
    ERROR = -1,
    OK = 0,
    MEASUREMENT_LIMIT = 10,
    ACTUATOR_LIMIT = 11,
    STATE_LIMIT = 12
};

const std::string NOOP_ALGORITHM = "Noop";
const std::string COUNTER_ALGORITHM = "Counter";
const std::string TRAJECTORY_FIT_ALGORITHM = "TrajectoryFit";
const std::string FIXED_TRAJECTORY_FIT_ALGORITHM = "FixedTrajectoryFit";
const std::string TRAJECTORY_FIT_BY1_ALGORITHM = "TrajectoryFitBy1";
const std::string TRAJECTORY_FIT_POLYNOMIAL = "TrajectoryFitPoly";
const std::string TRAJECTORY_FIT_POLYNOMIAL1 = "TrajectoryFitPoly1";
const std::string TRAJECTORY_FIT_PINV = "TrajectoryFitPinv";
const std::string TRAJECTORY_FIT_EACT = "TrajectoryFitEact";
const std::string INJECTOR_LAUNCH_ALGORITHM = "InjectorLaunch";
const std::string LONGITUDINAL_ALGORITHM = "Longitudinal";
const std::string LONGITUDINAL_CHIRP_ALGORITHM = "LongitudinalChirp";
const std::string BUNCH_CHARGE_ALGORITHM = "BunchCharge";
const std::string SINE_ALGORITHM = "Sine";
const std::string SINE_ALGORITHM_N = "SineN";

/**
 * Algorithm is an abstract class that defines the interface for invoking
 * feedback algorithms.
 *
 * The calculate() method must be written by subclasses implementing a specific
 * algorithm.
 *
 * @author L.Piccoli
 */
class Algorithm {
public:
    Algorithm(std::string name = "Algorithm");
    virtual ~Algorithm();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states);

    virtual int calculate(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet & states) = 0;

    virtual int calculate() {
        return -1;
    }

    virtual int reset() {
      return 0;
    }

    virtual void show() {
    }

    virtual void showDebug() {
    }

    virtual int setFeedbackDevices(bool measurements, bool actuators);

    void setMeasurements(MeasurementSet *measurements) {
        _measurements = measurements;
    }

    void setActuators(ActuatorSet *actuators) {
        _actuators = actuators;
    }

    void setStates(StateSet *states) {
        _states = states;
    }

    void setPatternIndex(int patternIndex) {
      _patternIndex = patternIndex;
    }

    std::string getName() {
        return _name;
    }
    
    virtual void setActuatorsReference();

protected:
    LoopConfiguration *_loopConfiguration;
    MeasurementSet *_measurements;
    ActuatorSet *_actuators;
    StateSet *_states;

    /** Brief description of the algorithm */
    std::string _name;

    /** Exception string message, used when a failure occurs */
    std::string _exceptionMessage;

    /** Index of the pattern used to process this algorithm */
    int _patternIndex;
};

FF_NAMESPACE_END

#endif	/* _ALGORITHM_H */


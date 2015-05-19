    /*
 * File:   TrajectoryFit.cc
 * Author: lpiccoli
 * 
 * Created on July 16, 2010, 11:09 AM
 */

#include "TrajectoryFit.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#include <boost/numeric/bindings/lapack/workspace.hpp>
#include <boost/numeric/bindings/lapack/gels.hpp>
#include <boost/numeric/bindings/traits/traits.hpp>
#include "epicsTime.h"

USING_FF_NAMESPACE

/**
 * Class constructor. Only initializes attributes
 *
 * @author L.Piccoli
 */
TrajectoryFit::TrajectoryFit(std::string algorithmName) :
Algorithm(algorithmName),
_orbitDifference(40),
_fitX(20),
_fitY(20),
_calcStates(40),
_predictedTrajectory(40),
_actuatorDelta(40),
_setpoint(40),
_workspace(200),
_getMeasStats(50, "Get Measurements"),
_fitPrepStats(50, "TrajectoryFit fit::pre"),
_fitGelsStats(50, "TrajectoryFit fit::gels"),
_fitPostStats(50, "TrajectoryFit fit::post"),
_calculateCorrectorsStats(50, "Calculate correctors"),
_updateDevicesStats(50, "Update Actuators/States"),
_correctorGelsStats(50, "CorrectorKicks gels") {
}

TrajectoryFit::~TrajectoryFit() {
}

using namespace boost::numeric::ublas;
using namespace boost::numeric::bindings::traits;
using namespace boost::numeric::bindings::lapack;

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
int TrajectoryFit::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) throw (Exception) {
    _loopConfiguration = configuration;
    _measurements = measurements;
    _actuators = actuators;
    _states = states;

    int numBpms = _measurements->size()/2;

    // Get upper half, the number of rows is the number of BPMs
    // while the number of columns is defined by the FMatrix configured
    // in LoopConfiguration::configureFMatrix()
    _transportR1X.resize(numBpms, _loopConfiguration->_fMatrix.size2());
    _transportR1X = project(_loopConfiguration->_fMatrix, range(0, numBpms),
            range(0, _loopConfiguration->_fMatrix.size2()));

    // Get lower half, the number of rows is the number of BPMs
    // while the number of columns is defined by the FMatrix configured
    // in LoopConfiguration::configureFMatrix()
    _transportR1X.resize(numBpms, _loopConfiguration->_fMatrix.size2());
    _transportR3Y = project(_loopConfiguration->_fMatrix,
            range(numBpms, _loopConfiguration->_fMatrix.size1()),
            range(0, _loopConfiguration->_fMatrix.size2()));

    _fitMatrix.resize(_measurements->size(), _transportR1X.size2());
    _fitMatrixGELS.resize(_measurements->size(), _transportR1X.size2());

    project(_fitMatrix, range(0, _transportR1X.size1()),
            range(0, _transportR1X.size2())) = _transportR1X;

    project(_fitMatrix, range(_transportR1X.size1(),
            _transportR1X.size1() + _transportR3Y.size1()),
            range(0, _transportR3Y.size2())) = _transportR3Y;

    // build Ss vector
    //Vector orbitDifference(_measurementY.size() * 2); // Ss
    _orbitDifference.resize(_measurements->size()); // Ss
    _calcStates.resize(_measurements->size()); // p

    _orbitReferenceX.resize(_measurements->size() / 2);
    _orbitReferenceY.resize(_measurements->size() / 2);

    for (int i = 0; i < (int) _orbitReferenceY.size(); ++i) {
        _orbitReferenceX[i] = _loopConfiguration->_refOrbit[i];
        _orbitReferenceY[i] = _loopConfiguration->_refOrbit[i + _loopConfiguration->_refOrbit.size() / 2];
    }

    _measurementX.resize(numBpms);
    _measurementY.resize(numBpms);

    _fitX.resize(numBpms);
    _fitY.resize(numBpms);
    _calcStates.resize(numBpms * 2);

    _gMatrixGELS = _loopConfiguration->_gMatrix;

    return 0;
}

/**
 * Invokes the calculate() with no parameters. All parameters passed
 * to this method have already been passed through configure().
 *
 * @author L.Piccoli
 */
int TrajectoryFit::calculate(LoopConfiguration &configuration,
        MeasurementSet &measurements,
        ActuatorSet &actuators,
        StateSet &states) throw (Exception) {
    return calculate();
};

/**
 * Implementation of the trajectory fit algorithm (see fbTransTimerFcn.m file)
 *
 * @return 0 on success, -1 on failure, STATE_LIMIT if at least one new state value
 * exceeds the state limits or ACTUATOR_LIMIT if at least one actuator value
 * exceeds the hihi/lolo actuator limits
 * @author L.Piccoli
 */
int TrajectoryFit::calculate() throw (Exception) {
    _getMeasStats.start();
    int numBpms = getMeasurements();

    if (numBpms < 0) {
        return -1;
    }

    _getMeasStats.end();
    if (fit(_fitX, _fitY, _calcStates) != 0) {
        return -1;
    }

    //_calcStates.resize(_states->size(), true);
    _calculateCorrectorsStats.start();

    // Gather current setpoint values
    _setpoint.resize(_states->size());
    int i = 0;
    for (StateSet::iterator sit = _states->begin(); sit != _states->end();
            ++sit, ++i) {
        _setpoint[i] = (*sit)->getSetpoint();
    }

    if (getCorrectorKicks(_calcStates, _setpoint, _actuatorDelta) != 0) {
        return -1;
    }

    // TODO: It TrajectoryFit is used by the LTU feedback, replace
    // energy with BY1BDES values

    // Convert theta to BDES per Actuator
    convertActuatorBdes(_actuatorDelta);

    _calculateCorrectorsStats.end();
    _updateDevicesStats.start();

    // Apply an overall gain to actuatorDelta
    _actuatorDelta *= _loopConfiguration->_pGainPv.getValue();

    // Results are in the actuatorDelta and calcStates vectors
    // At this point the current actuator values should be added to the
    // actuatorDelta
    if (updateStates(_calcStates) != 0) {
      return STATE_LIMIT;
    }
    if (updateActuators(_actuatorDelta) != 0) {
        return ACTUATOR_LIMIT;
    }

    // Checking and sending the data will be done in the Loop

    _updateDevicesStats.end();

    return 0;
}

/**
 * Convert calculated actuator values to BDES values, applying energies and
 * some magic numbers...
 *
 * @param actuatorDelta calculated changes in the actuator values
 * @author L.Piccoli
 */
void TrajectoryFit::convertActuatorBdes(Vector& actuatorDelta) {
    std::vector<double>::iterator energyIt = _loopConfiguration->_actEnergy.begin();
    for (Vector::iterator it = actuatorDelta.begin();
            it != actuatorDelta.end(); ++it, ++energyIt) {
        double energy = *energyIt;
        *it = *it * 33.3564 * energy;
	//	std::cout << (*it) << " * " << energy << std::endl;
    }
}

/**
 * Get measurements and copy them to internal vectors.
 */
int TrajectoryFit::getMeasurements() {
    if (_measurements == NULL) {
        return -1;
    }

    int numBpms = _measurements->size() / 2;
    int i = 0;
    bool pulseMismatch = false;
    bool getFailed = false;
    for (MeasurementSet::iterator it = _measurements->begin();
            it != _measurements->end(); ++it, ++i) {
        double meas = 0;
        epicsTimeStamp timestamp;
        MeasurementDevice *measurement = (*it);
        // Make sure the measurement has expected PulseId
        if (measurement->isFcom() && !measurement->isFile()) {
            if (measurement->peekPulseId() !=
                    (epicsUInt32) _loopConfiguration->_pulseIdPv.getValue()) {
                pulseMismatch = true;
            }
        }

        if (measurement->get(meas, timestamp) != 0) {
            getFailed = true;
        }

        if (i < numBpms) {
            // Update the measurementX vector from the measurements
            _measurementX[i] = meas;
        } else {
            // Update the measurementY vector from the measurements
            _measurementY[i - numBpms] = meas;
        }
    }

    if (pulseMismatch) {
        _loopConfiguration->_logger << "Sensor pulse id mismatch, no change"
                << Log::flushpv;
        return -1;
    }
    if (getFailed) {
        _loopConfiguration->_logger << "Cannot read sensors, no change"
                << Log::flushpv;
        return -1;
    }

    return numBpms;
}

/**
 * XY Trajectory Fit algorithm.
 *
 * This is used by the transversal feedbacks.
 *
 * This method replaces the following matlab code:
 *
 *    [Xsf,Ysf,p,dp,chisq,Q] = fbXYTrajFit1(Xs, loop.meas.dXs, Ys, loop.meas.dYs,...
 *                            loop.meas.Xs0, loop.meas.Ys0, R1s, R3s);
 *
 * >> See file fbXYTrajFit1.m for reference.
 *
 * TODO: Need to add some of the output parameters.
 *
 * @param fitX fit trajectory for X position (Xsf)
 * @param fitY fit trajectory for Y position (Ysf)
 * @param states (p)
 * @return 0 success, -1 error
 * @author L.Piccoli
 */
int TrajectoryFit::fit(Vector &fitX, // Xsf
        Vector &fitY, // YsF
        Vector &states) { // p

    _fitPrepStats.start();
    states.resize(_measurementX.size() * 2);

    // Calculate initial conditions (p)
    int size = _measurementX.size();
    for (int i = 0; i < size; ++i) {
        _orbitDifference[i] = _measurementX[i] - _orbitReferenceX[i];
        _orbitDifference[i + size] = _measurementY[i] - _orbitReferenceY[i];
        states[i] = _orbitDifference[i];
        states[i + size] = _orbitDifference[i + size];
    }

    // fitMatrix gets changed by gels(), need to keep a copy
    //    matrix<double, column_major> _fitMatrixGELS = _fitMatrix;
    //    _fitMatrixGELS = _fitMatrix;
    for (int i = 0; i < (int) _fitMatrix.size1(); ++i) {
        for (int j = 0; j < (int) _fitMatrix.size2(); ++j) {
            _fitMatrixGELS(i, j) = _fitMatrix(i, j);
        }
    }
    _fitPrepStats.end();

    // states.size() must be equal to max(fitMatrix.size1(), fitMatrix.size2())
    // otherwise cannon invoke gels()
    if (states.size() < std::max(_fitMatrixGELS.size1(), _fitMatrixGELS.size2())) {
        return -1;
    }

    _fitGelsStats.start();
    int status = gels('N', _fitMatrixGELS, states, workspace(_workspace));
    _fitGelsStats.end();

    _fitPostStats.start();
    if (status != 0) {
        return -1;
    }

    // GELS return the results in the first elements of the vector
    states = project(states, range(0, _fitMatrix.size2()));

    _predictedTrajectory.resize(_orbitDifference.size()); // Ssf
    _predictedTrajectory = prod(_fitMatrix, states);

    fitX = project(_predictedTrajectory, range(0, _measurementX.size()));
    fitY = project(_predictedTrajectory, range(_measurementX.size(),
            _measurementX.size() +
            _measurementY.size()));

    _fitPostStats.end();
    return 0;
}

/**
 * Computes G \ (states-sepoint)' * 1e-3
 *
 * @author L.Piccoli
 */
int TrajectoryFit::getCorrectorKicks(Vector &states, Vector &setpoints,
        Vector &theta) {
    Vector::iterator setpointIt = setpoints.begin();
    Vector::iterator statesIt = states.begin();
    Vector::iterator thetaIt = theta.begin();

    for (; setpointIt != setpoints.end(); ++setpointIt, ++statesIt, ++thetaIt) {
        (*thetaIt) = (*setpointIt) - (*statesIt);
    }
    
    //    theta = states;
    for (int i = 0; i < (int) _loopConfiguration->_gMatrix.size1(); ++i) {
        for (int j = 0; j < (int) _loopConfiguration->_gMatrix.size2(); ++j) {
            _gMatrixGELS(i, j) = _loopConfiguration->_gMatrix(i, j);
        }
    }

    _correctorGelsStats.start();
    int status = gels('N', _gMatrixGELS, theta, workspace(_workspace));
    _correctorGelsStats.end();
    if (status != 0) {
        return -1;
    }

    theta *= 1e-3;

    return 0;
}

/**
 * Update the values for the actuators using the specified vector of deltas.
 * The deltas must be added to the last values successfully written to the
 * ActuatorDevices.
 *
 * TODO: looks like this method is a candidate to be moved to the Algorithm class
 *
 * @return 0 if all actuator values are within limits, -1 if at least
 * one of the values is outside the limits
 * @author L.Piccoli
 */
int TrajectoryFit::updateActuators(Vector& actuatorDelta) throw (Exception) {
    bool actuatorLimit = false;
    int i = 0;
    std::string exceptionMessage = "Actuator(s) out of range:";

    // Fill in the current time and pulseId
    epicsTimeStamp timestamp;
    epicsTimeGetCurrent(&timestamp);
#ifdef RTEMS
    evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());
#endif

    for (ActuatorSet::iterator it = _actuators->begin();
            it != _actuators->end(); ++it, ++i) {
        ActuatorDevice *actuator = (*it);
        double value = actuator->getLastValueSet();
        // Change the actuator value only if MODE PV is ENABLE (true)
        if (_loopConfiguration->_mode == true) {
            value += actuatorDelta[i];
        }
        if (actuator->set(value, timestamp) != 0) {
	  if (actuatorLimit) exceptionMessage += ",";
	  exceptionMessage += " ";
	  exceptionMessage += (*it)->getDeviceName();
	  actuatorLimit = true;
        }
    }

    if (actuatorLimit) {
      throw Exception(exceptionMessage, Exception::ACTUATOR_LIMIT);
        return -1;
    }
    return 0;
}

/**
 * Update values of the states. If new state value exceed hihi/lolo limits,
 * then an error is returned.
 *
 * @param states vector containing updated state values
 * @return 0 on success, -1 if at least one new state value is outside
 * limits
 * @author L.Piccoli
 */
int TrajectoryFit::updateStates(Vector &states) throw (Exception) {
    bool stateLimit = false;
    int i = 0;
    std::string exceptionMessage = "State(s) out of range:";

    // Fill in the current time and pulseId
    epicsTimeStamp timestamp;
    epicsTimeGetCurrent(&timestamp);
#ifdef RTEMS
    evrTimePutPulseID(&timestamp, _loopConfiguration->_pulseIdPv.getValue());
#endif

    Vector::iterator stateIt = states.begin();
    for (StateSet::iterator it = _states->begin();
            it != _states->end(); ++it, ++i, ++stateIt) {
        StateDevice *state = (*it);
        double stateValue = (*stateIt);
        if (state->set(stateValue, timestamp) != 0) {
	  if (stateLimit) exceptionMessage += ",";
	  exceptionMessage += " ";
	  exceptionMessage += (*it)->getDeviceName();
	  stateLimit = true;
        }
    }

    if (stateLimit) {
      throw Exception(exceptionMessage, Exception::STATE_LIMIT);
        return -1;
    }
    return 0;
}

/**
 * Show algorithm information/status.
 *
 * @author L.Piccoli
 */
void TrajectoryFit::show() {
}

/**
 * Show time statistics.
 *
 * @author L.Piccoli
 */
void TrajectoryFit::showDebug() {
    _getMeasStats.show("    ");
    _fitPrepStats.show("    ");
    _fitGelsStats.show("    ");
    _fitPostStats.show("    ");
    _correctorGelsStats.show("    ");
    _calculateCorrectorsStats.show("    ");
    _updateDevicesStats.show("    ");
    _updateDevicesStats.show("    ");
}


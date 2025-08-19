/* 
 * File:   TrajectoryFit.h
 * Author: lpiccoli
 *
 * Created on July 16, 2010, 11:09 AM
 */

#ifndef _TRAJECTORYFIT_H
#define	_TRAJECTORYFIT_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/bindings/lapack/workspace.hpp>
#include <boost/numeric/bindings/traits/traits.hpp>
#include "Defs.h"
#include "LoopConfiguration.h"
#include "Algorithm.h"
#include "TimeUtil.h"

FF_NAMESPACE_START

class TrajectoryFitTest;

/** Definition of a Vector type with fixed size */
typedef boost::numeric::ublas::vector<double,
        boost::numeric::ublas::bounded_array<double,100> > Vector;

/** Definition of a Matrix type with fixed size */
typedef boost::numeric::ublas::matrix<double,
        boost::numeric::ublas::column_major,
        boost::numeric::ublas::bounded_array<double,400> > Matrix;

/**
 * Implementation of the Trajectory Fit algorithm. This class uses
 * boost::ublas and LAPACK (Fortran) libraries.
 *
 * @author L.Piccoli
 */
class TrajectoryFit : public Algorithm {
public:
    TrajectoryFit(std::string algorithmName = "TrajectoryFit");
    virtual ~TrajectoryFit();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states);

    virtual int calculate(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);
    virtual int calculate();
    virtual void show();
    virtual void showDebug();

    friend class TrajectoryFitTest;

protected:
    virtual void convertActuatorBdes(Vector &actuatorDelta);

private:
    int getMeasurements();
    int fit(Vector &fitX, Vector &fitY, Vector &states);
    int getCorrectorKicks(Vector &states, Vector &setpoints, Vector &theta);
    int updateActuators(Vector &actuatorDelta);
    int updateStates(Vector &states);

    /**
     * Vector of X measurements, updated on every iteration from the
     * MeasurementDevices
     */
    Vector _measurementX;

    /**
     * Vector of Y measurements, updated on every iteration from the
     * MeasurementDevices
     */
    Vector _measurementY;

    /** Orbit reference in X, initialized from the loopConfiguration (Xs0) */
    Vector _orbitReferenceX;

    /** Orbit reference in Y, initialized from the loopConfiguration (Ys0) */
    Vector _orbitReferenceY;

    /**
     * Concatenation of X and Y orbit references, first X positions, followed
     * by Y positions
     */
    Vector _orbitDifference;

    /** Trajectory fit for the X position, calculated every iteration */
    Vector _fitX;

    /** Trajectory fit for the Y position, calculated every iteration */
    Vector _fitY;

    /** States calculated by the fit() method */
    Vector _calcStates;

    /** Predicted trajectory, used to calculate _fitX and _fitY */
    Vector _predictedTrajectory;

    /** Changes to be applied to actuators */
    Vector _actuatorDelta;

    /** Setpoint values read from the configuration */
    Vector _setpoint;

    /** Workspace used by the LAPACK gels() routine */
    boost::numeric::bindings::traits::detail::array<double> _workspace;

    /** F Matrix */
    Matrix _fitMatrix;

    /**
     * Copy of the F Matrix used as parameter to gels(). It is modified by
     * gels().
     */
    Matrix _fitMatrixGELS;

    /**
     * Copy of the G Matrix, which is also modified by gels() when computing
     * the corrector kicks.
     */
    Matrix _gMatrixGELS;

    /**
     * Transport matrix initialized from the feedback matrix F. R1X is the
     * upper submatrix of F (upper half)
     */
    Matrix _transportR1X;

    /** 
     * Transport matrix initialized from the feedback matrix F. R3Y is the
     * lower subwatrix of F (lower half)
     */
    Matrix _transportR3Y;

    /** Time statistics for getting the measurements */
    TimeAverage _getMeasStats;

    /** Time statistics prior to calling gels() */
    TimeAverage _fitPrepStats;

    /** Time statistics for gels() call */
    TimeAverage _fitGelsStats;

    /** Time statistics for after gels() */
    TimeAverage _fitPostStats;

    /** Time statistics for calculating corrector kicks */
    TimeAverage _calculateCorrectorsStats;

    /** Time statistics for updating actuator and state devices */
    TimeAverage _updateDevicesStats;

    /** Time statistics for the getCorrectorKicks() method (gels() call) */
    TimeAverage _correctorGelsStats;
};

FF_NAMESPACE_END

#endif	/* _TRAJECTORYFIT_H */


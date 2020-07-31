/* 
 * File:   TrajectoryFitPinv.h
 * Author: lpiccoli
 *
 * Created on May 3, 2011, 14:30 AM
 */

#ifndef _TRAJECTORYFITPINV_H
#define	_TRAJECTORYFITPINV_H

#include "TrajectoryFitBy1.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

FF_NAMESPACE_START

/**
 * Implementation of TrajectoryFit using a fixed set of Measurement/State/Actuator
 * devices.
 *
 * When the algorithm is reconfigured a new F Matrix is loaded.
 *
 * @author L.Piccoli
 */
class TrajectoryFitPinv : public TrajectoryFitBy1 {
public:
    TrajectoryFitPinv(std::string algorithmName = "TrajectoryFitPinv");
    virtual ~TrajectoryFitPinv();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states);
    virtual void show();
    virtual int reset();

    virtual int calculate(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);
    virtual int calculate();

protected:
    /** New G Matrix used to calculate the corrector magnets */
    Matrix _pinvGMatrix;

    void calculateGPseudoInverse();
    void calculateSquarePseudoInverse();
    void calculateNonSquarePseudoInverse();

    virtual int checkConfig();
    virtual int getCorrectorKicks(Vector &states, Vector &setpoints, Vector &theta);

    /** Variables used to calculate pseudoinverse of a rectangular G Matrix */
    boost::numeric::ublas::vector<double> _s;
    Matrix _u;
    Matrix _v;
    Matrix _a;
    Matrix _sMatrix;
    Matrix _sPrime;
};

FF_NAMESPACE_END

#endif	/* _TRAJECTORYFITPINV_H */


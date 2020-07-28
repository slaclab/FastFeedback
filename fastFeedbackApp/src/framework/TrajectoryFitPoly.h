/* 
 * File:   TrajectoryFitPoly.h
 * Author: lpiccoli
 *
 * Created on July 16, 2010, 11:09 AM
 */

#ifndef _TRAJECTORYFITPOLY_H
#define	_TRAJECTORYFITPOLY_H

#include "TrajectoryFitPinv.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

FF_NAMESPACE_START

/**
 * Implementation of TrajectoryFit using a fixed set of Measurement/State/Actuator
 * devices.
 *
 * When the algorithm is reconfigured a new F Matrix is calculated using polynomial
 * values stored in the LoopConfiguration.
 *
 * @author L.Piccoli
 */
class TrajectoryFitPoly : public TrajectoryFitPinv {
public:
    TrajectoryFitPoly(std::string algorithmName = "TrajectoryFitPoly");
    virtual ~TrajectoryFitPoly();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states);
protected:
    void calculateFMatrix();
    Vector calculateEnergies();
    double evaluatePolynomial(boost::numeric::ublas::vector_range<Vector> &coefs, Vector &energies);
};

FF_NAMESPACE_END

#endif	/* _TRAJECTORYFITPOLY_H */


/* 
 * File:   TrajectoryFitEact.h
 * Author: lpiccoli
 *
 * Created on May 10, 2011, 14:05 AM
 */

#ifndef _TRAJECTORYFITEACT_H
#define	_TRAJECTORYFITEACT_H

#include "TrajectoryFitStatic.h"
#include "CaChannel.h"
#include <vector>

FF_NAMESPACE_START

/**
 * Implementation of the Trajectory Fit algorithm using energies
 * from the magnets EACT PVs.
 *
 * @author L.Piccoli
 */
class TrajectoryFitEact : public TrajectoryFitStatic {
public:
    TrajectoryFitEact(std::string algorithmName = "TrajectoryFitEact");
    virtual ~TrajectoryFitEact();

    virtual int reset();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states);

    virtual int calculate(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);
    virtual int calculate();

protected:
    void connect();
    void disconnect();

    /** Vector of Channel Access connections to the actuators EACT values */
    std::vector<CaChannel *> actuatorEnergy;
};

FF_NAMESPACE_END

#endif	/* _TRAJECTORYFITBY1_H */


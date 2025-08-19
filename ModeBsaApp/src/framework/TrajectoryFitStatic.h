/* 
 * File:   TrajectoryFitStatic.h
 * Author: lpiccoli
 *
 * Created on July 16, 2010, 11:09 AM
 */

#ifndef _TRAJECTORYFITSTATIC_H
#define	_TRAJECTORYFITSTATIC_H

#include "TrajectoryFitGeneral.h"

FF_NAMESPACE_START

/**
 * Implementation of TrajectoryFit using a fixed set of Measurement/State/Actuator
 * devices.
 *
 * @author L.Piccoli
 */
class TrajectoryFitStatic : public TrajectoryFitGeneral {
public:
    TrajectoryFitStatic(std::string algorithmName = "TrajectoryFit");
    virtual ~TrajectoryFitStatic();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states);
protected:
    virtual int checkConfig();
};

FF_NAMESPACE_END

#endif	/* _TRAJECTORYFITSTATIC_H */


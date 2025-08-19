/* 
 * File:   TrajectoryFitBy1.h
 * Author: lpiccoli
 *
 * Created on November 10, 2010, 11:15 AM
 */

#ifndef _TRAJECTORYFITBY1_H
#define	_TRAJECTORYFITBY1_H

#include "TrajectoryFitStatic.h"

FF_NAMESPACE_START

/**
 * Implementation of the Trajectory Fit algorithm using energies
 * from the BY1BDES PV.
 *
 * @author L.Piccoli
 */
class TrajectoryFitBy1 : public TrajectoryFitStatic {
public:
    TrajectoryFitBy1(std::string algorithmName = "TrajectoryFitBy1");
    virtual ~TrajectoryFitBy1();

protected:
    virtual void convertActuatorBdes(Vector &actuatorDelta);
};

FF_NAMESPACE_END

#endif	/* _TRAJECTORYFITBY1_H */


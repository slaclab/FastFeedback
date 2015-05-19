#ifndef __FFXYTRAJFIT_H__
#define __FFXYTRAJFIT_H__

#include <boost/numeric/ublas/matrix.hpp>

/**
 * XY Trajectory Fit algorithm
 * 
 * This is used by the transversal feedbacks.
 * 
 * @return 0 success, -1 error
 */
int ffXYTrajFit(const boost::numeric::ublas::vector<double> &measurementX,
		const boost::numeric::ublas::vector<double> &measurementY,
		const boost::numeric::ublas::vector<double> &orbitReferenceX,
		const boost::numeric::ublas::vector<double> &orbitReferenceY,
		const boost::numeric::ublas::matrix<double> &transportR1X,
		const boost::numeric::ublas::matrix<double> &transportR3Y,
		boost::numeric::ublas::vector<double> &fitX,
		boost::numeric::ublas::vector<double> &fitY,
		boost::numeric::ublas::vector<double> &initialConditions);

#endif

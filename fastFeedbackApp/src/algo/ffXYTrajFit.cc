#include <iostream>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#include <boost/numeric/bindings/lapack/workspace.hpp>
#include <boost/numeric/bindings/lapack/gels.hpp>
#include <boost/numeric/bindings/traits/traits.hpp>

#include "ffXYTrajFit.h"

using namespace boost::numeric::ublas;
using namespace boost::numeric::bindings::traits;
using namespace boost::numeric::bindings::lapack;

int ffXYTrajFit(const vector<double> &measurementX,     // Xs
		const vector<double> &measurementY,     // Ys
		const vector<double> &orbitReferenceX,  // Xs0
		const vector<double> &orbitReferenceY,  // Ys0
		const matrix<double> &transportR1X,     // R1s
		const matrix<double> &transportR3Y,     // R3s
		vector<double> &fitX,                   // Xsf
		vector<double> &fitY,                   // YsF
		vector<double> &initialConditions) {    // p

  // Build Ss vector
  vector<double> orbitDifference(measurementY.size() * 2); // Ss

  project(orbitDifference,
	  range(0, measurementX.size())) = measurementX - orbitReferenceX;
  project(orbitDifference,
	  range(measurementX.size(),
		measurementX.size() + measurementY.size())) =
    measurementY - orbitReferenceY;


  // Build Q matrix (fitMatrix), by taking the first four cols of R1s and
  // concatenating to the first four cols of R3s. The fifth column in
  // R1s/R3s is the energy, which is not currently used.
  matrix<double, column_major> fitMatrix(measurementY.size() * 2, 5);

  project(fitMatrix, range(0, transportR1X.size1()),
	  range(0, transportR1X.size2())) = transportR1X;

  project(fitMatrix, range(transportR1X.size1(),
			   transportR1X.size1() + transportR3Y.size1()),
  	  range(0, transportR3Y.size2())) = transportR3Y;

  // Eliminate the energy column
  fitMatrix = subrange(fitMatrix, 0, fitMatrix.size1(),
		       0, fitMatrix.size2() - 1);
  // fitMatrix gets changed by gels(), need to keep a copy
  matrix<double, column_major> fitMatrixGELS = fitMatrix;

  // Calculate initial conditions (p)
  initialConditions = orbitDifference;
  if (gels('N', fitMatrixGELS, initialConditions, optimal_workspace()) != 0) {
    std::cerr << "ERROR: Least squares failed" << std::endl;
  }

  // GELS return the results in the first elements of the vector
  initialConditions = project(initialConditions,
			      range(0, measurementX.size()));

  // sum((Q*p1-Ss).^2); 
  vector<double> v(orbitDifference.size());
  vector<double> predictedTrajectory(orbitDifference.size()); // Ssf

  predictedTrajectory = prod(fitMatrix, initialConditions);
  v = predictedTrajectory - orbitDifference;
  v = element_prod(v, v);
  double chisq1 = sum(v);

  fitX = project(predictedTrajectory, range(0, measurementX.size()));
  fitY = project(predictedTrajectory, range(measurementX.size(),
					    measurementX.size() +
					    measurementY.size()));

  return 0;
}    

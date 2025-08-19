/*
 * File:   TrajectoryFitPoly.cc
 * Author: lpiccoli
 * 
 * Created on March 28, 2011, 11:09 AM
 */

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/bindings/lapack/workspace.hpp>
#include <boost/numeric/bindings/traits/traits.hpp>
#include <boost/numeric/bindings/lapack/lapack.hpp>
#include "TrajectoryFitPoly.h"
#include "ExecConfiguration.h"

USING_FF_NAMESPACE

using namespace boost::lambda;
using namespace boost::numeric::ublas;
using namespace boost::numeric::bindings::traits;
using namespace boost::numeric::bindings::lapack;

/**
 * Class constructor. Only initializes attributes
 *
 * @author L.Piccoli
 */
TrajectoryFitPoly::TrajectoryFitPoly(std::string algorithmName) :
  TrajectoryFitPinv(algorithmName) {
}

TrajectoryFitPoly::~TrajectoryFitPoly() {
}

/**
 * Fills the vector containing the energy^5, energy^4, ..., 1
 * used to calculate the F Matrix elements
 *
 * @return a vector containing the calculated energy values
 * @author L.Piccoli
 */
Vector TrajectoryFitPoly::calculateEnergies() {
  int size = 6;
  Vector res(size);

  for (Vector::iterator it = res.begin(); it != res.end(); ++it) {
    (*it) = 0;
  }

  double energy = ExecConfiguration::getInstance()._by1BdesPv.getValue();
  res(res.size()-1) = 1;
  res(res.size()-2) = energy;
  for (int i = res.size() - 3; i >= 0; --i) {
    res(i) = res(i+1) * energy;
  }
  return res;
}

double TrajectoryFitPoly::evaluatePolynomial(vector_range<Vector> &coefs, Vector &energies) {
  Vector x = element_prod(coefs, energies);
  double sum = 0;
  for_each(x.begin(), x.end(), var(sum) += _1);
  return sum;
}

void TrajectoryFitPoly::calculateFMatrix() {
  _loopConfiguration->_fMatrix.resize(_measurements->size(), 4); // No energy column

  for (unsigned i = 0; i < _loopConfiguration->_fMatrix.size1(); ++i) {
    for (unsigned j = 0; j < _loopConfiguration->_fMatrix.size2(); ++j) {
      _loopConfiguration->_fMatrix(i, j) = 0;
    }
  }

  int maxBpms = _loopConfiguration->_measNumPv.getValue();
  maxBpms /= 2;
  Vector energies = calculateEnergies();
  
  Vector polyvals(_loopConfiguration->_polyvalsPv.getValue().size());
  Vector::iterator dest = polyvals.begin();
  for (std::vector<double>::iterator source = _loopConfiguration->_polyvalsPv.getValue().begin();
       source != _loopConfiguration->_polyvalsPv.getValue().end(); ++source, ++dest) {
    *dest = *source;
  }

  unsigned usedMeasurementCount = 0; // from 0 to number of USED BPMs
  int polyMeasurementCount = 0; // from 0 to 11 (MEASNUM / 2)
  int fMatrixRow = 0;           // from 0 to number of USED BPMs
  for (MeasurementSet::iterator it = _measurements->begin();
       usedMeasurementCount < _measurements->size() / 2; ++usedMeasurementCount, ++it, ++fMatrixRow) {
    int index = (*it)->getDeviceIndex();
    polyMeasurementCount = index - 1;
    int i = polyMeasurementCount;

    // Points to the row containing values for BPM X
    matrix_row<Matrix> fMatrixRowX(_loopConfiguration->_fMatrix, fMatrixRow);
    fMatrixRowX(2) = 0;
    fMatrixRowX(3) = 0;
    
    // Points to the section containing p11 values for BPM X
    int r = i * 6;
    vector_range<Vector> p11(polyvals, range(r, r + 6));
    
    // Points to the section containing p12 values for BPM X - skip all p11 values
    r = i * 6 + 11 * 6;
    vector_range<Vector> p12(polyvals, range(r, r + 6));
    
    double sum = evaluatePolynomial(p11, energies);
    fMatrixRowX(0) = sum;
    sum = evaluatePolynomial(p12, energies);
    fMatrixRowX(1) = sum;
    
    // Points to the row containing values for BPM Y
    matrix_row<Matrix> fMatrixRowY(_loopConfiguration->_fMatrix, fMatrixRow + _measurements->size()/2);
    fMatrixRowY(0) = 0;
    fMatrixRowY(1) = 0;
    
    // Points to the section containing p33 values for BPM Y - skip p11 and p12
    r = i * 6 + 11 * 6 * 2;
    vector_range<Vector> p33(polyvals, range(r, r + 6));
    
    // Points to the section containing p34 values for BPM Y - skip p11, p12 and p33
    r = i * 6 + 11 * 6 * 3;
    vector_range<Vector> p34(polyvals, range(r, r + 6));
    
    sum = evaluatePolynomial(p33, energies);
    fMatrixRowY(2) = sum;
    sum = evaluatePolynomial(p34, energies);
    fMatrixRowY(3) = sum;
  }
}

/**
 * @param configuration pointer to the LoopConfiguration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int TrajectoryFitPoly::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) {
  _loopConfiguration = configuration;
  _measurements = measurements;
  _actuators = actuators;
  _states = states;

  // Calculate the new F Matrix based on the current energy and
  // polynomial values. Only the F Matrix rows for the selected
  // BPMS must be calculated.
  calculateFMatrix();

  // Calculate the pinv(G Matrix) to be used when the correctors
  // are calculated.
  
  /*
  _pinvGMatrix.resize(_loopConfiguration->_gMatrix.size1(), _loopConfiguration->_gMatrix.size2());
  if (gelss(_loopConfiguration->_gMatrix, _pinvGMatrix, workspace(_workspace)) != 0) {
    throw Exception("Failed to calculate pinv(GMatrix)");
  }
  */
  calculateGPseudoInverse();

  return reallyConfig();
}

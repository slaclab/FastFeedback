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
#include "TrajectoryFitPinv.h"

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
TrajectoryFitPinv::TrajectoryFitPinv(std::string algorithmName) :
  TrajectoryFitBy1(algorithmName) {
}

TrajectoryFitPinv::~TrajectoryFitPinv() {
}

// MUTEX should not be necessary - the algorithm and configuration are both
// owned in the same thread - they will always process sequentially

int TrajectoryFitPinv::calculate(LoopConfiguration &configuration,
				 MeasurementSet &measurements,
				 ActuatorSet &actuators,
				 StateSet &states) throw (Exception) {
  //configuration._mutex->lock();
  try {
    return TrajectoryFitStatic::calculate(configuration, measurements,actuators, states);
  } catch (Exception &e) {
    //configuration._mutex->unlock();
    throw e;
  }
  //configuration._mutex->unlock();
  return -1;
}

int TrajectoryFitPinv::calculate() throw (Exception) {
  //_loopConfiguration->_mutex->lock();
  try {
    return TrajectoryFitStatic::calculate();
  } catch (Exception &e) {
    //_loopConfiguration->_mutex->unlock();
    throw e;
  }
  //_loopConfiguration->_mutex->unlock();
  return -1;
}

/**
 * @param configuration pointer to the LoopConfiguration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int TrajectoryFitPinv::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) throw (Exception) {
  _loopConfiguration = configuration;
  _measurements = measurements;
  _actuators = actuators;
  _states = states;

  reset();

  return reallyConfig();
}

int TrajectoryFitPinv::reset() {
  // The F and G Matrices may be reloaded while feedback is being calculated.
  // The mutex is used to make sure those matrices are not in use.

	  // MUTEX should not be necessary - the algorithm and configuration are both
	  // owned in the same thread - they will always process sequentially

  //_loopConfiguration->_mutex->lock();

  // Reload the F Matrix from the FMATRIX PV
  _loopConfiguration->configureFMatrix();

  // Reload the G Matrix from the GMATRIX PV
  _loopConfiguration->configureGMatrix();

  // Calculate the pinv(G Matrix) to be used when the correctors
  // are calculated.
  calculateGPseudoInverse();
  
  //_loopConfiguration->_mutex->unlock();

  return 0;
}

int TrajectoryFitPinv::checkConfig() {
  return 0;
};

/**
 * Computes (pinv(G) * (states-sepoint)') * 1e-3
 *
 * @author L.Piccoli
 */
int TrajectoryFitPinv::getCorrectorKicks(Vector &states, Vector &setpoints,
        Vector &theta) {
    Vector::iterator setpointIt = setpoints.begin();
    Vector::iterator statesIt = states.begin();
    Vector::iterator thetaIt = theta.begin();

    for (; setpointIt != setpoints.end(); ++setpointIt, ++statesIt, ++thetaIt) {
        (*thetaIt) = (*setpointIt) - (*statesIt);
    }

    theta = prod(_pinvGMatrix, theta);
    theta *= 1e-3;

    return 0;
}

/**
 * Calculates the pseudo inverse for the G Matrix. The method to calculate the
 * pseudo inverse depends whether G Matrix is square or not.
 *
 * @author L.Piccoli
 */
void TrajectoryFitPinv::calculateGPseudoInverse() {
  if (_loopConfiguration->_gMatrix.size1() == _loopConfiguration->_gMatrix.size2()){
    calculateSquarePseudoInverse();
  }
  else {
    calculateNonSquarePseudoInverse();
  }
}

/**
 * Calculate the pseudo inverse for a square G Matrix. It invokes the lapack gelss()
 * function.
 * 
 * @author L.Piccoli
 */
void TrajectoryFitPinv::calculateSquarePseudoInverse() {
  _pinvGMatrix.resize(_loopConfiguration->_gMatrix.size1(), _loopConfiguration->_gMatrix.size2());
  if (gelss(_loopConfiguration->_gMatrix, _pinvGMatrix, workspace(_workspace)) != 0) {
    throw Exception("Failed to calculate pseudo inverse of G Matrix");
  }
}

/**
 * Calculate the pseudo inverse for a non-square G Matrix. The lapack gesvd() and gelss() are
 * used.
 *
 * @author L.Piccoli
 */
void TrajectoryFitPinv::calculateNonSquarePseudoInverse() {
  unsigned M = _loopConfiguration->_gMatrix.size1();
  unsigned N = _loopConfiguration->_gMatrix.size2();

  _s.resize(M);
  _u.resize(M,M);
  _v.resize(N,N);
  _a.resize(M,N);

  _u.clear();
  _v.clear();
  _a.clear();
  _s.clear();

  for (unsigned i = 0; i < M; ++i) {
    for (unsigned j = 0; j < N; ++j) {
      _a(i,j) = _loopConfiguration->_gMatrix(i,j);
    }
  }

  for (unsigned i = 0; i < _s.size(); ++i) {
    _s[i] = 1;
  }

  /** DEBUG
  std::cout << "  _a(GMATRIX): " << std::endl;
  for (int i = 0; i < (int) _a.size1(); ++i) {
    std::cout << "  ";
    for (int j = 0; j < (int) _a.size2(); ++j) {
      std::cout << _a(i, j) << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "  _s: " << std::endl;
  for (int i = 0; i < (int) _s.size(); ++i) {
    std::cout << _s(i) << " ";
  }
  std::cout << std::endl;
   DEBUG **/

  int status = 0;
  try {
    status = gesvd(_a, _s, _u, _v);
  } catch (...) {
    throw Exception("Failed to find SVD of G Matrix");
  }
  if (status != 0) {
    throw Exception("Failed to find SVD of G Matrix");
  }


  _sMatrix.resize(M,M);
  _sMatrix.clear();
  unsigned z = 0;
  for (unsigned i = 0; i < _sMatrix.size1(); ++i) {
    for (unsigned j = 0; j < _sMatrix.size2(); ++j) {
      _sMatrix(i,j) = 0;
      if (i == j) { 
	if (z < _s.size()) {
	  _sMatrix(i,j) = _s(z);
	}
	++z;
      }
    }
  }

  // Find the pseudo inverse of S
  _sPrime.resize(N,M);
  _sPrime.clear();
  for (unsigned i = 0; i < 4; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      if (i == j) {
	_sPrime(i,j) = 1;
      }
    }
  }

  //  boost::numeric::bindings::traits::detail::array<double> _workspace(200);
  status = gelss(_sMatrix, _sPrime, workspace(_workspace));
  if (status != 0) {
    throw Exception("Failed to find pseudo inverse of G Matrix");
  }

  /** DEBUG
  std::cout << "  _s(eigenvalues): " << std::endl;
  for (int i = 0; i < (int) _s.size(); ++i) {
    std::cout << _s(i) << " ";
  }
  std::cout << std::endl;
  std::cout << "  _u: " << std::endl;
  for (int i = 0; i < (int) _u.size1(); ++i) {
    std::cout << "  ";
    for (int j = 0; j < (int) _u.size2(); ++j) {
      std::cout << _u(i, j) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "  _v: " << std::endl;
  for (int i = 0; i < (int) _v.size1(); ++i) {
    std::cout << "  ";
    for (int j = 0; j < (int) _v.size2(); ++j) {
      std::cout << _v(i, j) << " ";
    }
    std::cout << std::endl;
  }
 DEBUG **/

  // Now compute the pseudo inverse of X!
  // Which is X+ = V * _sprime * Utransposed
  _u = trans(_u);
  _v = trans(_v); 

  _a = prod(_v, _sPrime);
  _pinvGMatrix = prod(_a, _u);

  /** DEBUG
  std::cout << "  _a(pseudo-inverse): " << std::endl;
  for (int i = 0; i < (int) _a.size1(); ++i) {
    std::cout << "  ";
    for (int j = 0; j < (int) _a.size2(); ++j) {
      std::cout << _a(i, j) << " ";
    }
    std::cout << std::endl;
  }
  DEBUG **/

  if (std::isnan(_a(0,0))) {
    std::cout << "NAN NAN NAN --- nan pseudo inverse found!" << std::endl;
    throw Exception("Calculated NAN pseudo inverse of G Matrix");
  }
}

void TrajectoryFitPinv::show() {
  std::cout << "  pinv(GMatrix): " << std::endl;
  for (int i = 0; i < (int) _pinvGMatrix.size1(); ++i) {
    std::cout << "  ";
    for (int j = 0; j < (int) _pinvGMatrix.size2(); ++j) {
      std::cout << _pinvGMatrix(i, j) << " ";
    }
    std::cout << std::endl;
  }
}

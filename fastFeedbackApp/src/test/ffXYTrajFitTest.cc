#include "ffXYTrajFitTest.h"
#include "../algo/ffXYTrajFit.h"
#include <cmath>
#include <iostream>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ffXYTrajFitTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

using namespace boost::numeric::ublas;

void ffXYTrajFitTest::testTrajectory() {
  int numBpms = 4;
  int rmatCols = 5;
  vector<double> measurementX(numBpms);
  vector<double> measurementY(numBpms);
  vector<double> orbitReferenceX(numBpms);
  vector<double> orbitReferenceY(numBpms);
  matrix<double> transportR1X(numBpms, rmatCols);
  matrix<double> transportR3Y(numBpms, rmatCols);
  vector<double> fitX(numBpms);
  vector<double> fitY(numBpms);
  vector<double> initialConditions;
  vector<double> matlabFitX(numBpms);
  vector<double> matlabFitY(numBpms);

  // Fake measurements
  for (int i = 0; i < numBpms; ++i) {
    measurementX(i) = 1;
    measurementY(i) = 1;
  }

  // Fit values from Matlab
  matlabFitX(0) = 0.7401;
  matlabFitX(1) = 1.1143;
  matlabFitX(2) = 1.2460;
  matlabFitX(3) = 0.6843;
  matlabFitY(0) = 1.0863;
  matlabFitY(1) = 0.9795;
  matlabFitY(2) = 0.9558;
  matlabFitY(3) = 1.0768;

  // Actual Xs0/Ys0 reference orbit values
  orbitReferenceX(0) = 0.1077;
  orbitReferenceX(1) = -0.0886;
  orbitReferenceX(2) = -0.0060;
  orbitReferenceX(3) = 0.014;
  orbitReferenceY(0) = -0.0735;
  orbitReferenceY(1) = 0.0550;
  orbitReferenceY(2) = -0.0825;
  orbitReferenceY(3) = -0.0087;

  // Actual transport matrices
  transportR1X(0,0) = 1;
  transportR1X(0,1) = 0;
  transportR1X(0,2) = 0;
  transportR1X(0,3) = 0;
  transportR1X(0,4) = 0;

  transportR1X(1,0) = 0.4086;
  transportR1X(1,1) = 10.9938;
  transportR1X(1,2) = 0;
  transportR1X(1,3) = 0;
  transportR1X(1,4) = 0;

  transportR1X(2,0) = -2.7723;
  transportR1X(2,1) = 44.6555;
  transportR1X(2,2) = 0;
  transportR1X(2,3) = 0;
  transportR1X(2,4) = -0.0001;

  transportR1X(3,0) = -2.5674;
  transportR1X(3,1) = 34.9957;
  transportR1X(3,2) = 0;
  transportR1X(3,3) = 0;
  transportR1X(3,4) = -0.0001;
  
  transportR3Y(0,0) = 0;
  transportR3Y(0,1) = 0;
  transportR3Y(0,2) = 1;
  transportR3Y(0,3) = 0;
  transportR3Y(0,4) = 0;

  transportR3Y(1,0) = 0;
  transportR3Y(1,1) = 0.0003;
  transportR3Y(1,2) = 1.5934;
  transportR3Y(1,3) = 11.0950;
  transportR3Y(1,4) = 0;

  transportR3Y(2,0) = 0;
  transportR3Y(2,1) = 0.0013;
  transportR3Y(2,2) = 3.0611;
  transportR3Y(2,3) = 34.9879;
  transportR3Y(2,4) = 0.0001;

  transportR3Y(3,0) = -0.0001;
  transportR3Y(3,1) = 0.0029;
  transportR3Y(3,2) = 4.6980;
  transportR3Y(3,3) = 59.4554;
  transportR3Y(3,4) = 0.0002;
  

  CPPUNIT_ASSERT_EQUAL(0, ffXYTrajFit(measurementX, measurementY,
				      orbitReferenceX, orbitReferenceY,
				      transportR1X, transportR3Y,
				      fitX, fitY, initialConditions));

  for (unsigned int i = 0; i < measurementY.size(); ++i) {
    // There is a difference between the Matlab results and
    // uBlas/Lapack results
    double error = 0.006; 
    if (std::abs(matlabFitY(i) - fitY(i)) > error) {
      std::stringstream stream;
      stream << "Actual=" << matlabFitY(i) << " Expected=" << fitY(i);
      CPPUNIT_ASSERT_MESSAGE(stream.str(), false);
    }
    if (std::abs(matlabFitX(i) - fitX(i)) > error) {
      std::stringstream stream;
      stream << "Actual=" << matlabFitX(i) << " Expected=" << fitX(i);
      CPPUNIT_ASSERT_MESSAGE(stream.str(), false);
    }
  }

  CPPUNIT_ASSERT(true);
};


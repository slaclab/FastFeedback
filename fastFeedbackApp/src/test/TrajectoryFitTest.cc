#include "TrajectoryFitTest.h"
#include "TrajectoryFit.h"
#include "TimeUtil.h"
#include "NullChannel.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#include <boost/numeric/bindings/lapack/workspace.hpp>
#include <boost/numeric/bindings/lapack/gels.hpp>
#include <boost/numeric/bindings/traits/traits.hpp>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::TrajectoryFitTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

#ifdef RTEMS
#define TEST_DIR "/boot/g/lcls/vol8/epics/iocTop/FFController/Development/test/"
#else
#define TEST_DIR "/afs/slac/g/lcls/epics/iocTop/FFController/Development/test/"
#endif

        using namespace boost::numeric::ublas;
using namespace boost::numeric::bindings::traits;
using namespace boost::numeric::bindings::lapack;

void writeMatrix(Matrix &fitMatrix) {
    fitMatrix.resize(10, 4);
    fitMatrix(0, 0) = 1;
    fitMatrix(0, 1) = 0;
    fitMatrix(0, 2) = 0;
    fitMatrix(0, 3) = 0;

    fitMatrix(1, 0) = 0.97811;
    fitMatrix(1, 1) = 1.9127;
    fitMatrix(1, 2) = 0;
    fitMatrix(1, 3) = 0;

    fitMatrix(2, 0) = 0.12335;
    fitMatrix(2, 1) = 0.80696;
    fitMatrix(2, 2) = 0;
    fitMatrix(2, 3) = 0;

    fitMatrix(3, 0) = 1.65;
    fitMatrix(3, 1) = -0.33597;
    fitMatrix(3, 2) = 0;
    fitMatrix(3, 3) = 0;

    fitMatrix(4, 0) = 3.1276;
    fitMatrix(4, 1) = -0.39243;
    fitMatrix(4, 2) = 0;
    fitMatrix(4, 3) = 0;

    fitMatrix(5, 0) = 0;
    fitMatrix(5, 1) = 0;
    fitMatrix(5, 2) = 1;
    fitMatrix(5, 3) = 0;

    fitMatrix(6, 0) = 0;
    fitMatrix(6, 1) = 0;
    fitMatrix(6, 2) = 1.022;
    fitMatrix(6, 3) = 1.997;

    fitMatrix(7, 0) = 0;
    fitMatrix(7, 1) = 0;
    fitMatrix(7, 2) = 1.8976;
    fitMatrix(7, 3) = 4.2539;

    fitMatrix(8, 0) = 0;
    fitMatrix(8, 1) = 0;
    fitMatrix(8, 2) = -0.17824;
    fitMatrix(8, 3) = 2.3499;

    fitMatrix(9, 0) = 0;
    fitMatrix(9, 1) = 0;
    fitMatrix(9, 2) = -0.29402;
    fitMatrix(9, 3) = 1.5882;
}

/**
 * Compare values calculated by C++ with Injector Launch Matlab feedback
 */
void TrajectoryFitTest::testCalculateInjLaunch() {
    LoopConfiguration config;
    TrajectoryFit t;
    int numBpms = 5;
    int rmatCols = 4;

    config._logger.logToConsole();

    // G Matrix is read from the LoopConfiguration._gMatrix
    int numStates = 4; // G Matrix rows
    int numAct = 4; // G Matrix cols
    config._gMatrix.resize(numStates, numAct);
    config._gMatrix(0, 0) = 2.5181;
    config._gMatrix(0, 1) = 2.8809;
    config._gMatrix(0, 2) = 0;
    config._gMatrix(0, 3) = 0;
    config._gMatrix(1, 0) = -0.33363;
    config._gMatrix(1, 1) = 1.1087;
    config._gMatrix(1, 2) = 0;
    config._gMatrix(1, 3) = 0;
    config._gMatrix(2, 0) = 0;
    config._gMatrix(2, 1) = 0;
    config._gMatrix(2, 2) = -0.34556;
    config._gMatrix(2, 3) = 2.3591;
    config._gMatrix(3, 0) = 0;
    config._gMatrix(3, 1) = 0;
    config._gMatrix(3, 2) = -0.5343;
    config._gMatrix(3, 3) = 0.89322;

    // F Matrix
    config._fMatrix.resize(numBpms * 2, rmatCols);
    writeMatrix(config._fMatrix);

    // Reference orbit
    config._refOrbit.clear();
    config._refOrbit.resize(10);
    config._refOrbit[0] = -0.005313;
    config._refOrbit[1] = -0.12648;
    config._refOrbit[2] = -0.13236;
    config._refOrbit[3] = 0.051098;
    config._refOrbit[4] = -0.082934;
    config._refOrbit[5] = -0.10219;
    config._refOrbit[6] = -0.0022403;
    config._refOrbit[7] = -0.03867;
    config._refOrbit[8] = 0.10587;
    config._refOrbit[9] = -0.16693;

    // Actuator energy
    config._actEnergy.clear();
    config._actEnergy.resize(4);
    config._actEnergy[0] = 0.073587;
    config._actEnergy[1] = 0.135;
    config._actEnergy[2] = 0.073587;
    config._actEnergy[3] = 0.135;

    // Let the TrajectoryFit use the above configuration
    t._loopConfiguration = &config;

    // Create 10 Measurements (5 BPMs), insert values to be read by
    // TrajectoryFit
    MeasurementSet measurements;
    MeasurementDevice *M1 = new MeasurementDevice("XX00", "M1", 10);
    MeasurementDevice *M2 = new MeasurementDevice("XX00", "M2", 10);
    MeasurementDevice *M3 = new MeasurementDevice("XX00", "M3", 10);
    MeasurementDevice *M4 = new MeasurementDevice("XX00", "M4", 10);
    MeasurementDevice *M5 = new MeasurementDevice("XX00", "M5", 10);
    MeasurementDevice *M6 = new MeasurementDevice("XX00", "M6", 10);
    MeasurementDevice *M7 = new MeasurementDevice("XX00", "M7", 10);
    MeasurementDevice *M8 = new MeasurementDevice("XX00", "M8", 10);
    MeasurementDevice *M9 = new MeasurementDevice("XX00", "M9", 10);
    MeasurementDevice *M10 = new MeasurementDevice("XX00", "M10", 10);
    epicsTimeStamp timestamp;

    // Fill in the actual measurements
    M1->insert(0.0951, timestamp);
    M2->insert(-0.1331, timestamp);
    M3->insert(-0.1575, timestamp);
    M4->insert(0.0503, timestamp);
    M5->insert(-0.0535, timestamp);
    M6->insert(-0.1568, timestamp);
    M7->insert(-0.0012, timestamp);
    M8->insert(0.0184, timestamp);
    M9->insert(0.0372, timestamp);
    M10->insert(-0.1165, timestamp);
    measurements.insert(M1);
    measurements.insert(M2);
    measurements.insert(M3);
    measurements.insert(M4);
    measurements.insert(M5);
    measurements.insert(M6);
    measurements.insert(M7);
    measurements.insert(M8);
    measurements.insert(M9);
    measurements.insert(M10);
    t._measurements = &measurements;

    // Create 4 Actuators
    ActuatorSet actuators;
    ActuatorDevice *A1 = new ActuatorDevice("XX00", "A1", 10);
    ActuatorDevice *A2 = new ActuatorDevice("XX00", "A2", 10);
    ActuatorDevice *A3 = new ActuatorDevice("XX00", "A3", 10);
    ActuatorDevice *A4 = new ActuatorDevice("XX00", "A4", 10);
    NullChannel *nc = new NullChannel(CommunicationChannel::WRITE_ONLY, true);
    A1->setCommunicationChannel(nc);
    A2->setCommunicationChannel(nc);
    A3->setCommunicationChannel(nc);
    A4->setCommunicationChannel(nc);
    actuators.insert(A1);
    actuators.insert(A2);
    actuators.insert(A3);
    actuators.insert(A4);

    // Inital Actuators = 0.9435 (* 1e-3) -0.0013 0.0022 0.0018
    A1->set(0.0009435);
    A1->write();
    A2->set(-0.0013);
    A2->write();
    A3->set(0.0022);
    A3->write();
    A4->set(0.0018);
    A4->write();

    t._actuators = &actuators;

    // Create 4 States (and setpoints)
    StateSet states;
    StateDevice *S1 = new StateDevice("XX00", "S1", 10);
    StateDevice *S2 = new StateDevice("XX00", "S2", 10);
    StateDevice *S3 = new StateDevice("XX00", "S3", 10);
    StateDevice *S4 = new StateDevice("XX00", "S4", 10);
    states.insert(S1);
    states.insert(S2);
    states.insert(S3);
    states.insert(S4);
    t._states = &states;

    SetpointDevice *SP = new SetpointDevice("XX00", "SP", 10);
    S1->setSetpointDevice(SP);
    S2->setSetpointDevice(SP);
    S3->setSetpointDevice(SP);
    S4->setSetpointDevice(SP);

    // Set devices
    t._measurements = &measurements;
    t._actuators = &actuators;
    t._states = &states;
    t.configure(&config, &measurements, &actuators, &states);

    // Calculate!!!
    CPPUNIT_ASSERT_EQUAL(0, t.calculate());

    // Expected results
    Vector expectedStates;
    expectedStates[0] = 0.0127;
    expectedStates[1] = -0.0102;
    expectedStates[2] = 0.0007;
    expectedStates[3] = 0.0052;

    int i = 0;
    StateSet::iterator sit;
    for (sit = states.begin(); sit != states.end(); ++sit, ++i) {
        StateDevice *state = (*sit);
        state->write();
        double val = state->peek();
        if (std::abs(val - expectedStates[i]) > 0.001) {
            std::cout << "Got " << val << " expected " << expectedStates[i] << std::endl;
            CPPUNIT_ASSERT(false);
        }
    }

    Vector expectedActuators;
    expectedActuators[0] = 0.0009151101354;
    expectedActuators[1] = -0.001325704;
    expectedActuators[2] = 0.0022301044417;
    expectedActuators[3] = 0.00180675;

    ActuatorSet::iterator ait;
    i = 0;
    for (ait = actuators.begin(); ait != actuators.end(); ++ait, ++i) {
        ActuatorDevice *act = (*ait);
        act->write(false);
        double val = act->peek();
        if (std::abs(val - expectedActuators[i]) > 0.001) {
            std::cout << "Got " << val << " expected " << expectedActuators[i] << std::endl;
            CPPUNIT_ASSERT(false);
        }
    }
}

void TrajectoryFitTest::testGelsTime() {
    Time before;
    Time after;
    Time difference;

    // On RTEMS these took 200 usec:
    // [10,4]((1,0,0,0),(0.97811,1.9127,0,0),(0.12335,0.80696,0,0),(1.65,-0.33597,0,0),(3.1276,-0.39243,0,0),(0,0,1,0),(0,0,1.022,1.997),(0,0,1.8976,4.2539),(0,0,-0.17824,2.3499),(0,0,-0.29402,1.5882))
    // [10](0.118356,0.130582,-0.00892129,-0.0582374,0.0600485,-0.0330206,0.0287396,0.0732868,-0.0333315,0.129846)
    // [10](0.0225912,0.0511977,-0.0166962,0.0216112,0.0353789,0.0598412,0.0356673,0.0620973,-0.153407,0.0325754)

    Matrix fitMatrix;
    Vector states;
    Vector result;

    writeMatrix(fitMatrix);

    states.resize(10);
    states[0] = 0.118356;
    states[1] = 0.130582;
    states[2] = -0.00892129;
    states[3] = -0.0582374;
    states[4] = 0.0600485;
    states[5] = -0.0330206;
    states[6] = 0.0287396;
    states[7] = 0.0732868;
    states[8] = -0.0333315;
    states[9] = 0.129846;

    result.resize(10);
    result[0] = 0.0225912;
    result[1] = 0.0511977;
    result[2] = -0.0166962;
    result[3] = 0.0216112;
    result[4] = 0.0353789;
    result[5] = 0.0598412;
    result[6] = 0.0356673;
    result[7] = 0.0620973;
    result[8] = -0.153407;
    result[9] = 0.0325754;

    boost::numeric::bindings::traits::detail::array<double> w(100);

    before.now();
    int status = gels('N', fitMatrix, states, workspace(w));
    after.now();
    difference = after - before;
    //    std::cout << "status=" << status << " time=" << difference.toMicros() << std::endl;
    long elapsed = difference.toMicros();
    long expected = 3000;
    CPPUNIT_ASSERT(elapsed < expected);

    for (int i = 0; i < 10; ++i) {
        CPPUNIT_ASSERT(std::abs(states [i] - result [i]) < 0.001);
    }

    // This one takes longer!
    //    	  [10,4]((1,0,0,0),(0.97811,1.9127,0,0),(0.12335,0.80696,0,0),(1.65,-0.33597,0,0),(3.1276,-0.39243,0,0),(0,0,1,0),(0,0,1.022,1.997),(0,0,1.8976,4.2539),(0,0,-0.17824,2.3499),(0,0,-0.29402,1.5882))
    //	  [10](0.130961,0.135607,-0.00563554,-0.0228527,0.112262,-0.035836,0.0213231,0.0595639,-0.036705,0.124981)
    //	  [10](0.0392094,0.046117,-0.0191454,0.0194126,0.0345065,0.0542348,0.0340019,0.0614051,-0.146159,0.0353734)

    writeMatrix(fitMatrix);

    states[0] = 0.130961;
    states[1] = 0.135607;
    states[2] = -0.00563554;
    states[3] = -0.0228527;
    states[4] = 0.112262;
    states[5] = -0.035836;
    states[6] = 0.0213231;
    states[7] = 0.0595639;
    states[8] = -0.036705;
    states[9] = 0.124981;

    result[0] = 0.0392094;
    result[1] = 0.046117;
    result[2] = -0.0191454;
    result[3] = 0.0194126;
    result[4] = 0.0345065;
    result[5] = 0.0542348;
    result[6] = 0.0340019;
    result[7] = 0.0614051;
    result[8] = -0.146159;
    result[9] = 0.0353734;

    before.now();
    //    status = gels('N', fitMatrix, states, optimal_workspace());
    status = gels('N', fitMatrix, states, workspace(w));
    after.now();
    difference = after - before;
    //    std::cout << "status=" << status << " time=" << difference.toMicros() << std::endl;
    elapsed = difference.toMicros();
    expected = 3000;
    CPPUNIT_ASSERT(elapsed < expected);

    for (int i = 0; i < 10; ++i) {
        if (std::abs(states [i] - result [i]) > 0.001) {
            std::cout << "Got " << states[i] << " expected " << result[i] << std::endl;
            CPPUNIT_ASSERT(false);
        }
    }
}

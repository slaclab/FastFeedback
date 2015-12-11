/* 
 * File:   Longitudinal.h
 * Author: lpiccoli
 *
 * Created on November 18, 2010, 13:27
 */

#ifndef _LONGITUDINAL_H
#define	_LONGITUDINAL_H

#include <boost/numeric/ublas/matrix.hpp>
#include "Defs.h"
#include "LoopConfiguration.h"
#include "Algorithm.h"
#include "TimeUtil.h"
#include "MeasurementAverage.h"
#include "CaChannel.h"

FF_NAMESPACE_START

/** Definition of a Vector type with fixed size */
typedef boost::numeric::ublas::vector<double,
boost::numeric::ublas::bounded_array<double, 100 > > Vector;

/** Definition of a Matrix type with fixed size */
typedef boost::numeric::ublas::matrix<double,
boost::numeric::ublas::column_major,
boost::numeric::ublas::bounded_array<double, 400 > > Matrix;

enum MeasurementStatus {
    VALID,
    INVALID,
    M10ENERGY,
};

enum StateStatus {
  STATE_INITIAL,
  STATE_JUST_ON,
  STATE_ON,
  STATE_OFF,
};

enum StateIndex {
  STATE_DL1_ENERGY,
  STATE_BC1_ENERGY,
  STATE_BC1_CURRENT,
  STATE_BC2_ENERGY,
  STATE_BC2_CURRENT,
  STATE_DL2_ENERGY,
  MAX_LONGITUDINAL_STATES,
};

/**
 * Number of Measurement Devices used by the Longitudinal Feedback.
 * There are 7 BPMs and 2 BLENs:
 *
 *   M1 BPMS:IN20:731:X
 *   M2 BPMS:IN20:981:X
 *   M3 BPMS:LI21:233:X
 *   M4 BLEN:LI21:265:AIMAXF2
 *   M5 BPMS:LI24:801:X
 *   M6 BLEN:LI24:886:BIMAXF2
 *   M7 BPMS:BSY0:52:X
 *   M8 BPMS:LTU1:250:X
 *   M9 BPMS:LTU1:450:X
 */
enum MeasurementIndex {
    BPMS_IN20_731,
    BPMS_IN20_981,
    BPMS_LI21_233,
    BLEN_LI21_265,
    BPMS_LI24_801,
    BLEN_LI24_886,
    BPMS_BSY0_52,
    BPMS_LTU1_250,
    BPMS_LTU1_450,
    BPMS_LTU0_170,
    MAX_LONGITUDINAL_MEASUREMENTS,
    NO_MEASUREMENT,
    M1 = BPMS_IN20_731,
    M2 = BPMS_IN20_981,
    M3 = BPMS_LI21_233,
    M4 = BLEN_LI21_265,
    M5 = BPMS_LI24_801,
    M6 = BLEN_LI24_886,
    M7 = BPMS_BSY0_52,
    M8 = BPMS_LTU1_250,
    M9 = BPMS_LTU1_450,
    M10 = BPMS_LTU0_170,
};

/**
 * This is the total number of calculated energy elements. The number is
 * slightly smalled than the number of BPMs because either BPMS_IN20_731
 * or BPMS_IN20_981 are used at one time and BPMS_LTU1_250/BPMS_LTU1_450 or
 * BPMS_BSY0_52 are used at one time. The BPMS_LTU1_250/BPMS_LTU1_450 are
 * combined into one energy value.
 */
enum EnergyLocation {
    DL1_ENERGY,
    BC1_ENERGY,
    BC2_ENERGY,
    DL2_ENERGY,
    INTEGRAL_ENERGY_ELEMENTS
};

/**
 * Total number of calculated current elements. This is equivalent of the
 * number of BLENs in the system -> 2
 */
enum CurrentLocation {
    BC1_CURRENT,
    BC2_CURRENT,
    INTEGRAL_CURRENT_ELEMENTS
};

/** Number of averages for the integral energy/current */
const int INTEGRAL_AVERAGE_COUNT = 1;

/** Definition of a Matrix type with fixed size */
typedef boost::numeric::ublas::matrix<double,
boost::numeric::ublas::column_major,
boost::numeric::ublas::bounded_array<double, 400 > > Matrix;

/** Definition of a Vector type with fixed size */
typedef boost::numeric::ublas::vector<double,
        boost::numeric::ublas::bounded_array<double,100> > Vector;

/**
 * Dimensions of F and G matrices for Longitudinal algorithm. Both
 * matrices are 6x6.
 */
const int LONGITUDINAL_MATRIX_DIMENSION = 6;
const int LONGITUDINAL_F_MATRIX_ROWS = LONGITUDINAL_MATRIX_DIMENSION;
const int LONGITUDINAL_F_MATRIX_COLS = LONGITUDINAL_MATRIX_DIMENSION;
const int LONGITUDINAL_G_MATRIX_ROWS = LONGITUDINAL_MATRIX_DIMENSION;
const int LONGITUDINAL_G_MATRIX_COLS = LONGITUDINAL_MATRIX_DIMENSION;

/** Number of elements used to calculated the BLEN average raw measurement */
const int BLEN_AVERAGE_ELEMENTS = 10;

/**
 * Implementation of the Longitudinal algorithm.
 *
 * @author L.Piccoli
 */
class Longitudinal : public Algorithm {
public:
    Longitudinal(std::string algorithmName = "Longitudinal");
    virtual ~Longitudinal();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states) throw (Exception);

    virtual int calculate(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states) throw (Exception);
    virtual int calculate() throw (Exception);
    int setFeedbackDevices(bool measurements, bool actuators);
    virtual void show();
    virtual void showDebug();
    virtual int reset();
    virtual void setActuatorsReference();

protected:
    void checkActuatorLimits();
    int checkConfiguration() throw (Exception);
    virtual int checkMeasurementStatus();
    //    virtual int selectStatesSAVE() throw (Exception);
    virtual int selectStates() throw (Exception);
    //    void setMeasurementUsedBy();
    void setActuatorFeedbackPvs();
    void setPaths();
    void configureMatrix();
    int calculateEnergiesAndCurrents();
    virtual int updateStates() throw (Exception);
    int calculateCurrentState(StateDevice *state, double latestValue);
    int calculateEnergyState(StateDevice *state, double latestValue, double vernier);
    int updateActuatorsDelta();
    virtual int updateActuators() throw (Exception);
    int connectA6Limits();

    double calculateEnergyDl1(MeasurementDevice *m1, MeasurementDevice *m2,
            StateDevice *s1);
    double calculateEnergyBc1(MeasurementDevice *m3, StateDevice *s2);
    double calculateCurrentBc1(StateDevice *s3);
    double calculateEnergyBc2(MeasurementDevice *m5, StateDevice *s4);
    double calculateCurrentBc2(StateDevice *s5);
    double calculateEnergyDl2(MeasurementDevice *m7, MeasurementDevice *m8,
    MeasurementDevice *m9, MeasurementDevice *m10, StateDevice *s6);

    /** Pointer to the DL2 energy actuator, whose limits are updated at beam rate */
    ActuatorDevice *_dl2EnergyActuator;

    /**
     * Arrays of bools indicating whether a measurement device is to
     * be used in the current feedback cycle or not based on TMIT and IMAX
     * values.
     */
    MeasurementStatus _measurementStatus[MAX_LONGITUDINAL_MEASUREMENTS];

    /**
     * Flag used in the calculate() method to record the status of the
     * actuators FBCK PVs. The initial status is recorded when calculate()
     * is invoked the first time.
     */
    bool _firstCalculate;

    /**
     * Keep the last state of the $(LOOP):SxxUSED PV. If a change in the
     * value of the PV is detected, the corresponding Actuator FBCK PV is
     * turned on or off. This allow operators to have control over the actuators
     * while the state is not used.
     */
    bool _actuatorFbckState[MAX_LONGITUDINAL_MEASUREMENTS];

    StateStatus _stateStatus[MAX_LONGITUDINAL_STATES];

    /**
     * Array of moving averages for all Longitudital calculated energies (BPMs).
     * These averages (of last 3 values) are used for the integral feedback part.
     */
    MeasurementAverage *_energyAverage[INTEGRAL_ENERGY_ELEMENTS];

    /**
     * Array of moving averages for all Longitudital calculated currents (BLENs).
     * These averages (of last 3 values) are used for the integral feedback part.
     */
    MeasurementAverage *_currentAverage[INTEGRAL_CURRENT_ELEMENTS];

    /** Keep an average of the last 10 BLEN measurements at BC1 */
    MeasurementAverage _blenBc1Average;

    /** Keep an average of the last 10 BLEN measurements at BC2 */
    MeasurementAverage _blenBc2Average;

    /**
     * This attribute contains the BPM measurement index though which the beam
     * is passing on DL1.
     *
     * Possible values are:
     *    BPMS_IN20_731 or M1
     *    BPMS_IN20_981 or M2
     *    NO_MEASUREMENT -> at startup
     */
    MeasurementIndex _dl1Path;

    /**
     * This attribute contains the BPM measurement index though which the beam
     * is passing on DL2.
     *
     * Possible values are:
     *    BPMS_BSY0_52 or M7
     *    BPMS_LTU1_250 or M8
     *    NO_MEASUREMENT -> at startup
     */
    MeasurementIndex _dl2Path;

    /**
     * This attribute holds the result of element-by-element multiplication of
     * the F and G matrices. _fgMatrix is initialized by the configure()
     * method.
     *
     * On every feedback cycle some rows and columns of _fgMatrix need to
     * be zeroed out depending of the SxxUSEDBYLOOP PVs.
     */
    Matrix _fgMatrix;

    /**
     * This is the _fgMatrix with rows/cols zeroed out if there are
     * states not in use.
     */
    Matrix _usedFgMatrix;

    /** Holds latest calculated energies/currents used by updateActuators() */
    Vector _latestMeasurements;

    /**
     * Holds latest calculated energies/currents sum used by
     * updateActuators()
     */
    Vector _latestMeasurementsSum;

    /** Holds proportional changes in actuator values */
    Vector _pDelta;

    /** Holds integral changes in actuator values */
    Vector _iDelta;

    /** Holds final changes in actuator values */
    Vector _delta;

    /** Previous feedback mode of operation  */
    bool _previousMode;

    /** Time statistics for checking measurements */
    TimeAverage _checkMeasurementsStats;

    /** Time statistics for selecting states */
    TimeAverage _selectStatesStats;

    /** Time statistics for configuring matrix */
    TimeAverage _configureMatrixStats;

    /** Time statistics for calculationg energies and currents */
    TimeAverage _calculateEnergyCurrentStats;

    /** Time statistics for updating states */
    TimeAverage _updateStatesStats;

    /** Time statistics for updating actuators */
    TimeAverage _updateActuatorsStats;
    TimeAverage _updateActuatorsDetailStats;

    bool _stateUsed[6];
};

FF_NAMESPACE_END

#endif	/* _LONGITUDINAL_H */


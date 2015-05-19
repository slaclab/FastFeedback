/* 
 * File:   BunchCharge.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 3:59 PM
 */

#ifndef _BUNCHCHARGE_H
#define	_BUNCHCHARGE_H

#include "Defs.h"
#include "LoopConfiguration.h"
#include "Algorithm.h"
#include "MeasurementAverage.h"
#include "CaChannel.h"

FF_NAMESPACE_START

const double REALLY_LOW_TMIT = 1e6;

/**
 * The BunchCharge algorithm behaves a little different than the other feedbacks.
 * It is constantly reading the single measurement (BPMS:IN20:221:TMIT) and
 * once it accumulates 10 good measurements, a new actuator value is calculated
 * and written out (if possible).
 *
 * @author L.Piccoli
 */
class BunchCharge : public Algorithm {
public:
  BunchCharge();
  virtual ~BunchCharge();
  
  virtual int configure(LoopConfiguration *configuration,
			MeasurementSet *measurements, ActuatorSet *actuators,
			StateSet *states) throw (Exception);
  
  virtual int calculate(LoopConfiguration &configuration,
			MeasurementSet &measurements,
			ActuatorSet &actuators,
			StateSet &states) throw (Exception);

 protected:
    virtual int getMeasurements();

 private:
    /** Running average of M1 measurements */
    MeasurementAverage _m1Average;

    /** Pointer to the M1 measurement */
    MeasurementDevice *_m1;

    /** Pointer to the S1 state */
    StateDevice *_s1;

    /** Pointer to the A1 actuator */
    ActuatorDevice *_a1;

    /** Count the number of good TMITs collected */
    unsigned _goodMeasurementsCount;

    /**
     * Count the number of consecutive measurements with TMIT lower than
     * TMITLOW and higher than 1e6 -> this indicates low charge and 
     * operators may need to lower the TMITLOW value.
     */
    unsigned _lowTmitMeasurementsCount;
    
    /**
     * Count the number of times actuator setting was skipped because
     * waveplates were not done moving
     */
    unsigned _actuatorSkipCount;

    /** Count how many measurements had mismatching PULSEID in sequence */
    unsigned _mismatchPulseIdMeasurements;

    /**
     * ChannelAccess connection to laser waveplate status
     * PV -> WPLT:LR20:116:WP2_ANGLEXPS_STATUS
     */
    CaChannel *_laserWaveplateStatusChannel;

    /**
     * ChannelAccess connection to laser waveplate (1 if done moving),
     * PV -> WPLT:LR20:116:WP2_ANGLE.DMOV
     */
    CaChannel *_laserWaveplateDoneStatusChannel;

    /**
     * ChannelAccess connection to camera waveplate status
     * PV -> WPLT:IN20:181:VCC_ANGLEXPS_STATUS
     */
    CaChannel *_cameraWaveplateStatusChannel;

    /**
     * ChannelAccess connection to camera waveplate (1 if done moving),
     * PV -> WPLT:IN20:181:VCC_ANGLE.DMOV
     */
    CaChannel *_cameraWaveplateDoneStatusChannel;

    /**
     * Read back of the laser power (IOC:IN20:LS11:LASER_PWR_READBACK).
     * The actuator device (IOC:IN20:LS11:PCTRL) does not
     * contain the actual reading of the laser power. The feedback
     * must check with this PV wheather the actuator has a
     * meaningful value.
     */
    CaChannel *_laserPowerReadbackChannel;

    static const unsigned MAX_GOOD_MEASUREMENTS = 10;
    static const double ACTUATOR_DEADBAND = 0.005;
    static const unsigned MAX_ACTUATOR_SKIP = 10;
    static const unsigned MAX_MISMATCHED_PULSEID_MEASUREMENTS = 20;

    double calculateActuator();
    void updateActuator(double newValue);
    void updateState(double newValue);
    bool checkWaveplate() throw (Exception);
    bool doneMoving();
};

FF_NAMESPACE_END

#endif	/* _BUNCHCHARGE_H */


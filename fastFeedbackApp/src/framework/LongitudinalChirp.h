/* 
 * File:   LongitudinalChirp.h
 * Author: lpiccoli
 *
 * Created on March 17, 2011, 9:40
 */

#ifndef _LONGITUDINALCHIRP_H
#define	_LONGITUDINALCHIRP_H

#include "Longitudinal.h"

FF_NAMESPACE_START

/**
 * Implementation of a slightly different Longitudinal algorithm.
 * Instead of Energy and Current this algorithm calculates 
 * Energy and Chirp.
 *
 * @author L.Piccoli
 */
class LongitudinalChirp : public Longitudinal {
 public:
  LongitudinalChirp(std::string algorithmName = "LongitudinalChirp");
  virtual ~LongitudinalChirp();

  virtual int calculate() throw (Exception);

  virtual int configure(LoopConfiguration *configuration,
			MeasurementSet *measurements, ActuatorSet *actuators,
			StateSet *states) throw (Exception);
  virtual int reset();

 protected:
  enum {
    DL1,
    BC1_ENERGY,
    BC1_CHIRP,
    BC2_ENERGY,
    BC2_CHIRP,
    DL2,
    BC1_AMPLITUDE = BC1_ENERGY,
    BC1_PHASE = BC1_CHIRP,
    BC2_AMPLITUDE = BC2_ENERGY,
    BC2_PHASE = BC2_CHIRP,
  };

  int calculateInitialStates();
  int checkChirpControl();
  virtual int checkMeasurementStatus();
  virtual int updateActuators() throw (Exception);
  virtual int selectStates() throw (Exception);
  int calculateAmplitudeAndPhase();
  void getInitialValues();
  void resetActuators();
  bool isChirpValid(double chirp);
  Vector _previousActuator;
  Vector _actuatorValue;
  
  double _previousBc2Energy;
  double _previousBc2Chirp;

  bool _previousBc2FbckPv;

  // Value of _chirpControl for the current iteration
  bool _chirpControl;

  // State device S4 (BC2 Energy) used to define the CHIRPSTATE PV value
  StateDevice *_bc2EnergyState;

  // State device S5 (BC2 Bunch) used to define the CHIRPSTATE PV value
  StateDevice *_bc2BunchState;

  // Actuator device A4 (BC2 Amplitude) used to define the control FBCK PV value
  ActuatorDevice *_bc2Amplitude;

  // Actuator device A5 (BC2 Phase) used to define the control FBCK PV value
  ActuatorDevice *_bc2Phase;

  epicsTimeStamp _chirpOnTimestamp;

  // Number of cycles waiting for the CHIRPREADY PV to be set by the sequencer
  int _sequencerDelayCounter;

  // Signals that chirp has just been turned ON or OFF
  bool _chirpToggle;
};


FF_NAMESPACE_END

#endif	/* _LONGITUDINALCHIRP_H */


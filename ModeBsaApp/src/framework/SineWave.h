/* 
 * File:   SineWave.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 3:59 PM
 */

#ifndef _SINEWAVE_H
#define	_SINEWAVE_H

#include "Defs.h"
#include "LoopConfiguration.h"
#include "Algorithm.h"

class SineWaveTest;

FF_NAMESPACE_START

/**
 * Generates a sine wave from a counter or from the measurements
 * read in.
 *
 * Modes of operation are:
 * - Counter mode: actuator = sin(counter)
 * - 1 act, 1 meas, 1 state: actuator = sin(meas); state = actuator
 * - 1 act, 2 meas, 1 state: actuator = sin(meas1 + meas2); state = actuator
 * - 2 act, 2 meas, 2 state: s1,a1=sin(m1) and s2,a2=sin(m2)
 *
 * @author L.Piccoli
 */
class SineWave : public Algorithm {
public:

    enum SineWaveMode {
        COUNTER_MODE = 0,
        ONE_MEAS_ONE_ACT,
        TWO_MEAS_ONE_ACT,
        TWO_MEAS_TWO_ACT,
        N_MEAS_N_ACT,
        NOOP
    };

    SineWave(SineWaveMode mode = COUNTER_MODE) : Algorithm("SineWave"),
    _counter(0), _mode(mode) {
        switch(_mode) {
            case NOOP:
                _name = "Noop";
                break;
            default:
                break;
        }
    };

    virtual ~SineWave() {
    };

    virtual int calculate(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);

    friend class SineWaveTest;

private:

    int calculateCounter(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);
    int calculateMode1(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);
    int calculateMode2(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);
    int calculateMode3(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);
    int calculateMode4(LoopConfiguration &configuration,
            MeasurementSet &measurements,
            ActuatorSet &actuators,
            StateSet &states);


    /** Internal counter to generate the sine */
    int _counter;

    /** Operating mode */
    SineWaveMode _mode;
};

FF_NAMESPACE_END

#endif	/* _SINEWAVE_H */


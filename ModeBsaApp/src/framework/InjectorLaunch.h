/* 
 * File:   InjectorLaunch.h
 * Author: lpiccoli
 *
 * Created on July 16, 2010, 11:09 AM
 */

#ifndef _INJECTORLAUNCH_H
#define	_INJECTORLAUNCH_H

#include "TrajectoryFitGeneral.h"

FF_NAMESPACE_START

/**
 * Implementation of TrajectoryFit using a fixed set of measurements/states/actuators
 *
 * Measurement Devices:
 * M1 - BPMS:IN20:525:X (BPM9)
 * M2 - BPMS:IN20:581:X (BPM10)
 * M3 - BPMS:IN20:631:X (BPM11)
 * M4 - BPMS:IN20:651:X (BPM12)
 * M5 - BPMS:IN20:771:X (BPM14)
 * M6 - BPMS:IN20:781:X (BPM15)
 * M7 - BPMS:IN20:525:Y (BPM9)
 * M8 - BPMS:IN20:581:Y (BPM10)
 * M9 - BPMS:IN20:631:Y (BPM11)
 * M10 - BPMS:IN20:651:Y (BPM12)
 * M11 - BPMS:IN20:771:Y (BPM14)
 * M12 - BPMS:IN20:781:Y (BPM15)
 *
 * This algorithm switches between using BPM9-BPM15 or BPM9-BPM12
 * based on TMIT values.
 *
 * M5USED, M6USED, M11USED and M12USED *must* be set to 1, otherwise
 * the algorithm will fail during configuration.
 * 
 * @author L.Piccoli
 */
class InjectorLaunch : public TrajectoryFitGeneral {
public:
    InjectorLaunch(std::string algorithmName = "InjectorLaunch");
    virtual ~InjectorLaunch();

    virtual int calculate();

    virtual int configure(LoopConfiguration *configuration,
            MeasurementSet *measurements, ActuatorSet *actuators,
            StateSet *states);
    virtual void show();

protected:
    /**
     * Definition of which configuration is used by the InjectorLaunch:
     * None: has not been configured yet
     * Normal: beam is going through BPM14/BPM15
     * Spectrometer: beam is not going through BPM14/BPM15
     */
    enum BeamConfiguration {
        None = 0,
        Normal = 100,
        Spectrometer = 200
    };

    /** Holds the current beam configuration */
    BeamConfiguration _beamConfiguration;

    /**
     * M5 measurement device, which is the device BPMS:IN20:771. Used to find
     * if InjectorLaunch should be in Normal configuration.
     */
    MeasurementDevice *_m5;

    /**
     * M6 measurement device, which is the device BPMS:IN20:781. Used to define
     * the F Matrix and reference orbit vector used in Normal mode.
     */
    MeasurementDevice *_m6;

    /** Original full F Matrix, copied from the LoopConfiguration */
    Matrix _fMatrix;

    /**
     * F Matrix for Normal mode containing selected rows from the original
     * F Matrix.
     */
    Matrix _fMatrixNormal;

    /**
     * F Matrix for Spectrometer mode containing selected rows from the
     * original F Matrix.
     */
    Matrix _fMatrixSpectrometer;

    /** Original set of MeasurementDevices, copied from the LoopConfiguration */
    MeasurementSet _allMeasurements;

    /**
     * MeasurementSet for the Normal configuration. This set contains only
     * the devices used by the Normal configuration (i.e. BPM14/BPM15 are included)
     */
    MeasurementSet _measurementsNormal;

    /**
     * MeasurementSet for the Spectrometer configuration. This set contains
     * only devices used by the Spectrometer configuration (i.e. BPM14/BPM15 are
     * not in this set).
     */
    MeasurementSet _measurementsSpectrometer;

    /** Original Reference Orbit vector */
    std::vector<double> _refOrbit;

    /** Reference Orbit vector for Normal mode (all BPMS included) */
    std::vector<double> _refOrbitNormal;

    /**
     * Reference Orbit vector for Spectrometer mode, skipping values for
     * BPM14 and BPM15.
     */
    std::vector<double> _refOrbitSpectrometer;

    virtual int checkConfig();

    void saveNormalConfiguration();
    void saveSpectrometerConfiguration();
    void saveFMatrix(Matrix &fMatrix, int numDevices);
    void saveReferenceOrbit(std::vector<double> &refOrbit, int numDevices);

    int configureNormal();
    int configureSpectrometer();

private:
    /** Number of Measurement Devices when the Injector Launch is in Normal mode */
    static const int DEVICES_NORMAL_MODE = 6;

    /** Number of Measurement Devices when the Injector Launch is in Spectrometer mode */
    static const int DEVICES_SPECTROMETER_MODE = 5;
};

FF_NAMESPACE_END

#endif


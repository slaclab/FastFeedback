/* 
 * File:   LoopConfiguration.h
 * Author: lpiccoli
 *
 * Created on June 7, 2010, 11:34 AM
 */

#ifndef _LOOPCONFIGURATION_H
#define	_LOOPCONFIGURATION_H

#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>
#include <dbScan.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <epicsMutex.h>

#include "Defs.h"
#include "Pattern.h"
#include "PatternMask.h"
#include "MeasurementDevice.h"
#include "ActuatorDevice.h"
#include "StateDevice.h"
#include "SetpointDevice.h"
#include "CommunicationChannel.h"
#include "MeasurementCollector.h"
#include "DeviceView.h"
#include "PvData.h"
#include "Log.h"
#include "EventReceiver.h"
#include "FcomChannelStates.h"

class LoopConfigurationTest;
class LoopTest;

FF_NAMESPACE_START

// Declare class Algorithm here because its header file includes this file!
class Algorithm;
// Declare this enum here since Longitudinal.h includes this file.
enum EnergyLocation : int; 

typedef std::set<MeasurementDevice *, DeviceCompare> MeasurementSet;
typedef std::map<PatternMask, MeasurementSet *> MeasurementMap;

typedef std::set<ActuatorDevice *, DeviceCompare> ActuatorSet;
typedef std::map<PatternMask, ActuatorSet *> ActuatorMap;

typedef std::set<SetpointDevice *, DeviceCompare> SetpointSet;
typedef std::map<PatternMask, SetpointSet *> SetpointMap;

typedef std::set<StateDevice *, DeviceCompare> StateSet;
typedef std::map<PatternMask, StateSet *> StateMap;

typedef std::map<PatternMask, Algorithm *> AlgorithmMap;

//typedef boost::numeric::ublas::matrix<double> Matrix;
typedef boost::numeric::ublas::matrix<double, boost::numeric::ublas::column_major, boost::numeric::ublas::bounded_array<double,400> > Matrix;


/**
 * The LoopConfiguration contains configuration values associated with a
 * feedback loop. Most of the configuration is provided by PVs listed below.
 *
 * Once all configuration is read, the sets of Measurements, Actuators, States
 * and State Set Points should be turned into Devices and composed into
 * DeviceSets as needed.
 *
 * All DeviceSets are passed to the Loop, while Measurements are also added
 * to the MeasurementCollector.
 *
 * List of configuration PVs handled by the LoopConfiguration:
 *
 * - yxxDEVNAME: The actual Actuator/Measurement/State PV name is set in this
 *               PV. The name is passed to a Device object at startup time. The
 *               Device will be responsible for reading/writing to the PV either
 *               through Channel Access or FCOM. (string)
 * - yxxUSED: Defines whether an Actuator/Measurement/State is in use by the
 *            Loop. (binary)
 * - yxxCAMODE: Defines whether an Actuator/Measurement is accessed using
 *              Channel Access or FCOM (States are written only through FCOM).
 *              (short)
 *
 * Measurement specific PVs:
 *
 * - yxxDSPR: Dispersion of the measurement. This PV should be monitored.
 *            (double)
 * - AVGCNT: Number of values used to average a measurement. (double)
 * - USEAVG: Defines if the average measurement should be used instead of the
 *           latest available measurement. (double)
 *
 * State Setpoint specific PVs:
 *
 * - yxxOFFSET[1..4]: Offsets for state setpoints. Offsets are assigned to
 *                    timeslots/patterns. For each offset (time slot) one
 *                    StateDevice is created. (double)
 *
 * where:
 *
 * - xx = 1..N (index)
 * - y = device type [A,M,S] (type)
 * - z = suffix (for setpoints only) (suffix)
 *
 * Loop configuration PVs:
 *
 * - PGAIN: Proportional gain. (double)
 * - IGAIN: Integral gain. (double)
 * - IGAINERRNUM: Integral gain number of errors in sum. (double)
 * - FMATRIXFUNC: Feedback matrix function name. (string)
 * - GMATRIXFUNC: Gain matrix function name. (string)
 * - INITFUNC: Init function name. (string)
 * - ITERATEFUNC: Iterate function name. (string)
 * - NAME: Feedback name. (string)
 * - FMATRIX: Feedback matrix. (double[])
 * - GMATRIX: Gain matrix. (double[])
 * - POI[1..4]: Pattern of interest. (ushort[21])
 * - NETWORK: Use of Channel Access or FCOM for all Actuators/Measurements.
 *            (binary)
 * - POINUM: Number of patterns of interest, indicates how many of the POI[1..4]
 *           are in use. This is set by the HLA. (double)
 * - MODE: Controls whether the Loop is enabled or not. The Loop is still
 *         processing, but actuators are not being set. The control for
 *         completely stopping and starting a Loop is managed by the ExecThread
 *         though the STATE PV. (binary)
 * - NUMPOI: Number of the POI for the current pulse. This value is updated when
 *           a PATTERN_EVENT is received by the LoopThread. (double)
 * - NUMPOIHIST: History of POI number at the loop rate. This PV together with
 *               NUMPOI should be represented by a Device and a DeviceView. This
 *               allows the record processing to be started by the
 *               EpicsSenderThread. (double[])
 *
 * When creating Devices for Measurements, Actuators and States the Loop must
 * create the proper DeviceViews and add them to the EpicsSenderThread.
 *
 * Feedbacks with multiple patterns of interest require multiple sets of
 * Measurements, Actuators and States. Multiple States should share the
 * same setpoint.
 * 
 * @author L.Piccoli
 */
class LoopConfiguration {
public:

    LoopConfiguration(std::string slotName = "ERROR, MUST SPECIFY NAME");

    ~LoopConfiguration() {
    };

    int configure();
    int initialize();

    void showDevices();
    void showMatrices();
    void showReferenceOrbit();
    void showActuatorEnergy();
    void showDispersion();
    void showEref();
//ababbitt - need to insert the template

    template<class Map, class MapIterator, class Set, class SetIterator, class Device>
    Device *getDevice(Map &map, std::string deviceName, PatternMask patternMask);

    /**
     * Template method that creates DeviceViews for the specified type of Device
     * (Measurament, Actuator, State or Setpoint).
     *
     * This method is invoked by the Device Support code when connecting PVs to
     * the feedback devices.
     *
     * @param map map of devices (MeasurementMap, ActuatorMap or StateMap)
     * @param deviceName name of the device
     * @param patternIndex specify whether the DeviceView monitors data for the
     * specified pattern (if patternIndex is between 1 and 4), or for all the
     * patterns for the given deviceName (if patternIndex is 0).
     * @param deviceIndex each PV type (Measurement, Actuator, State) has an
     * index associated (e.g. M1, A3, S2). The deviceIndex is the number extracted
     * from the PV name by the Device Support code.
     * @return 0 on success, -1 if DeviceView could not be created
     * @author L.Piccoli
     */
    template<class Map, class MapIterator, class Set, class SetIterator, class Device >
    DeviceView * createDeviceView(Map &map, std::string deviceName,
    int patternIndex, int deviceIndex) {
      char type = deviceName.c_str()[0];
        DeviceView *deviceView = new DeviceView(1000, deviceIndex,
                            type, "", _slotName, patternIndex);

        // Get the MeasurementDevice(s) from the LoopConfiguration
        // If patternIndex is 0 the all patterns are added to the
        // DeviceView, otherwise only the MeasurementDevice for the
        // specified pattern is added.
        if (patternIndex == 0) {
            for (int i = 0; i <= (int) _patternMasks.size(); ++i) {
                Device *device = getDevice<Map, MapIterator, Set, SetIterator, Device >
                        (map, deviceName, _patternMasks[i]);
                if (device != NULL) {
                    deviceView->add(device);
                }
            }
        } else {
            if (patternIndex > (int) _patternMasks.size() || patternIndex < 1) {
          if (deviceView != NULL) {
                delete deviceView;
          }
          return NULL;

            }
            Device *device = getDevice<Map, MapIterator, Set, SetIterator, Device >
                    (map, deviceName, _patternMasks[patternIndex - 1]);
            if (device != NULL) {
                deviceView->add(device);
            }
        }

        return deviceView;
    }

    int peek(std::string deviceName, PatternMask patternMask,
	     DataPoint &dataPoint);

    int peek(std::string deviceName, PatternMask patternMask,
	     DataPoint &dataPoint, int pos);

    /**
     * Get the appropriate vernier or Eref values based on destination and location in the LINAC. 
     * enum EnergyLocation is defined in Longitudinal.h 
     */
    std::pair<double, double> getEnergy(int _patternMask, EnergyLocation energyLocation);

    /**
     * Defines whether the Loop is in compute only mode (i.e. all actuator
     * and state values are computed, but not set) or values are send out
     * to actuators.
     */
    bool _mode;

    /**
     * Defines whether the Loop is ON or OFF. This is controlled by the LoopThread
     * when it receives a RECONFIGURE_EVENT (generated by the FBCK:FBxx:xxxx:STATE
     */
    bool _enabled;

    /**
     * List of PatternMasks for this loop, which are configured from the _poi[1..4]
     * waveform attributes when the loop is reconfigured.
     */
    std::vector<PatternMask> _patternMasks;

    /** PV for pattern of interest 1 waveform */
    PvDataWaveform<unsigned short> _poi1Pv;

    /** PV for pattern of interest 2 waveform */
    PvDataWaveform<unsigned short> _poi2Pv;

    /** PV for pattern of interest 3 waveform */
    PvDataWaveform<unsigned short> _poi3Pv;

    /** PV for pattern of interest 4 waveform */
    PvDataWaveform<unsigned short> _poi4Pv;

    /** Map of MeasurementDevice sets, there is one set per pattern */
    MeasurementMap _measurements;

    /** Map of ActuatorDevice sets, there is one set per pattern */
    ActuatorMap _actuators;

    /** Map of Setpoints, which actually are MeasurementDevices */
    SetpointMap _setpoints; // TODO: This map may not be needed

    /** Map of StateDevice sets, there is one set per pattern */
    StateMap _states;

    /**
     * Instances of the algorithm selected for the feedback, there
     * is separated algorithm objects per pattern because there may
     * be pattern specific configuration.
     */
    AlgorithmMap _algorithms;

    /** F Matrix */
    Matrix _fMatrix;

    /** G Matrix */
    Matrix _gMatrix;

    /** Reference Orbit */
    std::vector<double> _refOrbit;

    /** Actuator Energy */
    std::vector<double> _actEnergy;

    /** Measurement Dispersion (Longitudinal only) */
    std::vector<double> _dispersions;

    /** Device support scanlist -> there is one list per loop */
    IOSCANPVT _scanlist;

    /**
     * Loop device name -> third field in the PV name (not the name set by the
     * user)
     */
    std::string _name;
    std::string _slotName;

    /** Index used to access PVDATA_MUTEX array, which is based on the _slotName */
    int _loopIndex;

    /**
     * Defines the user given name for this loop.
     * This attribute maps into the $(LOOP):NAME PV.
     */
    PvData<std::string> _loopNamePv;

    /**
     * Defines the algorithm used for this loop.
     * This attribute maps to the $(LOOP):ITERATEFUNC PV
     */
    PvData<std::string> _algorithmNamePv;

    /**
     * Defines whether the Loop is configured or not. This attribute in 
     * conjunction with LoopThread::_enabled ($(LOOP):STATE PV) defines when
     * the Loop is reconfigured (e.g. F Matrix changed). The reconfiguration
     * is triggered by the Java Config App.
     */
    bool _configured;

    /**
     * Defines a std::vector<double> that contains the double waveform
     * set by the Java Config App.
     *
     * This attribute maps to the $(LOOP):FMATRIX PV.
     */
    PvDataWaveform<double> _fMatrixPv;

    /**
     * Defines a std::vector<double> that contains the double waveform
     * set by the Java Config App.
     *
     * This attribute maps to the $(LOOP):GMATRIX PV.
     */
    PvDataWaveform<double> _gMatrixPv;

    /**
     * Defines a std::vector<double> that contains the dispersion of all
     * measurements - the dispersion is only used by the Longitudital
     * feedback.
     *
     * This attribute maps to the $(LOOP):MEASDSPR PV.
     */
    PvDataWaveform<double> _measDsprPv;

    /**
     * Defines a std::vector<double> that contains the reference orbit for
     * the MeasurementDevices.
     *
     * This attribute maps to the $(LOOP):REFORBIT PV.
     */
    PvDataWaveform<double> _refOrbitPv;

    /**
     * Defines a std::vector<double> that contains the energies for the Actuator
     * Devices currently selected.
     *
     * This attribute maps to the $(LOOP):ACTENERGY PV.
     */
    PvDataWaveform<double> _actEnergyPv;

    /**
     * This attribute holds the number of actuators that can be used
     * by the loop.
     *
     * This attribute maps to the $(LOOP):ACTNUM PV.
     */
    PvData<long> _actNumPv;

    /**
     * This attribute holds the number of measurements that can be used
     * by the loop.
     *
     * This attribute maps to the $(LOOP):MEASNUM PV.
     */
    PvData<long> _measNumPv;

    /**
     * This attribute holds the number of states that can be used
     * by the loop.
     *
     * This attribute maps to the $(LOOP):STATENUM PV.
     */
    PvData<long> _stateNumPv;

    /**
     * Contains the count of how many Actuators are in use by the Loop.
     * This number is set by the IOC based on the $(LOOP):A*USED PVs.
     *
     * This attribute is mapped to the $(LOOP):ACTUSEDNUM PV.
     */
    PvData<long> _actuatorsUsedPv;

    /**
     * Contains the count of how many Measurements are in use by the Loop.
     * This number is set by the IOC based on the $(LOOP):M*USED PVs.
     *
     * This attribute is mapped to the $(LOOP):MEASUSEDNUM PV.
     */
    PvData<long> _measurementsUsedPv;

    /**
     * Contains the count of how many States are in use by the Loop.
     * This number is set by the IOC based on the $(LOOP):S*USED PVs.
     *
     * This attribute is mapped to the $(LOOP):STATEUSEDNUM PV.
     */
    PvData<long> _statesUsedPv;

    /**
     * Contains the number of patterns used by the Loop. Usually a Loop
     * processes one pattern, but for 120Hz operations at least two patterns
     * must be used.
     *
     * This attribute is mapped to the $(LOOP):TOTALPOI PV.
     */
    PvData<long> _totalPoiPv;

    /**
     * This attribute is set by the Java config app and tells the loop that 
     * a new configuration is available and should be used when the $(LOOP):STATE
     * PV transitions between OFF and ON states.
     *
     * This attribute is mapped to the $(LOOP):INSTALLED PV.
     */
    PvData<bool> _installedPv;

    /**
     * Proportional gain for the Loop.
     *
     * This attribute is mapped to the $(LOOP):PGAIN PV.
     */
    PvData<double> _pGainPv;

    /**
     * Integral gain for the Loop.
     *
     * This attribute is mapped to the $(LOOP):IGAIN PV.
     */
    PvData<double> _iGainPv;

    /**
     * This attribute is mapped to the $(LOOP):PULSEID PV.
     * It is set by the LoopThread when a new Pattern is received.
     */
    PvData<long> _pulseIdPv;

    /**
     * Count the number of successfull feedback iterations.
     * This maps to the $(LOOP):COUNTER PV, which is monitored
     * by an external watchdog application.
     */
    PvData<long> _loopCounter;

    /**
     * String containing general status messages.
     *
     * TODO: Turn this attribute into a PV
     */
    std::ostringstream _statusMessage;
    PvData<std::string> _statusstrPv;

    PvData<std::string> _actuatorLastUpdatePv;

    /**
     * Each loop has its own Logger.
     */
    Log _logger;

    /**
     * Pointer to the EventReceiver (used by the LoopThread)
     */
    EventReceiver *_eventReceiver;

    /**
     * Defines whether the Loop should check the TMIT values on each
     * cycle or not. Only the Longitudinal feedback skip checking at
     * the Loop. Further checking happens on the Longitudinal::calculate()
     * method.
     */
    bool _skipTmitCheck;

    /**
     * This attribute is set to true only when measurements are *not*
     * read via ChannelAccess of Fcom, i.e. values are read from files
     * or counters.
     */
    bool _fakeBeam;

    /** Longitudinal: Control whether the BC2 Chirp is enabled or not */
    PvData<bool> _chirpControlPv;

    /**
     * Longitudinal: Contains the current chirp control value. It depends
     * on the S4USED (BC2 Energy) and S5USED (BC2 Bunch) states configuration
     */
    PvData<bool> _chirpStatePv;

    /** Longitudinal: User provided BC2 Chirp value */
    PvData<double> _bc2ChirpPv;

    /**
     * Defines if all devices (measurements/actuators/states) should be
     * selected by the Loop for processing. Certain algorithms need to 
     * read all measurement devices and then determine where beam is going.
     * This is the case for the Longitudinal algorithm.
     *
     * Algorithms whose used measurement devices are defined at configuration
     * time should have this PV set to 0/No/false.
     *
     * This attribute is mapped to the $(LOOP):SELECTALLDEVICES PV.
     */
    PvData<bool> _selectAllDevicesPv;

    /**
     * FcomChannel used to send out calculated states. There is only a single
     * CommunicationChannel for sending out states from a feedback loop.
     */
    FcomChannelStates *_statesFcomChannel;

    /**
     * Defines a std::vector<double> that contains the p11, p12, p33 and p34
     * polynomial coefficient matrices used by the Undulator Launch feedback.
     *
     * This attribute maps to the $(LOOP):POLYVALS PV.
     */
    PvDataWaveform<double> _polyvalsPv;

    /** Mutex to control access to configuration data */
    epicsMutex *_mutex;

    /**
     * Bool attribute to control if the Loop should skip actuators only
     * if the feedback is in COMPUTE mode. This was added to allow the
     * BunchCharge feedback to set the laser when the calculate() took
     * too long.
     */
    bool _skipOnlyCompute;

    /**
     * Counts how many cycles a status str message should be kept before
     * clearing it up.
     */
    int _clearStatusCounter;

    int _rate;

    /**
     * Used by the Longitudinal feedback to start acting after the CHIRPDES
     * is set by the sequencer (ChirpControl.st)
     */
    PvData<bool> _chirpReadyPv;


    /**
     * Controls whether calculated states are sent on FCOM, using the ID
     * FBCK:<IOC>:<LOOP>:STATES
     */
    PvData<bool> _bcastStatesPv;

    /** Helper function to output contents of the configuration to a stream */
    friend std::ostream & operator<<(std::ostream &os,
            LoopConfiguration * const loopConfig) {
        os << "Deprecated";
        return os;
    }


    friend class LoopConfigurationTest;
    friend class LoopTest;

    static const int MAX_PATTERNS = 4;
    static const int MAX_MEASUREMENTS = 40;
    static const int MAX_ACTUATORS = 10;
    static const int MAX_STATES = 10;
    static const int MAX_SETPOINTS = MAX_STATES;
    static const int DEVICE_BUFFER_SIZE = 100;
    
    // This is used by algorithms to reload the FMatrix
    int configureFMatrix();

    // This is used by algorithms to reload the GMatrix
    int configureGMatrix();

private:
    // Methods for initializing data strutures
    int initializePattern(PatternMask &patternMask);
    template<class DeviceType, class DeviceSet>
    int initializeDevices(std::string devicePrefix,
    int deviceCount, std::string deviceSuffix, int bufferSize,
    PatternMask patternMask, int patternIndex, DeviceSet *deviceSet);
    int assignSetpoints(StateSet *stateSet, SetpointSet *setpointSet);
    void setReferenceActuators(ActuatorSet *referenceSet, ActuatorSet *actuatorSet);

    // Methods that read configuration from PVs
    int configurePatterns();
    int configureAlgorithm();
    int configureDevices();
    int countUsedDevices();
    int reconfigurePattern(const PatternMask &previousPatternMask,
    const PatternMask &newPatternMask);

    template<class DeviceMap, class DeviceMapIterator, class DeviceType,
    class DeviceSet, class DeviceSetIterator>
    int reconfigureDevices(DeviceMap &map, const PatternMask &previousPatternMask,
    const PatternMask &newPatternMask);

    template <class DeviceSet, class DeviceSetIterator>
    int countUsedDevices(DeviceSet *deviceSet);
    int configureReferenceOrbit();
    int configureActuatorEnergy();
    int configureDispersions();

    int configureSetpoints();

    template<class Map, class MapIterator, class Set, class SetIterator, class Device>
    void showDeviceMap(Map map, bool showUsed = false);

    template<class Map, class MapIterator, class Set, class SetIterator, class Device>
    int peekDeviceMap(Map &map, std::string deviceName,
		      PatternMask patternMask, DataPoint &dataPoint);

    template<class Map, class MapIterator, class Set, class SetIterator, class Device>
    int peekDeviceMap(Map &map, std::string deviceName,
		      PatternMask patternMask, DataPoint &dataPoint, int pos);


    /**
     * Get beam destination from pattern index.
     * ATTN: If the pattern assignments change for HXR/SXR delivery this code will also have to change!
     * TODO: Investigate making this runtime configuratble.
     */

    enum class Destination {
        HXR,
        SXR,
    };

    Destination getPatternDestination(int patternIndex) {
       if (patternIndex == 0 || patternIndex == 3) // P1 or P4 aka DS0 or DS3
           return Destination::HXR;
       else                                        // P2 or P3 aka DS1 or DS2
           return Destination::SXR;
    } 

    /*
     * The following PVs are specific to a destination. Used for Longitudinal feedback.
     */

    /** FBCK:FBxx:LG01:DL1VERNIER_(SXR) */
    PvData<double> _dl1EnergyVernierPvHXR;
    PvData<double> _dl1EnergyVernierPvSXR;

    /** FBCK:FBxx:LG01:BC1VERNIER_(SXR) */
    PvData<double> _bc1EnergyVernierPvHXR;
    PvData<double> _bc1EnergyVernierPvSXR;

    /** FBCK:FBxx:LG01:BC2VERNIER_(SXR) */
    PvData<double> _bc2EnergyVernierPvHXR;
    PvData<double> _bc2EnergyVernierPvSXR;

    /** FBCK:FBxx:LG01:DL2VERNIER_(SXR) */
    PvData<double> _dl2EnergyVernierPvHXR;
    PvData<double> _dl2EnergyVernierPvSXR;

    /* SIOC:SYS0:FBxx:DL1_EREF_(SXR) PV via CA */
    PvData<double> _dl1ErefPvHXR;
    PvData<double> _dl1ErefPvSXR;

    /* SIOC:SYS0:FBxx:BC1_EREF_(SXR) PV via CA */
    PvData<double> _bc1ErefPvHXR;
    PvData<double> _bc1ErefPvSXR;

    /* SIOC:SYS0:FBxx:BC2_EREF_(SXR) PV via CA */
    PvData<double> _bc2ErefPvHXR;
    PvData<double> _bc2ErefPvSXR;

    /* SIOC:SYS0:FBxx:DL2_EREF_(SXR) PV via CA */
    PvData<double> _dl2ErefPvHXR;
    PvData<double> _dl2ErefPvSXR;

    /* SIOC:SYS0:FBxx:LEM_DL2ENLOLO_(SXR) PV via CA */
    PvData<double> _dl2EnLoloPvHXR;
    PvData<double> _dl2EnLoloPvSXR;

    /* SIOC:SYS0:FBxx:LEM_DL2ENHIHI_(SXR) PV via CA */
    PvData<double> _dl2EnHihiPvHXR;
    PvData<double> _dl2EnHihiPvSXR;

};

FF_NAMESPACE_END

#endif	/* _LOOPCONFIGURATION_H */


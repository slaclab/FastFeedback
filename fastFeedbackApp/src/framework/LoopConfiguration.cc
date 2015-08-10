/* 
 * File:   LoopConfiguration.cc
 * Author: lpiccoli
 * 
 * Created on June 7, 2010, 11:34 AM
 */

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "MeasurementCollector.h"
#include "LoopConfiguration.h"
#include "NullChannel.h"
#include "FileChannel.h"
#include "SineWave.h"
#include "TrajectoryFit.h"
#include "TrajectoryFitBy1.h"
#include "TrajectoryFitStatic.h"
#include "TrajectoryFitPoly.h"
#include "TrajectoryFitPinv.h"
#include "TrajectoryFitEact.h"
#include "InjectorLaunch.h"
#include "BunchCharge.h"
#include "PatternManager.h"
#include "CaChannel.h"
#include "Loop.h"
#include "PatternGenerator.h"
#include "ExecConfiguration.h"
#include "Longitudinal.h"
#include "LongitudinalChirp.h"

//#include <cadef.h>

USING_FF_NAMESPACE

LoopConfiguration::LoopConfiguration(std::string slotName) :
//_modePv(slotName + " MODE", false), // Set loop to Compute mode (no settings)
_mode(false),
_enabled(false),
_poi1Pv(slotName + " POI1"),
_poi2Pv(slotName + " POI2"),
_poi3Pv(slotName + " POI3"),
_poi4Pv(slotName + " POI4"),
_refOrbit(MAX_MEASUREMENTS),
_actEnergy(MAX_ACTUATORS),
_dispersions(MAX_MEASUREMENTS),
_slotName(slotName),
_loopIndex(10),
_loopNamePv(slotName + " NAME", "NONAME"),
_algorithmNamePv(slotName + " ALGORITHM", "NOOP"),
_configured(false),
_fMatrixPv(slotName + " FMATRIX"),
_gMatrixPv(slotName + " GMATRIX"),
_measDsprPv(slotName + " MEASDSPR"),
_refOrbitPv(slotName + " REFORBIT"),
_actEnergyPv(slotName + " ACTENERGY"),
_actNumPv(slotName + " ACTNUM", 0),
_measNumPv(slotName + " MEASNUM", 0),
_stateNumPv(slotName + " STATENUM", 0),
_actuatorsUsedPv(slotName + " ACTUSEDNUM", 0),
_measurementsUsedPv(slotName + " MEASUSEDNUM", 0),
_statesUsedPv(slotName + " STATEUSEDNUM", 0),
_totalPoiPv(slotName + " TOTALPOI", 0),
_installedPv(slotName + " INSTALLED", false),
_pGainPv(slotName + " PGAIN", 0.01),
_iGainPv(slotName + " IGAIN", 1),
_pulseIdPv(slotName + " PULSEID", 0),
_loopCounter(slotName + " COUNTER", 0),
_statusstrPv(slotName + " STATUSSTR"),
_actuatorLastUpdatePv(slotName + " ACTUPDATE"),
_eventReceiver(NULL),
_skipTmitCheck(false),
_fakeBeam(false),// --->>> RESTORE THIS WHEN BEAM IS BACK!!!!
//_fakeBeam(true), // FOR TESTS
_chirpControlPv(slotName + " CHIRPCTRL", false),
_chirpStatePv(slotName + " CHIRPSTATE", false),
_bc2ChirpPv(slotName + " CHIRPDES", 2800),
_selectAllDevicesPv(slotName + " SELECTALLDEVICES", false),
_statesFcomChannel(NULL),
_polyvalsPv(slotName + " POLYVALS"),
_skipOnlyCompute(false),
_clearStatusCounter(0),
_chirpReadyPv(slotName + " CHIRPREADY"),
_dl1EnergyVernierPv(slotName + " DL1VERNIER"),
_bc1EnergyVernierPv(slotName + " BC1VERNIER"),
_bc2EnergyVernierPv(slotName + " BC2VERNIER"),
_dl2EnergyVernierPv(slotName + " DL2VERNIER"),
_bcastStatesPv(slotName + " BCASTSTATES") {
    _statusMessage << "Not configured";
    _logger.setSlotName(_slotName);
    _logger.setStatusstrPv(&_statusstrPv);
//    _mutex = new epicsMutex();
    _actuatorLastUpdatePv.initScanList(); // This enable PV updates.
    _statusstrPv.initScanList(); // This enable PV updates.
    _pulseIdPv.initScanList(); // This enable PV updates.
    _loopIndex = 0;
}

/**
 * Initialize all data structures at startup time. The Loop is configured
 * once it is turned on (STATE=ON). By default Loops are started with STATE=OFF.
 *
 * All possible Measurement, State and Actuator devices are created in this
 * method for the MAX_PATTERNS handled by the system (currently 4).
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LoopConfiguration::initialize() {
    _configured = false;
    _installedPv = true;

    ActuatorSet *actuatorSetP1;

    // First need to create four fake PatternMask that will be used to initialize
    // all devices.
    for (int i = 0; i < MAX_PATTERNS; ++i) {
        PatternMask fakePatternMask;
        fakePatternMask._timeSlot = 10 + i;
        for (int j = 0; j < MAX_EVR_MODIFIER; ++j) {
            fakePatternMask._inclusionMask[j] = i + j;
            fakePatternMask._exclusionMask[j] = 0xffffffff;
        }
        initializePattern(fakePatternMask);

        // Create all possible measurement devices
        MeasurementMap::iterator mIt;
        mIt = _measurements.find(fakePatternMask);
        initializeDevices<MeasurementDevice, MeasurementSet >
                ("M", MAX_MEASUREMENTS, "", DEVICE_BUFFER_SIZE,
                fakePatternMask, i + 1, mIt->second);

        // Create all possible actuator devices
        ActuatorMap::iterator aIt;
        aIt = _actuators.find(fakePatternMask);
        initializeDevices<ActuatorDevice, ActuatorSet >
                ("A", MAX_ACTUATORS, "", DEVICE_BUFFER_SIZE,
                fakePatternMask, i + 1, aIt->second);

        // Set reference actuators if pattern is P2/P3/P4
        if (i == 0) {
            actuatorSetP1 = aIt->second;
        } else {
            setReferenceActuators(actuatorSetP1, aIt->second);
        }

        // Create all possible state devices
        StateMap::iterator sIt;
        sIt = _states.find(fakePatternMask);
        initializeDevices<StateDevice, StateSet >
                ("S", MAX_STATES, "", DEVICE_BUFFER_SIZE,
                fakePatternMask, i + 1, sIt->second);

        // Create all possible setpoint devices
        SetpointMap::iterator spIt;
        std::ostringstream desStream;
        desStream << "DES" << i + 1;
        spIt = _setpoints.find(fakePatternMask);
        initializeDevices<SetpointDevice, SetpointSet >
                ("S", MAX_SETPOINTS, desStream.str(), DEVICE_BUFFER_SIZE,
                fakePatternMask, i + 1, spIt->second);

        // Assign setpoints to the states (its a one-to-one mapping)
        if (assignSetpoints(sIt->second, spIt->second) != 0) {
	  _logger << Log::showtime << "ERROR: Failed to assign setpoints to states." << Log::flush;
	  return -1;
        }

        // Initialize algorithm to NOOP, i.e. no calculation done.
        SineWave *sineWave = new SineWave(SineWave::NOOP);
        _algorithms.insert(std::pair<PatternMask, Algorithm *>(fakePatternMask, sineWave));
    }

    return 0;
}

/**
 * Assign Setpoints to States. The must be a one-to-one mapping in the passed
 * sets.
 * 
 * @param stateSet set of StateDevices (ordered by name)
 * @param setpointSet set of SetpointDevices (ordered by name)
 * @return 0 on success, -1 if sets have different number of elements
 * @author L.Piccoli
 */
int LoopConfiguration::assignSetpoints(StateSet *stateSet,
        SetpointSet *setpointSet) {
    if (stateSet->size() != setpointSet->size()) {
        return -1;
    }

    StateSet::iterator sIt;
    SetpointSet::iterator spIt;
    for (sIt = stateSet->begin(), spIt = setpointSet->begin();
            sIt != stateSet->end(); ++sIt, ++spIt) {
        (*sIt)->setSetpointDevice(*spIt);
    }
    return 0;
}

/**
 * Create *all* devices (Measurements, Actuators, States and Setpoints) for
 * the Loop. These may or may not be used when the system is configured, but
 * need to be created in order to be available for future use.
 *
 * The communicationChannel of these devices is set to NULL. The correct
 * communicationChannel must be set when the Loop is configured (according
 * to the CAMODE PV).
 *
 * Measurement devices created here are not added to the Collector thread
 * list (through the MeasurementCollector singleton). This action must be
 * taken when the Loop is configured. Likewise, measurement devices must be
 * removed if they are not in use (M[1..40]USED PV).
 *
 * @author L.Piccoli
 */
template<class DeviceType, class DeviceSet>
int LoopConfiguration::initializeDevices(std::string devicePrefix,
int deviceCount, std::string deviceSuffix, int bufferSize, PatternMask patternMask,
int patternIndex, DeviceSet *deviceSet) {
    for (int i = 1; i <= deviceCount; ++i) {
        DeviceType *device;
        std::ostringstream deviceName;
        deviceName << devicePrefix << i << deviceSuffix;

        device = new DeviceType(_slotName, deviceName.str(), bufferSize,
                patternMask, patternIndex);
        device->setCommunicationChannel(NULL);
        device->setPatternMask(patternMask);

        deviceSet->insert(device);
    }

    return 0;
}

/**
 * Set the P1 reference actuators for actuators running on other patterns
 * (e.g. P2/P3/P4).
 *
 * @param referenceSet set of actuators for P1
 * @param actuatorSet set of actuators for P2, P3 or P4
 * @author L.Piccoli
 */
void LoopConfiguration::setReferenceActuators(ActuatorSet *referenceSet,
        ActuatorSet *actuatorSet) {
    ActuatorSet::iterator it;
    for (it = actuatorSet->begin(); it != actuatorSet->end(); ++it) {
        ActuatorDevice *actuator = *it;
        ActuatorSet::iterator refIt = referenceSet->begin();
        while (refIt != referenceSet->end()) {
            ActuatorDevice *reference = *refIt;
            if (reference->getName() == actuator->getName()) {
                actuator->setReferenceActuator(reference);
            }
            ++refIt;
        }
    }
}

/**
 * Create the DeviceSets for an initial pattern. This method is used at
 * startup only.
 *
 * @return 0
 * @author L.Piccoli
 */
int LoopConfiguration::initializePattern(PatternMask &patternMask) {
    _patternMasks.push_back(patternMask);

    // Create a MeasurementSet for this pattern mask
    MeasurementSet *measurementSet = new MeasurementSet;
    _measurements.insert(std::pair<PatternMask, MeasurementSet *>(patternMask,
            measurementSet));

    // Create an ActuatorSet for this pattern mask
    ActuatorSet *actuatorSet = new ActuatorSet;
    _actuators.insert(std::pair<PatternMask, ActuatorSet *>(patternMask,
            actuatorSet));

    // Create a StateSet for this pattern mask
    StateSet *stateSet = new StateSet;
    _states.insert(std::pair<PatternMask, StateSet *>(patternMask,
            stateSet));

    // Create a SetpointSet for this pattern mask
    SetpointSet *setpointSet = new SetpointSet;
    _setpoints.insert(std::pair<PatternMask, SetpointSet *>(patternMask,
            setpointSet));

    // Only a small check...
    MeasurementMap::iterator it = _measurements.find(patternMask);
    if (it == _measurements.end()) {
      _logger << Log::showtime << "ERROR: Failed initialize pattern mask "
	      << patternMask.toString().c_str() << Log::flush;
        return -1;
    }

    return 0;
}

/**
 * This method is invoked when a Loop is turned on (STATE=ON). The configuration
 * uses values from the PVs (through Device Support).
 * 
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LoopConfiguration::configure() {
    _logger.setLoopName(_loopNamePv.getValue());

    if (configurePatterns() != 0) {
      _logger << Log::showtime << "ERROR: Failed to configure patterns." << Log::flushpv;
      _logger << Log::showtime <<"ERROR: Failed to configure patterns." << Log::cout;
      return -1;
    }

    if (countUsedDevices() != 0) {
      _logger << Log::showtime <<"ERROR: Failed to count used devices." << Log::flushpv;
      _logger << Log::showtime <<"ERROR: Failed to count used devices." << Log::cout;
      return -1;
    }

    if (configureReferenceOrbit() < 0) {
      _logger << Log::showtime <<"ERROR: Failed to configure reference orbit." << Log::flushpv;
      _logger << Log::showtime <<"ERROR: Failed to configure reference orbit." << Log::cout;
      return -1;
    }

    if (configureActuatorEnergy() < 0) {
      _logger << Log::showtime <<"ERROR: Failed to configure actuator energy." << Log::flushpv;
      _logger << Log::showtime <<"ERROR: Failed to configure actuator energy." << Log::cout;
      return -1;
    }

    configureDispersions();

    if (configureFMatrix() != 0) {
      _logger << Log::showtime <<"ERROR: Failed to configure F Matrix." << Log::flushpv;
      _logger << Log::showtime <<"ERROR: Failed to configure F Matrix." << Log::cout;
        return -1;
    }

    if (configureGMatrix() != 0) {
      _logger << Log::showtime <<"ERROR: Failed to configure G Matrix." << Log::flushpv;
      _logger << Log::showtime <<"ERROR: Failed to configure G Matrix." << Log::cout;
        return -1;
    }

    try {
      if (configureAlgorithm() != 0) {
	_logger << Log::showtime <<"ERROR: Failed to configure algorithm." << Log::flushpv;
	_logger << Log::showtime <<"ERROR: Failed to configure algorithm." << Log::cout;
        return -1;
      }
    } catch (Exception &e) {
      _logger << Log::showtime <<"ERROR: Failed to configure algorithm - " << e.what() << Log::flushpv;
      _logger << Log::showtime <<"ERROR: Failed to configure algorithm! " << e.what() << Log::cout;
      return -1;
    }

    // Create FcomChannel for States
    // Only Longitudital and LTU need States on FCOM
//     long numPatterns = _totalPoiPv.getValue();

    if (_bcastStatesPv == 1) {
      try {
        std::string statesChannelName = "FBCK:";
        statesChannelName += getenv("LOCA_NAME");
        statesChannelName += ":";
        statesChannelName += _slotName;
        statesChannelName += ":STATES";
        _statesFcomChannel = new FcomChannelStates(CommunicationChannel::WRITE_ONLY,
						   statesChannelName);
	std::cout<< "Created _statesFcomChannel" << std::endl;
      } catch (Exception &e) {
	_logger << "ERROR: Failed to create FcomChannel for states ("
		<< e.what() << ")" << Log::cout;
        return -1;
      }
    }
      
    _logger << Log::showtime << "INFO: Loop configured successfully" << Log::flushpvnoalarm;

    return 0;
}

/**
 * Count the number of Actuators, States and Measurements used to define the
 * size of the F and G matrices.
 *
 * The number of used devices is retrieved from the set assigned to the first
 * pattern of interest in the device map. The *USED PV is mapped to all
 * device.
 *
 * @return 0
 * @author L.Piccoli
 */
int LoopConfiguration::countUsedDevices() {
    _measurementsUsedPv =
            countUsedDevices<MeasurementSet, MeasurementSet::iterator >
            (_measurements.begin()->second);

    _actuatorsUsedPv =
            countUsedDevices<ActuatorSet, ActuatorSet::iterator >
            (_actuators.begin()->second);

    _statesUsedPv =
            countUsedDevices<StateSet, StateSet::iterator >
            (_states.begin()->second);

    if (_algorithmNamePv != LONGITUDINAL_ALGORITHM && 
	_algorithmNamePv != LONGITUDINAL_CHIRP_ALGORITHM) {
      _logger << Log::showtime << "INFO: Loop configured with " << _measurementsUsedPv.getValue()
	      << " measurements, " << _actuatorsUsedPv.getValue()
	      << " actuators, and " << _statesUsedPv.getValue() << " states."
	      << Log::flush;
      
      if (_measurementsUsedPv.getValue() == 0) {
	_logger << Log::showtime << "No sensors defined" << Log::flushpv;
	return -1;
      }
      if (_actuatorsUsedPv.getValue() == 0) {
	_logger << Log::showtime << "No actuators defined" << Log::flushpv;
	return -1;
      }
      if (_statesUsedPv.getValue() == 0) {
	_logger << Log::showtime << "No states defined" << Log::flushpv;
	return -1;
      }
    }

    return 0;
}

/**
 * Count the number of devices with the *USED PV set to true. The *USED PV
 * is mapped to the Device::_used attribute, which is accessible through the
 * Device::isUsed() method.
 *
 * @param deviceSet pointer to a std::set containing the devices
 * @return number of devices used for the given set.
 * @author L.Piccoli
 */
template <class DeviceSet, class DeviceSetIterator>
int LoopConfiguration::countUsedDevices(DeviceSet *deviceSet) {
    int used = 0;
    for (DeviceSetIterator it = deviceSet->begin();
            it != deviceSet->end(); ++it) {
        if ((*it)->isUsed()) {
            used++;
        }
    }
    return used;
}

/**
 * The loop receives a vector of reference orbits throught the REFORBIT PV,
 * which is accessible throught the attribute _refOrbitPv.
 *
 * When the loop is (re)configured the user specifies which MeasurementDevices
 * are used (MxxUSED PV). Based on this value the LoopConfiguration selects
 * only those selected reference orbits.
 *
 * Selected references are saved in the _refOrbit attribute (std::vector<double>)
 * and are used by the selected algorithm. The order of the references is the
 * same as the selected MeasurementDevices.
 *
 * @return number of selected reference orbits.
 * @author L.Piccoli
 */
int LoopConfiguration::configureReferenceOrbit() {
    int used = 0;
    _refOrbit.clear();

    std::vector<double>::iterator refIt = _refOrbitPv.getValueAddress()->begin();
    MeasurementSet *measurementsFirstPattern = _measurements.begin()->second;
    for (MeasurementSet::iterator it = measurementsFirstPattern->begin();
            it != measurementsFirstPattern->end(); ++it, ++refIt) {
        if ((*it)->isUsed()) {
            double refOrbit = *refIt;
            _refOrbit.push_back(refOrbit);
            used++;
        }
    }
    return used;
}

/**
 * The loop receives a vector of actuator energy throught the ACTENERGY PV,
 * which is accessible throught the attribute _actEnergyPv.
 *
 * When the loop is (re)configured the user specifies which ActuatorDevices
 * are used (AxxUSED PV). Based on this value the LoopConfiguration selects
 * only those actuator energies.
 *
 * Selected energies are saved in the _actEnergy attribute (std::vector<double>)
 * and are used by the selected algorithm. The order of the energies is the
 * same as the selected ActuatorDevices.
 *
 * @return number of selected actuator energies.
 * @author L.Piccoli
 */
int LoopConfiguration::configureActuatorEnergy() {
    int used = 0;
    _actEnergy.clear();

    // The number of elements in the _actEnergyPv must be at greater or
    // equal to the number of actuators used
    if ((int) _actEnergyPv.getValueAddress()->size() < _actuatorsUsedPv.getValue()) {
      _logger << Log::showtime << "Not enough values in ACTENERGY PV" << Log::flushpv;
      _logger << Log::showtime << "ERROR: Number of used actuators ("
	      << _actuatorsUsedPv.getValue()
	      << ") is larger than the number of energy values ("
	      << (int) _actEnergyPv.getValueAddress()->size()
	      << ")" << Log::flush;
      return -1;
    }
    std::vector<double>::iterator actIt = _actEnergyPv.getValueAddress()->begin();
    ActuatorSet *actuatorsFirstPattern = _actuators.begin()->second;
    for (ActuatorSet::iterator it = actuatorsFirstPattern->begin();
            it != actuatorsFirstPattern->end(); ++it, ++actIt) {
        if ((*it)->isUsed()) {
            double actEnergy = *actIt;
            _actEnergy.push_back(actEnergy);
            used++;
        }
    }
    return used;
}

/**
 * Copy the dispersion values from the MEASDSPR PV into the _dispersions vector.
 * The dispersions are only used by the Longitudinal feedback, and dispersions
 * for all devices must be present. This means all MxxUSED PV must be set to 1.
 *
 * @author L.Piccoli
 */
int LoopConfiguration::configureDispersions() {
    _dispersions.clear();

    for (std::vector<double>::iterator dispIt = _measDsprPv.getValueAddress()->begin();
	 dispIt != _measDsprPv.getValueAddress()->end(); ++dispIt) {
      double dispersion = *dispIt;
      _dispersions.push_back(dispersion);
    }
    return 0;
}

/**
 * Load the current F Matrix set to the $(LOOP):FMATRIX PV (waveform of doubles).
 *
 * The F Matrix is send as a waveform of doubles (_fMatrixPv) by the Java
 * configuration App. The number of rows and cols depends on the number of
 * measurement devices and the number of states used.
 *
 * Additionally, for TRxx loops, the rows are ordered first by X measurements
 * then Y measurements. The order is assumed to be sent correctly from the
 * config app (TODO: check if orded is really correct).
 *
 * The fifth column (energy) is received via the $(LOOP):FMATRIX PV, but
 * currently discarded.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LoopConfiguration::configureFMatrix() {
    // The number of rows is the number of Measurement Devices
    int rows = _measurementsUsedPv.getValue();

    // The number of rows must not exceed the number of measurements
    if (rows > MAX_MEASUREMENTS) {
        _logger << Log::showtime << "Matrix size does not match defined devices" << Log::flushpv;
        _logger << Log::showtime << "ERROR: Can't configure F Matrix, too many rows: "
                << rows << ". Maximum is " << MAX_MEASUREMENTS << Log::flush;
        return -1;
    }

    // The number of columns can be either 4 (X/Y position and X/Y angle),
    // unless there is only one BPM in use (i.e. only 2 measurement devices)
    int cols = 4;
    if (rows <= 2) {
        cols = 2;
    }
    // Add the energy col to the number of received columns
    int expectedCols = cols + 1;

    // If selected algorithm is Longitudinal, then the size of the F matrix
    // is different (it is a 6x6 matrix...)
    std::string algoName = _algorithmNamePv.getValue();
    if (algoName == LONGITUDINAL_ALGORITHM ||
	algoName == LONGITUDINAL_CHIRP_ALGORITHM) {
        rows = LONGITUDINAL_F_MATRIX_ROWS;
        cols = LONGITUDINAL_F_MATRIX_COLS;
    }

    // Copy from the waveform into the matrix
    _fMatrix.resize(rows, cols);
    std::vector<double> *fRows = _fMatrixPv.getValueAddress();

    // If the _fMatrixPv has less elements than needed, return error
    if (algoName != INJECTOR_LAUNCH_ALGORITHM) {
      if (rows * expectedCols > (int) fRows->size()) {
        _logger << Log::showtime << "Matrix size does not match defined devices" << Log::flushpv;
        _logger << Log::showtime << "ERROR: Can't configure F Matrix (expected "
                << rows * expectedCols << " elements, got "
                << (int) fRows->size() << "). Used measurements="
                << _measurementsUsedPv.getValue() << Log::flush;
        return -1;
      }
    }

    if (rows > 2) {
      std::vector<double>::iterator it = fRows->begin();
      for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
	  _fMatrix(i, j) = (*it);
	  ++it;
        }
        // Skip copying the energy column - if not Longitudinal!
        if (algoName != LONGITUDINAL_ALGORITHM &&
	    algoName != LONGITUDINAL_CHIRP_ALGORITHM) {
	  ++it;
        }
      }
    }
    // If there are only 2 measurements, we need to get col 0 and col 2 from
    // the _fMatrix
    else {
      std::vector<double>::iterator it = fRows->begin();
      for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
	  _fMatrix(i, j) = (*it);
	  ++it;
	  ++it; // skip col 1
        }
        // Skip copying the energy column - if not Longitudinal!
        if (algoName != LONGITUDINAL_ALGORITHM &&
	    algoName != LONGITUDINAL_CHIRP_ALGORITHM) {
	  ++it;
        }
      }
    }
    return 0;
}

/**
 * Load the current G Matrix set to the $(LOOP):GMATRIX PV (waveform of doubles).
 *
 * The G Matrix is send as a waveform of doubles (_gMatrixPv) by the Java
 * configuration App. The number of rows is the number of used states and
 * the number of cols is the number of actuators in use.
 *
 * The number of states and actuators is use is kept in the attributes
 * _statesUsed and _actuatorsUsed.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LoopConfiguration::configureGMatrix() {
    int rows = _statesUsedPv.getValue();
    int cols = _actuatorsUsedPv.getValue();

    // If selected algorithm is Longitudinal, then the size of the G matrix
    // is different (it is a 6x6 matrix...)
    std::string algoName = _algorithmNamePv.getValue();
    if (algoName == LONGITUDINAL_ALGORITHM ||
	algoName == LONGITUDINAL_CHIRP_ALGORITHM) {
        rows = LONGITUDINAL_G_MATRIX_ROWS;
        cols = LONGITUDINAL_G_MATRIX_COLS;
    } else if (rows > MAX_STATES || cols > MAX_ACTUATORS) {
        _logger << Log::showtime << "G Matrix size does not match defined devices" << Log::flushpv;
        return -1;
    }

    _gMatrix.resize(rows, cols);

    std::vector<double> *gVector = _gMatrixPv.getValueAddress();
    std::vector<double>::iterator it = gVector->begin();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (it != gVector->end()) {
                _gMatrix(i, j) = (*it);
                ++it;
            }
        }
    }

    return 0;
}

/**
 * Instantiates the loop algorithm according to the string set in the
 * $(LOOP):ITERATEFUNC PV.
 *
 * If a new algorithm is added this method must be modified in order
 * to recognize the algorithm name and instanciate the correct object.
 *
 * TODO: missing cases -> if a pattern changed, the algorithm is not deleted
 * TODO: missing cases -> if algorithm is the same, should keep it?
 *
 * @return 0 on success, -1 if the algorithm name is not recognized.
 * @author L.Piccoli
 */
int LoopConfiguration::configureAlgorithm() {
    std::string algoName = _algorithmNamePv.getValue();

    long numAlgorithms = _totalPoiPv.getValue();
    for (int i = 0; i < numAlgorithms; ++i) {
        Algorithm *algorithm;
        PatternMask patternMask = _patternMasks[i];

        if (algoName == NOOP_ALGORITHM) {
            algorithm = new SineWave(SineWave::NOOP);
        } else if (algoName == COUNTER_ALGORITHM) {
            algorithm = new SineWave(SineWave::COUNTER_MODE);
        } else if (algoName == TRAJECTORY_FIT_ALGORITHM) {
            algorithm = new TrajectoryFitStatic();
        } else if (algoName == TRAJECTORY_FIT_BY1_ALGORITHM) {
            algorithm = new TrajectoryFitBy1();
        } else if (algoName == FIXED_TRAJECTORY_FIT_ALGORITHM) {
            algorithm = new TrajectoryFitStatic();
        } else if (algoName == TRAJECTORY_FIT_POLYNOMIAL) {
            algorithm = new TrajectoryFitPoly();
        } else if (algoName == TRAJECTORY_FIT_PINV) {
            algorithm = new TrajectoryFitPinv();
        } else if (algoName == TRAJECTORY_FIT_EACT) {
            algorithm = new TrajectoryFitEact();
        } else if (algoName == INJECTOR_LAUNCH_ALGORITHM) {
            algorithm = new InjectorLaunch();
        } else if (algoName == LONGITUDINAL_ALGORITHM) {
            algorithm = new Longitudinal();
            _skipTmitCheck = true;
        } else if (algoName == LONGITUDINAL_CHIRP_ALGORITHM) {
            algorithm = new LongitudinalChirp();
            _skipTmitCheck = true;
        } else if (algoName == BUNCH_CHARGE_ALGORITHM) {
            algorithm = new BunchCharge();
	    _skipTmitCheck = true;
        } else if (algoName == SINE_ALGORITHM) {
            algorithm = new SineWave(SineWave::ONE_MEAS_ONE_ACT);
        } else if (algoName == SINE_ALGORITHM_N) {
            algorithm = new SineWave(SineWave::N_MEAS_N_ACT);
        } else {
            _logger << Log::showtime << "Invalid Feedback algorithm \"" << algoName.c_str()
                    << "\"" << Log::flushpv;
            _logger << Log::showtime << "ERROR: Invalid algorithm \"" << algoName.c_str()
                    << "\"" << Log::flush;
            return -1;
        }

        AlgorithmMap::iterator it;
        it = _algorithms.find(patternMask);
        // Delete previous algorithm
        if (it != _algorithms.end()) {
	      if (it->second != NULL) {
		delete it->second;
	        it->second = NULL;
	      }
	      _algorithms.erase(it);
        }
	algorithm->setPatternIndex(i);
        _algorithms.insert(std::pair<PatternMask, Algorithm *>(patternMask,
                algorithm));
    }
    return 0;
}

/**
 * This method gets the current patterns from the _poi[1..4]Pv waveform attributes
 * and saves them into the _patternMasks vector.
 *
 * The _patternMasks vector is used by the LoopThreads::configure() method.
 *
 * All devices (actuators, measurements and states) have a pattern mask
 * associated.
 * 
 * If a pattern mask changes, then it must be changed on the devices as well.
 * Additionally, the _measurements, _actuators and _states attributes are
 * std::maps, which are keyed on a PatternMask. Likewise these maps must be
 * updated.
 *
 * The POI pvs contain an inclusion mask, an exclusion mask and a timeslot.
 *
 * TODO: Handle case where number of patterns changes -> currently pattern
 * modifications are not allowed
 *
 * @return 0 on success
 * @author L.Piccoli
 */
int LoopConfiguration::configurePatterns() {
    // Save old patterns
    std::vector<PatternMask> previousPatternMasks = _patternMasks;

    long numPatterns = _totalPoiPv.getValue();

    if (numPatterns >= 1) {
        _patternMasks[0] = _poi1Pv.getValue();
	//	std::cout << (&_patternMasks[0]) << std::endl;
        if (reconfigurePattern(previousPatternMasks[0], _patternMasks[0]) != 0) {
            return -1;
        }
    }

    if (numPatterns >= 2) {
        _patternMasks[1] = _poi2Pv.getValue();
        if (reconfigurePattern(previousPatternMasks[1], _patternMasks[1]) != 0) {
            return -1;
        }
    }

    if (numPatterns >= 3) {
        _patternMasks[2] = _poi3Pv.getValue();
        if (reconfigurePattern(previousPatternMasks[2], _patternMasks[2]) != 0) {
            return -1;
        }
    }

    if (numPatterns >= 4) {
        _patternMasks[3] = _poi4Pv.getValue();
        if (reconfigurePattern(previousPatternMasks[3], _patternMasks[3]) != 0) {
            return -1;
        }
    }

    return 0;
}

/**
 * Make sure if a pattern changes that all the Devices are updated accordignly.
 *
 * TODO: currently pattern change is not allowed.
 *
 * @param previousPatternMask pattern mask of the previous configuration
 * @param newPatternMask pattern mask of the new configuration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LoopConfiguration::reconfigurePattern(const PatternMask &previousPatternMask,
        const PatternMask &newPatternMask) {
    if (previousPatternMask == newPatternMask) {
      //        std::cout << "INFO: Reconfiguring for same pattern mask." << std::endl;
        return 0; // Do nothing if patterns are the same
    }

    // Measurements
    if (reconfigureDevices<MeasurementMap, MeasurementMap::iterator,
            MeasurementDevice, MeasurementSet, MeasurementSet::iterator >
            (_measurements, previousPatternMask, newPatternMask) != 0) {
        _logger << Log::showtime << "ERROR: Failed to reconfigure Measurements." << Log::flush;
        return -1;
    }

    // The goal of this piece of code is to let the loop run even when there is
    // no beam (i.e. no pockcel_perm bit set). This is needed for debugging 
    // purposes, and will work only when M1DEV name starts with "FILE:"
    MeasurementMap::iterator mIt = _measurements.begin();
    MeasurementSet *set = mIt->second;
    MeasurementSet::iterator sIt = set->begin();
    MeasurementDevice *m1 = (*sIt);
    if (m1->isFile()) {
      _fakeBeam = true;
    }

    // Actuators
    if (reconfigureDevices<ActuatorMap, ActuatorMap::iterator,
            ActuatorDevice, ActuatorSet, ActuatorSet::iterator >
            (_actuators, previousPatternMask, newPatternMask) != 0) {
        _logger << Log::showtime << "ERROR: Failed to reconfigure Actuators." << Log::flush;
        return -1;
    }

    // States
    if (reconfigureDevices<StateMap, StateMap::iterator,
            StateDevice, StateSet, StateSet::iterator >
            (_states, previousPatternMask, newPatternMask) != 0) {
        _logger << Log::showtime << "ERROR: Failed to reconfigure States." << Log::flush;
        return -1;
    }

    // Setpoints
    if (reconfigureDevices<SetpointMap, SetpointMap::iterator,
            SetpointDevice, SetpointSet, SetpointSet::iterator >
            (_setpoints, previousPatternMask, newPatternMask) != 0) {
        _logger << Log::showtime << "ERROR: Failed to reconfigure Setpoints." << Log::flush;
        return -1;
    }

    return 0;
}

template<class DeviceMap, class DeviceMapIterator, class DeviceType, class DeviceSet, class DeviceSetIterator>
int LoopConfiguration::reconfigureDevices(DeviceMap &map,
const PatternMask &previousPatternMask, const PatternMask & newPatternMask) {
    // New pattern must not be in the map (search for the same mask).
    DeviceMapIterator mIt = map.find(newPatternMask);
    if (mIt != map.end()) {
        PatternMask mask = mIt->first;
        _logger << Log::showtime << "ERROR: New pattern mask already exists! "
                << mask.toString().c_str() << Log::flush;
        return -1;
    }

    // Previous pattern must be in the map (search for the mask)
    mIt = map.find(previousPatternMask);
    if (mIt == map.end()) {
        _logger << Log::showtime << "ERROR: Could not find previous pattern mask." << Log::flush;
        return -1;
    }

    // Remove previous pattern from map
    DeviceSet *set = mIt->second;
    map.erase(mIt);

    // Remove previous pattern mask from map
    mIt = map.find(previousPatternMask);
    if (mIt != map.end()) {
        _logger << Log::showtime << "ERROR: Could not delete previous pattern mask." << Log::flush;
        return -1;
    }

    // Set new pattern to devices in the set
    for (DeviceSetIterator sIt = set->begin(); sIt != set->end(); ++sIt) {
        (*sIt)->setPatternMask(newPatternMask);
    }

    // Add new pattern and same set with updated devices to map
    std::pair<DeviceMapIterator, bool> ret;
    ret = map.insert(std::pair<PatternMask, DeviceSet *>(newPatternMask, set));
    if (ret.second == false) {
        _logger << Log::showtime << "ERROR: Failed to insert new pattern mask." << Log::flush;
    }

    PatternMask p(ret.first->first);
    if (p != newPatternMask) {
        _logger << Log::showtime << "ERROR: Iterator returned from insert contains different pattern masks." << Log::flush;
        return -1;
    }

    return 0;
}

/**
 * Returns the latest value for the specified device/patternMask.
 *
 * @param deviceName name of the device
 * @param patternMask pattern mask of the deviced to be searched for
 * @param value returning value
 * @param timestamp value timestamp
 * @return 0 if device was found, -1 otherwise
 * @author L.Piccoli
 */
int LoopConfiguration::peek(std::string deviceName, PatternMask patternMask,
        DataPoint &dataPoint) {
    if (peekDeviceMap<MeasurementMap, MeasurementMap::iterator, MeasurementSet,
            MeasurementSet::iterator, MeasurementDevice >
            (_measurements, deviceName, patternMask, dataPoint/*value, timestamp*/) == 0) {
        return 0;
    }
    if (peekDeviceMap<ActuatorMap, ActuatorMap::iterator, ActuatorSet,
            ActuatorSet::iterator, ActuatorDevice >
            (_actuators, deviceName, patternMask, dataPoint/*value, timestamp*/) == 0) {
        return 0;
    }
    if (peekDeviceMap<StateMap, StateMap::iterator, StateSet,
            StateSet::iterator, StateDevice >
            (_states, deviceName, patternMask, dataPoint/*value, timestamp*/) == 0) {
        return 0;
    }
    if (peekDeviceMap<SetpointMap, SetpointMap::iterator, SetpointSet,
            SetpointSet::iterator, SetpointDevice >
            (_setpoints, deviceName, patternMask, dataPoint/*value, timestamp*/) == 0) {
        return 0;
    }
    return -1;
}

int LoopConfiguration::peek(std::string deviceName, PatternMask patternMask,
        DataPoint &dataPoint, int pos) {
    if (peekDeviceMap<MeasurementMap, MeasurementMap::iterator, MeasurementSet,
            MeasurementSet::iterator, MeasurementDevice >
            (_measurements, deviceName, patternMask, dataPoint/*value, timestamp*/, pos) == 0) {
        return 0;
    }
    if (peekDeviceMap<ActuatorMap, ActuatorMap::iterator, ActuatorSet,
            ActuatorSet::iterator, ActuatorDevice >
            (_actuators, deviceName, patternMask, dataPoint/*value, timestamp*/, pos) == 0) {
        return 0;
    }
    if (peekDeviceMap<StateMap, StateMap::iterator, StateSet,
            StateSet::iterator, StateDevice >
            (_states, deviceName, patternMask, dataPoint/*value, timestamp*/, pos) == 0) {
        return 0;
    }
    if (peekDeviceMap<SetpointMap, SetpointMap::iterator, SetpointSet,
            SetpointSet::iterator, SetpointDevice >
            (_setpoints, deviceName, patternMask, dataPoint/*value, timestamp*/, pos) == 0) {
        return 0;
    }
    return -1;
}


/**
 * Template method that retrives a Device from a map based on the Device name
 * and pattern mask.
 *
 * @param deviceName name of the Device being searched
 * @param patternMask copy of the pattern mask used by the device
 * @return a pointer to the Device, or NULL if it is not found
 * @author L.Piccoli
 */
template<class Map, class MapIterator, class Set, class SetIterator, class Device >
Device * LoopConfiguration::getDevice(Map &map, std::string deviceName,
PatternMask patternMask) {
    MapIterator it;
    it = map.find(patternMask);
    if (it == map.end()) {
        return NULL;
    }

    SetIterator setIterator;

    // Creating the following Device creates undesireds PvData instances!
    Device device("XX00", deviceName, 0, patternMask);
    setIterator = it->second->find(&device);
    if (setIterator == it->second->end()) {
        return NULL;
    }

    Device *actualDevice = *setIterator;
    return actualDevice;
}


/**
 * Template method that returns the latest value for the specified device/pattern
 * from the specified map.
 *
 * This method is used to peek at data from Measurements, Actuators and
 * States, which are stored in Maps of Sets, keyed on the Pattern.
 *
 * @param map map of devices (MeasurementMap, ActuatorMap or StateMap)
 * @param deviceName name of the device
 * @param patternMask pattern mask of the deviced to be searched for
 * @param value the latest value from the device found
 * @param timestamp value timestamp
 * @return 0 if device was found, -1 otherwise
 * @author L.Piccoli
 */
template<class Map, class MapIterator, class Set, class SetIterator, class Device>
int LoopConfiguration::peekDeviceMap(Map &map, std::string deviceName,
PatternMask patternMask,
DataPoint &dataPoint) {
    Device *actualDevice = getDevice<Map, MapIterator, Set, SetIterator, Device >
            (map, deviceName, patternMask);
    if (actualDevice == NULL) {
        return -1;
    }

    actualDevice->peek(dataPoint);
    return 0;
}
// ababbitt- inserting the template definition into the header file to fix compile error for LinuxRT
template<class Map, class MapIterator, class Set, class SetIterator, class Device>
int LoopConfiguration::peekDeviceMap(Map &map, std::string deviceName,
PatternMask patternMask,
DataPoint &dataPoint, int pos) {
    Device *actualDevice = getDevice<Map, MapIterator, Set, SetIterator, Device >
            (map, deviceName, patternMask);
    if (actualDevice == NULL) {
        return -1;
    }

    actualDevice->peek(dataPoint, pos);
    return 0;
}

template<class Map, class MapIterator, class Set, class SetIterator, class Device>
void LoopConfiguration::showDeviceMap(Map map, bool showUsed) {
    for (int i = 0; i < _totalPoiPv.getValue(); ++i) {
        PatternMask patternMask = _patternMasks[i];
        MapIterator mIt = map.find(patternMask);
        if (mIt != map.end()) {
            SetIterator setIterator;
            for (setIterator = mIt->second->begin();
                    setIterator != mIt->second->end(); ++setIterator) {
                Device *device = (*setIterator);
                if (showUsed) {
                    if (device->isUsed()) {
                        std::cout << "    ";
                        device->show();
                        std::cout << std::endl;
                    }
                } else {
                    if (device->hasCommunicationChannel()) {
                        std::cout << "    ";
                        device->show();
                        std::cout << std::endl;
                    }
                }
            }
            std::cout << std::endl;
        }
    }
}

/**
 * Prints out information of devices used by the feedback loop.
 *
 * @author L.Piccoli
 */
void LoopConfiguration::showDevices() {
    bool showUsed = true;
    if (_algorithmNamePv == LONGITUDINAL_ALGORITHM ||
	_algorithmNamePv == LONGITUDINAL_CHIRP_ALGORITHM) {
        showUsed = false;
    }

    std::cout << "  Measurements:" << std::endl;
    showDeviceMap<MeasurementMap, MeasurementMap::iterator, MeasurementSet,
            MeasurementSet::iterator, MeasurementDevice > (_measurements, showUsed);

    std::cout << "  Actuators:" << std::endl;
    showDeviceMap<ActuatorMap, ActuatorMap::iterator, ActuatorSet,
            ActuatorSet::iterator, ActuatorDevice > (_actuators, showUsed);

    std::cout << "  States:" << std::endl;
    showDeviceMap<StateMap, StateMap::iterator, StateSet, StateSet::iterator,
            StateDevice > (_states, showUsed);
}

/**
 * This method must be called after the StateDevices have been created.
 * It will assign the proper Setpoints to the StateDevices.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int LoopConfiguration::configureSetpoints() {
    return 0;
}

/**
 * Display the F and G matrices.
 *
 * @author L.Piccoli
 */
void LoopConfiguration::showMatrices() {
    std::cout << "  F Matrix (" << _fMatrix.size1() << ", "
            << _fMatrix.size2() << ")" << std::endl;
    for (int i = 0; i < (int) _fMatrix.size1(); ++i) {
        std::cout << "  ";
        for (int j = 0; j < (int) _fMatrix.size2(); ++j) {
            std::cout << _fMatrix(i, j) << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "  G Matrix (" << _gMatrix.size1() << ", "
            << _gMatrix.size2() << ")" << std::endl;
    for (int i = 0; i < (int) _gMatrix.size1(); ++i) {
        std::cout << "  ";
        for (int j = 0; j < (int) _gMatrix.size2(); ++j) {
            std::cout << _gMatrix(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

void LoopConfiguration::showReferenceOrbit() {
    std::vector<double>::iterator it;
    std::cout << "  ";
    for (it = _refOrbit.begin(); it != _refOrbit.end(); ++it) {
        std::cout << *it << " ";
    }
}

void LoopConfiguration::showActuatorEnergy() {
    std::vector<double>::iterator it;
    std::cout << "  ";
    for (it = _actEnergy.begin(); it != _actEnergy.end(); ++it) {
        std::cout << *it << " ";
    }
}

void LoopConfiguration::showDispersion() {
    std::vector<double>::iterator it;
    std::cout << "  ";
    for (it = _dispersions.begin(); it != _dispersions.end(); ++it) {
      std::cout << *it << " ";
    }
}

/*
 * File:   InjectorLaunch.cc
 * Author: lpiccoli
 * 
 * Created on July 16, 2010, 11:09 AM
 */

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <algorithm>
#include "InjectorLaunch.h"
#include "ExecConfiguration.h"

USING_FF_NAMESPACE

using namespace boost::numeric::ublas;

/**
 * Class constructor. Only initializes attributes
 *
 * @author L.Piccoli
 */
InjectorLaunch::InjectorLaunch(std::string algorithmName) :
TrajectoryFitGeneral(algorithmName),
_beamConfiguration(None),
_m5(NULL),
_m6(NULL) {
}

InjectorLaunch::~InjectorLaunch() {
}

/**
 * Initialize algorithm parameters based on data stored in the LoopConfiguration
 * (e.g. F Matrix).
 *
 * The transportR1X matrix is composed by the first rows of the
 * F matrix. The number of rows is defined by the number of BPMs
 * (i.e.) measurements used in the loop. The number of BPMs can
 * be considered as the number of MeasurementDevices / 2.
 *
 * @param configuration pointer to the LoopConfiguration
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int InjectorLaunch::configure(LoopConfiguration *configuration,
        MeasurementSet *measurements, ActuatorSet *actuators,
        StateSet *states) {
    _loopConfiguration = configuration;
    _measurements = measurements;
    _actuators = actuators;
    _states = states;

    _allMeasurements = *_measurements;

    _m5 = NULL;
    _m6 = NULL;

    MeasurementSet::iterator it;
    for (it = _measurements->begin(); it != _measurements->end(); ++it) {
        if ((*it)->getDeviceName().find("BPMS:IN20:771:X") != std::string::npos) {
            _m5 = (*it);
        }
        if ((*it)->getDeviceName().find("BPMS:IN20:781:X") != std::string::npos) {
            _m6 = (*it);
        }
    }

    if (_m5 == NULL) {
        throw Exception("Could not find BPM14");
    }

    // Both M5 & M6 must have USED PV set to 1, otherwise do not run!
    if (!_m5->isUsed()) {
        throw Exception("BPMS:IN20:771:X not USED, check config");
    }
    if (!_m6->isUsed()) {
        throw Exception("BPMS:IN20:781:X not USED, check config");
    }

    // Save the _loopConfiguration->_fMatrix locally. Parts of
    // it will be selected by the reallyConfig() method, according
    // to the beam configuration
    _fMatrix = _loopConfiguration->_fMatrix;

    // Save the _loopConfiguration->_refOrbit locally. Same reason
    // as above...
    _refOrbit = _loopConfiguration->_refOrbit;

    // Save Measurements/Actuators/States for Normal mode
    saveNormalConfiguration();

    // Save Measurements/Actuators/States for Spectrometer mode
    saveSpectrometerConfiguration();

    // This will select all the devices that have USED = 1.
    // The resulting configuration
    return configureNormal();
}

/**
 * Reconfigure the algorithm based on the TMIT readings from M5/M6.
 * 
 * This method is the first function invoked when Algorithm::calculate() is called,
 * allowing the InjectorLaunch to reconfigure if there has been a change to
 * where the beam is going to.
 *
 * If TMIT on M5 is good, then use the Normal configuration
 *
 * If TMIT on M5 is bad then use only the BPMS before the spectrometer.
 *
 * @return 0
 * @author L.Piccoli
 */
int InjectorLaunch::checkConfig() {
    double m5Tmit = _m5->getTmit();

    double lowTmit = ExecConfiguration::getInstance()._tmitLowPv.getValue();

    // Reconfigure to Normal setup
    if ((m5Tmit > lowTmit && _beamConfiguration == Spectrometer) ||
	_beamConfiguration == None) {
        return configureNormal();
    }

    // Switch to Spectrometer configuration
    if ((m5Tmit < lowTmit && _beamConfiguration == Normal) ||
	(m5Tmit < lowTmit && _beamConfiguration == None)) {
        return configureSpectrometer();
    }

    return 0;
};

/**
 * The calculate() is overwritten here only for the purpose of restoring
 * _allMeasurement devices to the original _measurements set. This must
 * be done, otherwise if the loop is stopped only the measurement devices
 * for the normal or spectrometer configuration will be removed from the
 * MeasurementCollector.
 *
 * @return returns status of TrajectoryFitGeneral::calculates()
 * @author L.Piccoli
 */
int InjectorLaunch::calculate() {
  int result = TrajectoryFitGeneral::calculate();

  *_measurements = _allMeasurements;

  return result;
}

/**
 * Select those MeasurementDevices that are used in the Normal mode of
 * operation, i.e. beam continues downstream through BC1. Those devices
 * whose USED PV are 0 are left out - usually BPMS:IN20:525:X/Y.
 *
 * Save a version of the F Matrix and reference orbit vector used when
 * in Normal mode.
 *
 * @author L.Piccoli
 */
void InjectorLaunch::saveNormalConfiguration() {
  // Make sure at least M5 BPM is used
  if (!_m5->isUsed()) {
    throw Exception("BPM14 not is use, check configuration");
  }

  if (!_m6->isUsed()) {
    throw Exception("BPM15 not is use, check configuration");
  }

  // Select Normal mode MeasurementDevices - set the USEDBYLOOP PVs
  // to make it easier to build the F Matrix below. All measurements
  // that have the USED PV set will have the USEDBYLOOP PV set.
  int numDevices = 0;
  for (MeasurementSet::iterator it = _allMeasurements.begin();
       it != _allMeasurements.end(); ++it) {
    if ((*it)->isUsed()) {
      _measurementsNormal.insert((*it));
      (*it)->setUsedBy(true);
      numDevices++;
    } else {
      // If USED PV is 0, then USEDBYLOOP PV must also be set to 0
      (*it)->setUsedBy(false);
    }
  }


  _fMatrixNormal.resize(numDevices, _fMatrix.size2());
  
  saveFMatrix(_fMatrixNormal, numDevices);
  
  saveReferenceOrbit(_refOrbitNormal, numDevices);
}

/**
 * Select those MeasurementDevices that are used in the Spectrometer mode
 * of operation, i.e. beam goes to the spectrometer dump and/or is not getting
 * to BPM14. In this setup the BPMS:IN20:771/781:X/Y are left out.
 *
 * Save a version of the F Matrix and reference orbit vector used when
 * in Spectrometer mode.
 *
 * @author L.Piccoli
 */
void InjectorLaunch::saveSpectrometerConfiguration() {
    // Select Spectrometer mode MeasurementDevices - set the USEDBYLOOP PVs
    // to make it easier to build the F Matrix below
    int numDevices = 0;
    for (MeasurementSet::iterator it = _allMeasurements.begin();
            it != _allMeasurements.end(); ++it) {
        if ((*it)->isUsed()) {
            if ((*it)->getDeviceName().find("BPMS:IN20:7") == std::string::npos) {
                _measurementsSpectrometer.insert((*it));
                (*it)->setUsedBy(true);
                numDevices++;
            } else {
                (*it)->setUsedBy(false);
            }
        } else {
            // If USED PV is 0, then USEDBYLOOP PV must also be set to 0
            (*it)->setUsedBy(false);
        }
    }

    _fMatrixSpectrometer.resize(numDevices, _fMatrix.size2());

    saveFMatrix(_fMatrixSpectrometer, numDevices);

    saveReferenceOrbit(_refOrbitSpectrometer, numDevices);
}

/**
 * Save selected rows of the original F Matrix to the passed matrix.
 * Only those devices that have USEDBYLOOP PV set to 1 are selected.
 *
 * @param fMatrix matrix that will receive the selected rows
 * @param numDevices number of rows to be selected
 * @author L.Piccoli
 */
void InjectorLaunch::saveFMatrix(Matrix& fMatrix, int numDevices) {
    int sourceIndex = 0;
    int destIndex = 0;
    for (MeasurementSet::iterator it = _allMeasurements.begin();
            it != _allMeasurements.end(); ++it) {
        // If device is not is use, must skip it, this row will not
        // be present in the F Matrix
        if ((*it)->isUsed()) {
            if ((*it)->getUsedBy()) {
                matrix_row<Matrix> sourceRow(_fMatrix, sourceIndex);
                matrix_row<Matrix> destRow(fMatrix, destIndex);
                destRow = sourceRow;
                ++destIndex;
                ++sourceIndex;
            }
            // Skip original F Matrix row if device is listed an USED, but
            // not used in the current configuration
            if (!(*it)->getUsedBy()) {
                ++sourceIndex;
            }
        }
    }
}

/**
 * Save selected values from the original Reference Orbit vector, saving
 * to the vector passed as parameter.
 *
 * The original Reference Orbit contains only values for those devices that
 * have the MxxUSED PV set to 1. The REFORBIT PV containing all values is
 * processed by the LoopConfiguration.
 *
 * Here we have to select only those that will be used according to the
 * InjectorLaunch configuration. Only those devices that have the MxxUSEDBYLOOP
 * set are selected.
 *
 * @param refOrbit vector that will receive the selected values
 * @param numDevices number of values to be copied (equivalent to 2x the number
 * of used BPMs)
 * @author L.Piccoli
 */
void InjectorLaunch::saveReferenceOrbit(std::vector<double> &refOrbit,
        int numDevices) {
    int sourceIndex = 0;
    int destIndex = 0;
    refOrbit.resize(numDevices);
    for (MeasurementSet::iterator it = _allMeasurements.begin();
            it != _allMeasurements.end(); ++it) {
        // If device is not is use, must skip it, this row will not
        // be present in the F Matrix
        if ((*it)->isUsed()) {
            if ((*it)->getUsedBy()) {
                refOrbit[destIndex] = _refOrbit[sourceIndex];
                ++destIndex;
                ++sourceIndex;
            }
            // Skip original F Matrix row if device is listed an USED, but
            // not used in the current configuration
            if (!(*it)->getUsedBy()) {
                ++sourceIndex;
            }
        }
    }
}

/**
 * Select Measurements/Actuators/States to be used. Beam is travelling through:
 * M1 -> M2 -> M3 -> M4 -> M5 -> M6 -> ...
 *
 * The _measurements, _actuators and _states must be changed to the sets
 * used by the Normal configuration.
 */
int InjectorLaunch::configureNormal() {
    // Change Measurements to set saved for Normal mode
    *_measurements = _measurementsNormal;

    // Change the F Matrix
    _loopConfiguration->_fMatrix = _fMatrixNormal;

    // Change the reference orbit
    _loopConfiguration->_refOrbit = _refOrbitNormal;

    std::cout << "Ref orbit: ";
    for (int i = 0; i < _loopConfiguration->_refOrbit.size(); i++) {
      std::cout << _loopConfiguration->_refOrbit.at(i) << " ";
    }
    std::cout << std::endl;

    // Now set the MxxUSEDBYLOOP PV
    int i = 0;
    for (MeasurementSet::iterator it = _allMeasurements.begin();
            it != _allMeasurements.end(); ++it, ++i) {
        if ((*it)->isUsed()) {
	  (*it)->setUsedBy(true);
        }
	else {
	  (*it)->setUsedBy(false);
	}
    }

    _beamConfiguration = Normal;

    _loopConfiguration->_logger << Log::showtime << "Feedback in NORMAL mode."
			    << Log::flushpvnoalarm;

    return reallyConfig();
}

/**
 * Select Measurements/Actuators/States to be used. Beam is travelling through:
 * M1 -> M2 -> M3 -> M4 -> ?
 *
 * The _measurements, _actuators and _states must be changed to the sets
 * used by the Normal configuration.
 */
int InjectorLaunch::configureSpectrometer() {
    // Change Measurements to set saved for Spectrometer mode
    *_measurements = _measurementsSpectrometer;

    // Change the F Matrix
    _loopConfiguration->_fMatrix = _fMatrixSpectrometer;

    // Change the reference orbit
    _loopConfiguration->_refOrbit = _refOrbitSpectrometer;

    std::cout << "Ref orbit: ";
    for (int i = 0; i < _loopConfiguration->_refOrbit.size(); i++) {
      std::cout << _loopConfiguration->_refOrbit.at(i) << " ";
    }
    std::cout << std::endl;

    // Now set the MxxUSEDBYLOOP PV
    int i = 0;
    for (MeasurementSet::iterator it = _allMeasurements.begin();
            it != _allMeasurements.end(); ++it, ++i) {
        if ((*it)->isUsed()) {
            if ((*it)->getDeviceName().find("BPMS:IN20:7") != std::string::npos) {
                (*it)->setUsedBy(false);
            } else {
                (*it)->setUsedBy(true);
            }
        }
	else {
	  (*it)->setUsedBy(false);
	}
    }

    _beamConfiguration = Spectrometer;

    _loopConfiguration->_logger << Log::showtime << "Feedback in SPECTROMETER mode."
				<< Log::flushpvnoalarm;

    return reallyConfig();
}

void InjectorLaunch::show() {
    std::cout << "        -> Beam: ";
    if (_beamConfiguration == Normal) {
        std::cout << "Normal" << std::endl;
    } else if (_beamConfiguration == Spectrometer) {
        std::cout << "Spectrometer" << std::endl;
    } else {
        std::cout << "None" << std::endl;
    }
}

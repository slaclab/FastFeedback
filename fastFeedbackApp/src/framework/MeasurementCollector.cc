/* 
 * File:   MeasurementCollector.cc
 * Author: lpiccoli
 * 
 * Created on June 3, 2010, 2:04 PM
 */

#include <iostream>

#include "MeasurementCollector.h"
#include "ExecConfiguration.h"
#include "Log.h"
#include "PatternManager.h"

#include <cadef.h>

USING_FF_NAMESPACE

/**
 * Updates every MeasurementDevice for a specific pattern. This method is called
 * by the CollectorThread once the timer (of approximatelly 2.5 ms) started after
 * the fiducial is received and there is a pattern match.
 *
 * All PatternMasks registered are checked, and if there is a match, the
 * associated Devices read out.
 *
 * @param pattern current pattern of interest, received by the PatternManager
 * and forwarded to the CollectorThread.
 * @return returns 0 if there were updates, -1 if there are no measurements
 * to be updated
 * @author L.Piccoli
 */
int MeasurementCollector::update(Pattern evrPattern) {
    CollectorMeasurementMap::iterator iterator;
    bool match = false;

    if (_measurements.empty()) {
      return -1;
    }

    _updateStats.start();
    iterator = _measurements.begin();
    while (iterator != _measurements.end()) {
        PatternMask patternMask = (*iterator).first;
        bool thisMatch = evrPattern.match(patternMask);
        if (thisMatch) {
            _patternMatchCount++;
#ifdef CHECK_BEAM
	    // If EVR pattern has no pockel_cel, then we should not read 
	    // measurements, there won't be any...
	    if (evrPattern.hasBeam()) {
#endif
	      _updateSetStats.start();
	      updateMeasurementSet(iterator->second, evrPattern.getPulseId());
	      _updateSetStats.end();
#ifdef CHECK_BEAM
	    }
#endif

            match = true;
        }
        ++iterator;
    };
      
    _updateStats.end();

    if (!match) {
        _patternNotMatchedCount++;
    }


    return 0;
}

/**
 * Updates the devices in the specified set. Timestamp of the measurements
 * are checked MeasurementDevices are using FCOM.
 *
 * @param measurementSet set of MeasurementDevices to be updated
 * @param patternPulseId pulse id from the event received from the EVR containing
 * the expected measurement pulse id. PulseIds must match if FcomChannel is
 * used by the MeasurementDevice
 * @return 0
 * @author L.Piccoli
 */
int MeasurementCollector::updateMeasurementSet(CollectorMeasurementSet *measurementSet,
        epicsUInt32 patternPulseId) {
  CollectorMeasurementSet::iterator iterator;
  bool readErrors = false;

    for (iterator = measurementSet->begin();
            iterator != measurementSet->end(); iterator++) {
      _readStats.start();
      if ((*iterator)->read() != 0) {
	_readStats.end();
	_readErrorCount++;
	readErrors = true;
      } else {
	_readStats.end();
	// Skip pulse id checking if TMIT is low
#ifdef CHECK_BEAM
	if ((*iterator)->getTmit() >
	    ExecConfiguration::getInstance()._tmitLowPv.getValue()) {
#endif
	  // Check PulseID of the new measurement
	  _checkTimestampStats.start();
	  if (!(*iterator)->checkPulseId(patternPulseId)) {
	    _measurementTimestampMismatchCount++;

	    Log::getInstance() << Log::flagPulseId << Log::dpWarn
			       << "WARN: MeasurementCollector::updateMeasurementSet() Meas PULSEID="
			       << (int)(*iterator)->peekPulseId() << ", Patt PULSEID="
			       <<  (int) patternPulseId << Log::dp;
#ifdef CHECK_BEAM
	  }
#endif
	  _checkTimestampStats.end();
	}
      }
    }
    
    if (readErrors) {
      Log::getInstance() << "Failed to read measurements" << Log::flush;
    }

    return 0;
}

/**
 * Add the specified MeasurementDevice to the list of devices to be updated.
 * The device is added to the list of devices that respond to the same pattern.
 *
 * The _mutex must be locked/unlocked while colling this method. (e.g.
 * MeasurementCollector::getInstance().lock())
 * 
 * @param measurement MeasurementDevice to be added to the collector list
 * @return 0 if device was added successfully, -1 if there are errors. If device
 * is already on the list it is not added again.
 * @author L.Piccoli
 */
int MeasurementCollector::add(MeasurementDevice* measurement) {
    CollectorMeasurementSet *measurementSet;

    // If this is a new pattern mask create a new MeasurementSet,
    // otherwise retrieve existing MeasurementSet
    if (!hasPatternMask(measurement->getPatternMask())) {
        measurementSet = new CollectorMeasurementSet;
        _measurements.insert(std::pair<PatternMask, CollectorMeasurementSet *>
                (measurement->getPatternMask(), measurementSet));
    } else {
        CollectorMeasurementMap::iterator iterator;
        iterator = _measurements.find(measurement->getPatternMask());
        measurementSet = iterator->second;
    }

    unsigned size = measurementSet->size();
    measurementSet->insert(measurement);

    if (size == measurementSet->size()) {
      Log::getInstance() << "WARN: Failed to add measurement "
			 << measurement->getDeviceName().c_str() << " (" 
			 << measurement->getName().c_str() << ")"
			 << Log::flush;
    }
    _measurementCount += (measurementSet->size() - size);

    return 0;
}

/**
 * Remove the specified measurement from the list of devices to be updated.
 *
 * TODO: When std::multiset is used to hold the measurements this method must
 * be updated. Cannot call erase(measurement) directly. Must search for the
 * measurement that has the same slot name (e.g. TR01, TR02)
 *
 * @param measurement pointer to the device to be removed
 * @return always returns 0
 * @author L.Piccoli
 */
int MeasurementCollector::remove(MeasurementDevice* measurement) {
    if (hasPatternMask(measurement->getPatternMask())) {
        CollectorMeasurementMap::iterator iterator;
        iterator = _measurements.find(measurement->getPatternMask());

        int size = iterator->second->size();
        iterator->second->erase(measurement);
        _measurementCount -= (size - iterator->second->size());

        if (iterator->second->empty()) {
            _measurements.erase(iterator);
        }
    }

    return 0;
}

/**
 * Returns whether a pattern mask is already defined in the map of watched
 * pattern masks.
 *
 * @param patternMask the pattern mask to be searched
 * @return true if patternMask is defined, false if not
 * @author L.Piccoli
 */
bool MeasurementCollector::hasPatternMask(PatternMask patternMask) {
    CollectorMeasurementMap::iterator iterator;

    iterator = _measurements.find(patternMask);
    if (iterator == _measurements.end()) {
        return false;
    } else {
        return true;
    }
}

/**
 * Return the MeasurementCollector singleton
 *
 * @return the MeasurementCollector
 * @author L.Piccoli
 */
MeasurementCollector &MeasurementCollector::getInstance() {
    static MeasurementCollector _instance;
    return _instance;
}

/**
 * Remove all entries from the _measurements. Used for testing only.
 * 
 * @author L.Piccoli
 */
void MeasurementCollector::clear() {
    _measurements.erase(_measurements.begin(), _measurements.end());
    _measurementCount = 0;
}

void MeasurementCollector::clearStats() {
  _updateStats.clear();
  _updateSetStats.clear();
};

void MeasurementCollector::show() {
    std::cout << "  number of devices: " << _measurementCount << std::endl;
    std::cout << "  read errors:       " << _readErrorCount << std::endl;
    std::cout << "  ts mismatches:     " << _measurementTimestampMismatchCount
            << std::endl;
    std::cout << "  patterns:" << std::endl;
    std::cout << "  -> not matched: " << _patternNotMatchedCount << std::endl;
    std::cout << "  -> matched: " << _patternMatchCount << std::endl;
    std::cout << "  -> mutex info: " << std::endl;
    _mutex->show(4);
}

void MeasurementCollector::showDebug() {
    _updateStats.show("  ");
    _updateSetStats.show("  ");
    _readStats.show("  ");
    _checkTimestampStats.show("  ");
    std::cout << "  pulseId mismatches: " << _measurementTimestampMismatchCount
            << std::endl;
}

void MeasurementCollector::showMeasurements() {
    CollectorMeasurementMap::iterator mit;

    lock();
    std::cout << "--- MeasurementCollector ---" << std::endl;
    if (_measurements.empty()) {
        std::cout << "  -> There are no measurements." << std::endl;
    } else {
        for (mit = _measurements.begin(); mit != _measurements.end(); ++mit) {
            PatternMask mask = mit->first;
            std::cout << &mask << std::endl;
            CollectorMeasurementSet::iterator sit;
            for (sit = mit->second->begin(); sit != mit->second->end(); ++sit) {
                MeasurementDevice *device;
                device = (*sit);
                std::cout << "  " << device->getDeviceName() << " - "
			  << device->peek() << std::endl;
		//		device->show();
            }
        }
    }
    unlock();
}

void MeasurementCollector::lock() {
    _mutex->lock();
}

void MeasurementCollector::unlock() {
    _mutex->unlock();
}

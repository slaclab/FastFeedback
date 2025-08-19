/* 
 * File:   MeasurementCollector.h
 * Author: lpiccoli
 *
 * Created on June 3, 2010, 2:04 PM
 */

#ifndef _MEASUREMENTCOLLECTOR_H
#define	_MEASUREMENTCOLLECTOR_H

#include <iostream>
#include <map>
#include <set>
#include "Defs.h"
#include "Pattern.h"
#include "PatternMask.h"
#include "MeasurementDevice.h"
#include "TimeUtil.h"
#include <epicsMutex.h>

class MeasurementCollectorTest;

FF_NAMESPACE_START

//---------------------------------------------------------------------------
// TODO: In order to have loops using same MeasurementDevices for the same
// PatternMask we need to change from std::set to std::multiset. The remove()
// method must be changed to find the actual MeasurementDevice being removed.
// See std::multiset::erase reference manual:
//   http://www.cplusplus.com/reference/stl/multiset/erase/
//---------------------------------------------------------------------------
typedef std::set<MeasurementDevice *, DevicePvNameCompare> CollectorMeasurementSet;
typedef std::map<PatternMask, CollectorMeasurementSet *> CollectorMeasurementMap;

/**
 * The MeasurementCollector contains a list of all measured machine parameters
 * used by the feedback loops.
 *
 * Measurements are organized in sets that are visible to the Loop thread.
 *
 * Values are all update at once when the timer expires sending a read
 * measurements event to the CollectorThread. The CollectorThread asks for
 * the MeasurementCollector to go through all devices in the list and update
 * them.
 *
 * The values are retrieved either using FCOM or Channel Access.
 *
 * Each measurement can have a history of values and an average of the previous
 * N points.
 *
 * Pattern Awareness: the Measurement must receive the PATTERN_EVENT from the
 * PatternManager. It will be used by the MeasurementDevices to retrive new
 * values only when there is a pattern of interest.
 * 
 * @author L.Piccoli
 */
class MeasurementCollector {
public:

    MeasurementCollector() :
      _measurementCount(0),
      _readErrorCount(0),
      _patternMatchCount(0),
      _patternNotMatchedCount(0),
      _measurementTimestampMismatchCount (0),
      _updateStats(50, "Measurement Collector Update Stats"),
      _updateSetStats(50, "Measurement Set Collector Update Stats"),
      _readStats(50, "Reading Measurements"),
      _checkTimestampStats (50, "Checking PULSEIDs") {
        _mutex = new epicsMutex();
    };

    virtual ~MeasurementCollector() {
        delete _mutex;
    };

    int update(Pattern evrPattern);
    int add(MeasurementDevice *measurement);
    int remove(MeasurementDevice *measurement);
    void clear();
    void clearStats();

    static MeasurementCollector &getInstance();

    int getMeasurementCount() {
        return _measurementCount;
    };

    void lock();
    void unlock();

    void show();
    void showDebug();
    void showMeasurements();

    friend class MeasurementCollectorTest;
private:
    int updateMeasurementSet(CollectorMeasurementSet *measurementSet,
            epicsUInt32 patternPulseId);

    /** Mutex to control access to the _measurements MeasurementMap */
    epicsMutex *_mutex;

    bool hasPatternMask(PatternMask patternMask);

    /**
     * For every PatternMask there is an associated set of MeasurementDevices.
     * When update() is called only the MeasurementDevices whose PatternMask
     * match the received Pattern are updated.
     */
    CollectorMeasurementMap _measurements;

    /** Count the number of devices being collected */
    int _measurementCount;

    /** Count the number of errors while reading devices */
    long _readErrorCount;

    /** Count the number of pattern matches */
    long _patternMatchCount;

    /** Count the number of patterns not matched */
    long _patternNotMatchedCount;

    /** Count the number of PulseId mismatches */
    long _measurementTimestampMismatchCount;

    /** Stats about the update running time */
    TimeAverage _updateStats;
    TimeAverage _updateSetStats;
    TimeAverage _readStats;
    TimeAverage _checkTimestampStats;
};

FF_NAMESPACE_END

#endif	/* _MEASUREMENTCOLLECTOR_H */


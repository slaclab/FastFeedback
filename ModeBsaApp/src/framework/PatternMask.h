/* 
 * File:   PatternMask.h
 * Author: lpiccoli
 *
 * Created on September 21, 2010, 11:54 AM
 */

#ifndef _PATTERNMASK_H
#define	_PATTERNMASK_H

#include <iostream>
#include <fstream>
#include <epicsTime.h>
#include <evrTime.h> // for evrModifier_ta
#include <evrPattern.h>
#include <vector>
#include <string>
#include "Defs.h"

FF_NAMESPACE_START

/**
 * This struct contains all the relevant information about a pattern masks
 * used to match pattern received by the EVR.
 *
 * @author L.Piccoli
 */
typedef struct PatternMask {
    static const unsigned int PatternPvSize = 21;

    /** Inclusion mask, defined by the POI PV */
    evrModifier_ta _inclusionMask;

    /** Exclusion mask, defined by the POI PV */
    evrModifier_ta _exclusionMask;

    /** Time slot, defined by the POI PV */
    unsigned long _timeSlot;

    PatternMask();
    PatternMask(const PatternMask & other);

    bool operator==(const PatternMask & other) const;
    bool operator!=(const PatternMask & other) const;
    bool operator<(const PatternMask & other) const;
    PatternMask &operator=(const std::vector<unsigned short> &vector);
    PatternMask &operator=(PatternMask & other);
    std::string toString();
   
    /** Determines whether the destination is HXR or SXR */ 
    bool destinationHXR();

    /**
     * Print out the PatternMask information to the specified stream.
     *
     * @author L.Piccoli
     */
    friend std::ostream & operator<<(std::ostream &os, PatternMask * const pattern) {
        os << "PatternMask: [IncMask: ";
        for (int i = 0; i < 5; ++i) {
            os << std::hex << "0x" << pattern->_inclusionMask[i] << " ";
        }
        os << std::hex << "0x" << pattern->_inclusionMask[5];
        os << "; ExcMask: ";
        for (int i = 0; i < 5; ++i) {
            os << std::hex << "0x" << pattern->_exclusionMask[i] << " ";
        }
        os << std::hex << "0x" << pattern->_exclusionMask[5] << std::dec << "; ";
        os << "TS: " << std::dec << pattern->_timeSlot << "]";
        return os;
    }

    static PatternMask ZERO_PATTERN_MASK;
} PatternMask;

FF_NAMESPACE_END

#endif	/* _PATTERN_H */


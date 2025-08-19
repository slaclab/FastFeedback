/* 
 * File:   Pattern.h
 * Author: lpiccoli
 *
 * Created on May 26, 2010, 11:49 AM
 */

#ifndef _PATTERN_H
#define	_PATTERN_H

#include <iostream>
#include <fstream>
#include <epicsTime.h>
#include <evrTime.h> // for evrModifier_ta
#include <evrPattern.h>
#include <vector>
#include <string.h>
#include "Defs.h"
#include "PatternMask.h"

FF_NAMESPACE_START

/**
 * This struct contains all the relevant information about a pattern received
 * by the EVR.
 *
 * @author L.Piccoli
 */
typedef struct Pattern {
    /** Pattern received from the EVR */
    evrModifier_ta _value;

    /** Timestamp of this pattern */
    epicsTimeStamp _timestamp;

    /** Pattern Status (see evrPattern.h) */
    unsigned long _status;

    Pattern();
    Pattern(const PatternMask &mask);
    Pattern(const Pattern & other);
    Pattern(const evrModifier_ta &value, const epicsTimeStamp timestamp);
    bool is10Hz();
    bool is5Hz();
    bool is1Hz();
    bool isHalfHz();
    bool isTs1();
    bool isTs4();
    bool hasBeam();
    bool match(PatternMask &patternMask);
    bool operator==(const Pattern & other) const;
    bool operator==(const std::vector<unsigned short> &other) const;
    bool operator!=(const Pattern & other) const;
    bool operator<(const Pattern & other) const;
    Pattern & operator=(const Pattern & other);
    epicsUInt32 getPulseId();

    /**
     * Print out the pattern information to the specified stream.
     *
     * @author L.Piccoli
     */
    friend std::ostream & operator<<(std::ostream &os, Pattern * const pattern) {
        os << "Pattern: [Evr: ";
        for (int i = 0; i < 5; ++i) {
            os << std::hex << "0x" << pattern->_value[i] << " ";
        }
        os << std::hex << "0x" << pattern->_value[5] << std::dec << "]";

        if (pattern->_timestamp.secPastEpoch != 0 &&
                pattern->_timestamp.nsec != 0) {
            os << "[" << pattern->getPulseId() << "]";
        }
        return os;
    }

    static Pattern ZERO_PATTERN;

} Pattern;

FF_NAMESPACE_END

#endif	/* _PATTERN_H */


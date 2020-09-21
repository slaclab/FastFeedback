
#include "PatternMask.h"
#include "Log.h"
#include <sstream>

USING_FF_NAMESPACE

PatternMask PatternMask::ZERO_PATTERN_MASK;

/**
 * Struct constructor initializing all fields to zero
 *
 * @author L.Piccoli
 */
PatternMask::PatternMask() {
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _inclusionMask[i] = 0;
        _exclusionMask[i] = 0;
    }
    _timeSlot = 0;
}

/**
 * Assignment constructor, all fields from the source PatternMask
 * are copied to the new PatternMask.
 *
 * @param other PatternMask being copied.
 * @author L.Piccoli
 */
PatternMask::PatternMask(const PatternMask & other) {
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _inclusionMask[i] = other._inclusionMask[i];
        _exclusionMask[i] = other._exclusionMask[i];
        _timeSlot = other._timeSlot;
    }
}

/**
 * Returns whether two PatternMasks are the same, i.e. the
 * inclusion/exclusion masks and time slot are equal.
 *
 * @param other PatternMask being compared to this one
 * @return true if _includeMask/_excludeMask/_timeSlot are
 * the same on both PatternMasks, false otherwise
 * @author L.Piccoli
 */
bool PatternMask::operator==(const PatternMask & other) const {
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        if (_inclusionMask[i] != other._inclusionMask[i]) {
            return false;
        }
        if (_exclusionMask[i] != other._exclusionMask[i]) {
            return false;
        }
    }
    if (_timeSlot != other._timeSlot) {
        return false;
    }
    return true;
}

/**
 * Returns whether two PatternMasks are different.
 * The == operator is being called for the comparison.
 *
 * @param other PatternMask being compared to this one
 * @return false if _includeMask/_excludeMask/_timeSlot are
 * the same on both PatternMasks, true otherwise
 * @author L.Piccoli
 */
bool PatternMask::operator!=(const PatternMask & other) const {
    return !(*this == other);
}

/**
 * This operator is defined in order to allow PatternMasks to be used
 * as Keys in std::map.
 *
 * The order of comparison is: _timeSlot, _inclusionMask[0..N],
 * _excludeMask[0..].
 *
 * @param other PatternMask being compared to this one
 * @return true if this PatternMask is smaller than the other Pattern,
 * false otherwise
 * @author L.Piccoli
 */
bool PatternMask::operator<(const PatternMask & other) const {
  /*
  PatternMask p2 = other;
  std::string str2 = p2.toString();
  PatternMask p1 = *this;
  std::string str1 = p1.toString();

  Log::getInstance() << Log::dpInfo << "INFO: PatternMask::operator< (this="
		     << str1.c_str() << ", other="
		     << str2.c_str() << ")"
		     << Log::flush;
  */
    if (*this == other) {
        return false;
    }

    if (_timeSlot < other._timeSlot) {
        return true;
    } else if (_timeSlot == other._timeSlot) {
        for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
            if (_inclusionMask[i] > other._inclusionMask[i]) {
                return false;
            } else if (_inclusionMask[i] < other._inclusionMask[i]) {
                return true;
            }
        }
        for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
            if (_exclusionMask[i] > other._exclusionMask[i]) {
                return false;
            } else if (_exclusionMask[i] < other._exclusionMask[i]) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Assign a pattern mask from a std::vector<unsigned short>. The input vector
 * contains the inclusion/exclusion masks and the timeslot.
 *
 * The input vector has 21 unsigned short elements. The first 10 are
 * the inclusion mask, starting at element 0, ending on element 5. Element
 * 6 (MPS related) is not included. The first short is the low word and the
 * second short is the high word. The next 10 unsigned shorts are for the
 * exclusion mask, and the last unsigned short is the timeslot.
 *
 * Change (01/27/2011): The config app now sends the POI shifted to the left,
 * i.e. the first elements of the inclusion/exclusion masks are now dropped.
 * New elements are added to the end. This method must put the POI data in
 * the right spot in the _inclusionMask and _exclusionMask. Now the first
 * element of the masks should be ZERO, and the data received from the PV starts
 * at the second element.
 *
 * @param vector containing 21 unsigned shorts
 * @author L.Piccoli
 */
PatternMask &PatternMask::operator=(const std::vector<unsigned short> &vector) {
    if (vector.size() < PatternMask::PatternPvSize) {
        return *this;
    }

    // First 10 elements are the inclusion mask (except last modifier)
    for (int i = 0; i < MAX_EVR_MODIFIER - 1; ++i) {
        _inclusionMask[i+1] = vector[i * 2 + 1] << 16 | vector[i * 2];
    }

    // Next 10 elements are the exclusion mask (again, except last modifier)
    int maskIndex = 0;
    for (int i = MAX_EVR_MODIFIER - 1; i < (MAX_EVR_MODIFIER - 1) * 2; ++i) {
        _exclusionMask[maskIndex+1] = vector[i * 2 + 1] << 16 | vector[i * 2];
        maskIndex++;
    }

    _timeSlot = vector[PatternMask::PatternPvSize - 1];

    _inclusionMask[0] = 0;
    _exclusionMask[0] = 0;

    return *this;
}

/**
 * Assignment operator specifying another PatternMask.
 *
 * @param other PatternMask to be copied.
 * @author L.Piccoli
 */
PatternMask &PatternMask::operator=(PatternMask &other) {
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _inclusionMask[i] = other._inclusionMask[i];
        _exclusionMask[i] = other._exclusionMask[i];
        _timeSlot = other._timeSlot;
    }
    return *this;
}

std::string PatternMask::toString() {
    std::ostringstream str;
    str << "PatternMask: [IncMask: ";
    for (int i = 0; i < 5; ++i) {
        str << std::hex << "0x" << _inclusionMask[i] << " ";
    }
    str << std::hex << "0x" << _inclusionMask[5];
    str << "; ExcMask: ";
    for (int i = 0; i < 5; ++i) {
        str << std::hex << "0x" << _exclusionMask[i] << " ";
    }
    str << std::hex << "0x" << _exclusionMask[5] << std::dec << "; ";
    str << "TS: " << std::dec << _timeSlot << "]";
    return str.str();
}

bool
PatternMask::destinationHXR()
{
    /*
     * If the destination is HXR it will either have BKRCUS in its exclusion
     * mask or BKRCUS will not be in its inclusion mask (or both).
     *
     * If a mask contains BKRCUS the bitwise and with the mask and BKRCUS will
     * be a non-zero (true) value. Likewise, if BKRCUS is not in the mask, the
     * bitwise and will be zero (false). We can just return the logical or of
     * these two bitwise operations to determine the destination this pattern
     * mask is configured for.
     */

    return ( _exclusionMask[MOD3_IDX] & BKRCUS ||
           !(_inclusionMask[MOD3_IDX] & BKRCUS) );
}

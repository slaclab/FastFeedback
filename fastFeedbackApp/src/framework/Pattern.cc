#include "Pattern.h"
#include "debugPrint.h"
#include "ExecConfiguration.h"

USING_FF_NAMESPACE

Pattern Pattern::ZERO_PATTERN;

/**
 * Struct constructor initializing all fields to zero
 *
 * @author L.Piccoli
 */
Pattern::Pattern() {
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _value[i] = 0;
    }
    _timestamp.secPastEpoch = 0;
    _timestamp.nsec = 0;
    _status = 0;
}

/**
 * Create a pattern that matches the specified mask.
 *
 * @param mask a patternMask that will match this pattern.
 * @author L.Piccoli
 */
Pattern::Pattern(const PatternMask& mask) {
    // The _value field must include all bits from the mask._inclusionMask
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _value[i] = mask._inclusionMask[i];
    }

    // Clear the bits set in the mask._exclusionMask
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _value[i] &= ~mask._exclusionMask[i];
    }

    _timestamp.secPastEpoch = 0;
    _timestamp.nsec = 0;
    _status = 0;
}

/**
 * Assignment constructor, all fields from the source Pattern
 * are copied to the new Pattern.
 *
 * @param other Pattern being copied.
 * @author L.Piccoli
 */
Pattern::Pattern(const Pattern & other) {
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _value[i] = other._value[i];
    }
    _timestamp.secPastEpoch = other._timestamp.secPastEpoch;
    _timestamp.nsec = other._timestamp.nsec;
    _status = other._status;
}

/**
 * Constructor specifying an evr received pattern and a timestamp.
 *
 * @param value initial value for the evr received pattern
 * @param timestamp when the pattern was received
 * @author L.Piccoli
 */
Pattern::Pattern(const evrModifier_ta &value, const epicsTimeStamp timestamp) {
    memcpy(&_value, &value, sizeof (evrModifier_ta));
    _timestamp.secPastEpoch = timestamp.secPastEpoch;
    _timestamp.nsec = timestamp.nsec;
}

/**
 * Check if 10 Hz bit is set in received pattern.
 *
 * @return true if 10 Hz is set, false otherwise
 * @author L.Piccoli
 */
bool Pattern::is10Hz() {
    return _value[4] & MOD5_10HZ_MASK;
}

/**
 * Check if 5 Hz bit is set in received pattern.
 *
 * @return true if 5 Hz is set, false otherwise
 * @author L.Piccoli
 */
bool Pattern::is5Hz() {
    return _value[4] & MOD5_5HZ_MASK;
}

/**
 * Check if 1 Hz bit is set in received pattern.
 *
 * @return true if 1 Hz is set, false otherwise
 * @author L.Piccoli
 */
bool Pattern::is1Hz() {
    return _value[4] & MOD5_1HZ_MASK;
}

/**
 * Check if 0.5 Hz bit is set in received pattern.
 *
 * @return true if 0.5 Hz is set, false otherwise
 * @author L.Piccoli
 */
bool Pattern::isHalfHz() {
    return _value[4] & MOD5_HALFHZ_MASK;
}

bool Pattern::isTs1() {
  return _value[3] & TIMESLOT1_MASK;
}

bool Pattern::isTs4() {
  return _value[3] & TIMESLOT4_MASK;
}

bool Pattern::hasBeam() {
    if (ExecConfiguration::getInstance()._lclsModePv.getValue()) {
        return true;
    }
    else {
        return _value[2] & POCKCEL_PERM;
    }
}

/**
 * Checks whether there is a match between this pattern and the specified mask.
 *
 * @param patternMask mask checked against this pattern
 * @return true if there is a match, false otherwise
 */
bool Pattern::match(PatternMask& patternMask) {
  //#ifdef RTEMS
    if (evrPatternCheck(0, patternMask._timeSlot, patternMask._inclusionMask,
            patternMask._exclusionMask, _value) == 0) {
        return false;
    }
    return true;
    //#else
    /*
    // On Linux a match means that bits from the inclusion masks are set
    // and bits from the exclusion mask are reset.
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        epicsUInt32 mask = patternMask._inclusionMask[i];
        if ((_value[i] & mask) != mask) {
            return false;
        }
        mask = patternMask._exclusionMask[i];
        if ((_value[i] & mask) != 0) {
            return false;
        }
    }

    return true;
#endif
    */
}

/**
 * Returns whether two Patterns are the same.
 *
 * The timestamps, status, inclusion/exclusion masks are *not* used in
 * this comparison.
 *
 * TODO: compare timestamps (?)
 *
 * @param other Pattern being compared to this one
 * @return true if _value is the same on both Patterns, false
 * otherwise
 * @author L.Piccoli
 */
bool Pattern::operator==(const Pattern & other) const {
    if (_value[0] == other._value[0] &&
            _value[1] == other._value[1] &&
            _value[2] == other._value[2] &&
            _value[3] == other._value[3] &&
            _value[4] == other._value[4] &&
            _value[5] == other._value[5]) {

        return true;
    }
    return false;
}

/**
 * Returns whether two Patterns are the same.
 *
 * The timestamps, status, inclusion/exclusion masks are *not* used in
 * this comparison.
 *
 * @param other evr pattern vector
 * @return true if _value is the same on both Patterns, false
 * otherwise
 * @author L.Piccoli
 */
bool Pattern::operator==(const std::vector<unsigned short> &other) const {
    if (other.size() > MAX_EVR_MODIFIER) {
        //        std::cerr << "ERROR: Pattern::operator==(vector) size > 6" << std::endl;
        return false;
    }
    if (_value[0] == other[0] &&
            _value[1] == other[1] &&
            _value[2] == other[2] &&
            _value[3] == other[3] &&
            _value[4] == other[4] &&
            _value[5] == other[5]) {

        return true;
    }
    return false;
}

/**
 * Returns whether two Patterns are different.
 * The == operator is being called for the comparison
 *
 * @param other Pattern being compared to this one
 * @return true if _value is different on both Patterns, false
 * otherwise
 * @author L.Piccoli
 */
bool Pattern::operator!=(const Pattern & other) const {

    return !(*this == other);
}

/**
 * This operator is defined in order to allow Patterns to be used
 * as Keys in std::map.
 *
 * @param other Pattern being compared to this one
 * @return true if this Pattern is smaller than the other Pattern,
 * false otherwise
 * @author L.Piccoli
 */
bool Pattern::operator<(const Pattern & other) const {
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        if (_value[i] > other._value[i]) {
            return false;
        } else if (_value[i] < other._value[i]) {

            return true;
        }
    }
    return false;
}

/**
 * Assignment operator specifying another pattern.
 *
 * @param other pattern to be copied.
 * @author L.Piccoli
 */
Pattern & Pattern::operator=(const Pattern & other) {
  memcpy(this, &other, sizeof(Pattern));
  /*
    for (int i = 0; i < MAX_EVR_MODIFIER; ++i) {
        _value[i] = other._value[i];
    }
    _timestamp.secPastEpoch = other._timestamp.secPastEpoch;
    _timestamp.nsec = other._timestamp.nsec;
    _status = other._status;
  */
    return *this;
}

epicsUInt32 Pattern::getPulseId() {
    return PULSEID(_timestamp);
}

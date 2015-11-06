/* 
 * File:   PatternGenerator.h
 * Author: lpiccoli
 *
 * Created on June 10, 2010, 2:40 PM
 */

#ifndef _PATTERNGENERATOR_H
#define	_PATTERNGENERATOR_H

#include <map>
#include <string>
#include "Defs.h"
#include "Pattern.h"
#include "PatternMask.h"
#include "Timer.h"

class PatternGeneratorTest;
class ExecConfigurationTest;

FF_NAMESPACE_START

/**
 * This class is used for testing only.
 *
 * Generates the fiducials after a predefined time delay. The fiducial contains
 * a pattern that is periodically repeated.
 *
 * This class contains a vector of pattern masks used to produce the sequence of
 * patterns for the testing system.
 *
 * @author L.Piccoli
 */
class PatternGenerator {
public:
    PatternGenerator();
    virtual ~PatternGenerator();

    Pattern next();
    void generate();

    int add(PatternMask patternMask);
    int remove(PatternMask patternMask);

    void start();
    void stop();

    int getGeneratedPatternCount() {
        return _generatedPatternCount;
    };

    bool isEnabled() {
        return _enabled;
    };

    void show();

    void setTimerMonitorCallback();

    static PatternGenerator &getInstance();

    friend class PatternGeneratorTest;
    friend class ExecConfigurationTest;

private:
    /** List of patterns to be generated */
    std::map<PatternMask, int> _patternMasks;

    /** Points to the next pattern to be generated */
    int _next;
    std::map<PatternMask, int>::iterator _nextPattern;

    /**
     *  Time delay in seconds between patterns - must be in seconds because
     * that is the resolution of the epicsTimer
     */
    double _delay;

    /** Timer used to generate the callbacks to produce patterns */
    Timer *_timer;

    /** Count how many times the generator has sent out a pattern */
    int _generatedPatternCount;

    /** Indicates whether the PatternGenerator is on or off */
    bool _enabled;

    /** There is only one PatternGenerator instance */
    static PatternGenerator _instance;
};

FF_NAMESPACE_END

#endif	/* _PATTERNGENERATOR_H */


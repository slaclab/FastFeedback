/* 
 * File:   PatternGenerator.cc
 * Author: lpiccoli
 * 
 * Created on June 10, 2010, 2:40 PM
 */

#include <fstream>
#include <iostream>

#include "PatternGenerator.h"
#include "PatternManager.h"

#include <cadef.h>

USING_FF_NAMESPACE

/**
 * Callback function invoked by the PatternGenerator timer.
 *
 * @author L.Piccoli
 */
void PatternGeneratorCallback(void *arg) {
    PatternGenerator::getInstance().generate();
}

/**
 * PatternGenerator singleton initialization
 */
PatternGenerator PatternGenerator::_instance;

/**
 * PatternGenerator constructor, initializes the timer
 *
 * @author L.Piccoli
 */
PatternGenerator::PatternGenerator() :
_next(0),
_delay(2),
_generatedPatternCount(0),
_enabled(true) {
    _timer = new Timer(&PatternGeneratorCallback);
    _nextPattern = _patternMasks.end();
}

/**
 * PatternGenerator destructor, deletes the timer.
 *
 * @author L.Piccoli
 */
PatternGenerator::~PatternGenerator() {
    delete _timer;
}

/**
 * Invokes the PatternManager passing the next pattern.
 *
 * @return the next pattern
 */
Pattern PatternGenerator::next() {
    if (_nextPattern == _patternMasks.end()) {
        _nextPattern = _patternMasks.begin();
    } else {
        ++_nextPattern;
        if (_nextPattern == _patternMasks.end()) {
            _nextPattern = _patternMasks.begin();
        }
    }

    Pattern pattern(_nextPattern->first);

    return pattern;
}

/**
 * This method is called by the _timer and is responsible for retrieving the
 * next pattern and send it to the PatternManager singleton.
 *
 * @author L.Piccoli
 */
void PatternGenerator::generate() {
    if (!_patternMasks.empty()) {
        Pattern pattern = next();
        PatternManager::getInstance().testFiducialCallback(pattern);
        _generatedPatternCount++;
    }
}

/**
 * Start the timer, allowing pattern to be generated at the rate defined
 * by the attribute _delay.
 * 
 * @author L.Piccoli
 */
void PatternGenerator::start() {
    if (_timer != NULL) {
        _timer->start(_delay, true);
        _enabled = true;
    }
}

/**
 * Stop the timer that generates patterns.
 *
 * @author L.Piccoli
 */
void PatternGenerator::stop() {
    _timer->cancel();
    _enabled = false;
}

/**
 * Return the PatternGenerator singleton
 *
 * @return the PatternGenerator
 * @author L.Piccoli
 */
PatternGenerator &PatternGenerator::getInstance() {
    return _instance;
}

int PatternGenerator::add(PatternMask patternMask) {
    std::map<PatternMask, int>::iterator it = _patternMasks.find(patternMask);
    if (it != _patternMasks.end()) {
        ++(it->second);
        return -1;
    } else {
        std::cout << "INFO: Adding pattern mask " << &patternMask << std::endl;
        _patternMasks.insert(std::pair<PatternMask, int>(patternMask, 1));
    }
    return 0;
}

int PatternGenerator::remove(PatternMask patternMask) {
    std::map<PatternMask, int>::iterator it = _patternMasks.find(patternMask);
    if (it != _patternMasks.end()) {
        --(it->second);
        if (it->second == 0) {
            _patternMasks.erase(it);
        }
        return 0;
    }
    return -1;
}

/**
 * Prints out information about the PatternGenerator.
 *
 * @author L.Piccoli
 */
void PatternGenerator::show() {
    std::cout << "  state: ";
    if (_enabled) {
        std::cout << "on" << std::endl;
    } else {
        std::cout << "off" << std::endl;
    }
    std::cout << "  count: " << _generatedPatternCount << std::endl;
    std::map<PatternMask, int>::iterator it;
    for (it = _patternMasks.begin(); it != _patternMasks.end(); ++it) {
        PatternMask pattern = it->first;
        std::cout << "  -> " << &pattern << std::endl;
    }
}

void PatternGenerator::setTimerMonitorCallback() {
    _timer->setCallback(&PatternGeneratorCallback);
}

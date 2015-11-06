/* 
 * File:   ExecConfiguration.cc
 * Author: lpiccoli
 * 
 * Created on June 10, 2010, 2:20 PM
 */

#include <iostream>
#include <fstream>

#include "ExecConfiguration.h"
#include "PatternGenerator.h"
#include "Algorithm.h"
#include "Log.h"

USING_FF_NAMESPACE

/**
 * ExecConfiguration singleton (It's initialized by the ExecThread::start())
 */
ExecConfiguration *EXEC_CONFIGURATION_INSTANCE;

ExecConfiguration::ExecConfiguration() :
_timerDelayPv("TIMERDELAY"), // Delay is in microseconds
_algorithmsAvailable("ALGORITHMS"),
_tmitLowPv("TMITLOW"),
_by1BdesPv("BY1BDES"),
_laserPowerReadbackPv("LASER_PWR_READBACK"),
_dl1ErefPv("DL1_EREF"),
_bc1ErefPv("BC1_EREF"),
_bc2ErefPv("BC2_EREF"),
_dl2ErefPv("DL2_EREF"),
_dl2EnLoloPv("LEM_DL2ENLOLO"),
_dl2EnHihiPv("LEM_DL2ENHIHI"),
_s29AsumPv("S29_ASUM") {
    std::vector<std::string> *algorithms = _algorithmsAvailable.getValueAddress();
    if (algorithms != NULL) {
        algorithms->push_back(NOOP_ALGORITHM);
        algorithms->push_back(SINE_ALGORITHM_N);
        algorithms->push_back(TRAJECTORY_FIT_ALGORITHM);
        algorithms->push_back(TRAJECTORY_FIT_BY1_ALGORITHM);
    } else {
        Log::getInstance() << "ERROR: Failed to register algorithms." << Log::flush;
    }

    _hasPatternGenerator = false;

    std::cout << ">> ExecConfiguration instance created." << std::endl;
};

/**
 * Create all data structures after the IOC is started. This prepares the
 * LoopConfiguration to be configured when the loops are turned on.
 *
 * At startup time all LoopThreads have the STATE set to OFF.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ExecConfiguration::initialize() {
    // Need to find out the number of Loops based on the PVs available, and
    // create one LoopConfiguration for each Loop found.
    std::vector<std::string> slots;
    if (getSlotNames(slots) != 0) {
        return -1;
    }

    std::cout << "--- Found " << slots.size() << " slots ----------------------"
            << std::endl;
    std::vector<std::string>::iterator slot;
    for (slot = slots.begin(); slot != slots.end(); ++slot) {
        std::cout << " -> " << *slot;
        LoopConfiguration *loopConfiguration = new LoopConfiguration(*slot);
        if (loopConfiguration->initialize() != 0) {
            std::cout << "... ERROR." << std::endl;
            return -1;
        }
        _loopConfigurations.insert(std::pair<std::string,
                LoopConfiguration *>(*slot, loopConfiguration));
        std::cout << "... done." << std::endl;
    }

    std::cout << "-----------------------------------------" << std::endl;

    // After creating or reconfiguring the LoopConfigurations then
    // create and start the LoopThreads -> they must have STATE=OFF

    // The configuration and reconfiguration actions have considerable
    // differences.

    // Use the pattern generator for now
    _hasPatternGenerator = true;

    return 0;
}

int ExecConfiguration::reconfigure() {
    return -1;
}

/**
 * Search for all defined Loop slot names in the system. This method is using
 * at startup time only and the output defines the number of LoopConfigurations
 * to be created.
 *
 * When the system is reconfigured the number of slots will remain the same,
 * therefore this method does not have to be invoked again.
 *
 * This method reads the names of all the $(LOOP):NAME devices from a file
 * generated at the ioc boot time:
 *
 *   $ dbgrep "*:NAME" > /tmp/loops.txt (on Linux)
 *   $ iocshCmd("dbgrep 'FBCK:*:NAME' > /data/config/loops.txt") (on RTEMS)
 *
 * @param slotNames output vector containing all loop slot names found
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ExecConfiguration::getSlotNames(std::vector<std::string> &slotNames) {
    std::ifstream file;
    std::string fileName;
#ifdef LINUX // On Linux the st.cmd creates the loops.txt file
    fileName = "/tmp/loops.txt";
#else // On RTEMS the file is read from the '/data' area
    fileName = "/data/config/loops.txt";
#endif

    file.open(fileName.c_str(), std::ifstream::in);

    if (file.fail()) {
        Log::getInstance() << "ERROR: failed to open file \""
                << fileName.c_str() << "\"" << Log::flush;
        return -1;
    }

    std::string pvName;
    std::string expectedSize = "FBCK:FB01:TR01:NAME";
    while (!file.fail() && !file.eof()) {
        file >> pvName;
        if (!file.eof()) {
            if (pvName.size() == expectedSize.size()) {
                std::string slot = pvName.substr(10, 4);
                slotNames.push_back(slot);
            }
        }
    }

    file.close();

    return 0;
}

/**
 * Clear the loop configurations, used for testing only
 * 
 * @author L.Piccoli
 */
void ExecConfiguration::clear() {
    _loopConfigurations.clear();
}

/**
 * Helper method to find a LoopConfiguration based on the name.
 *
 * @param loopName name for the loop being searched.
 * @return pointer to the LoopConfiguration or NULL if given loopName
 * is not registered
 * @author L.Piccoli
 */
LoopConfiguration *ExecConfiguration::getLoopConfiguration(std::string loopName) {
    LoopConfigurationMap::iterator it;
    it = _loopConfigurations.find(loopName);
    if (it == _loopConfigurations.end()) {
        return NULL;
    }
    return it->second;
}

/**
 * Return the ExecConfiguration singleton
 *
 * @return the ExecConfiguration
 * @author L.Piccoli
 */
ExecConfiguration &ExecConfiguration::getInstance() {
    return *EXEC_CONFIGURATION_INSTANCE;
}

/**
 * This method return an index for a given loopName. The index represents
 * the order in which the Loops where added to the configuration. This method
 * is used *only* by the device support code.
 *
 * @param loopName name of the loop to be searched
 * @return loop index, a number between 0 and number of loops - 1
 * @author L.Piccoli
 */
int ExecConfiguration::getLoopIndex(std::string loopName) {
    LoopConfigurationMap::iterator it;
    it = _loopConfigurations.begin();

    int i = 0;
    while (i < (int) _loopConfigurations.size()) {
        if (it->first == loopName) {
            return i;
        }
        ++i;
        ++it;
    }
    return -1;
}

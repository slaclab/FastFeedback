/* 
 * File:   ExecThread.cc
 * Author: lpiccoli
 * 
 * Created on June 10, 2010, 2:12 PM
 */

#include "ExecThread.h"
#include "ExecConfiguration.h"
#include "CollectorThread.h"
#include "PatternManager.h"
#include "PatternGenerator.h"
#include "Log.h"
#include <iocInit.h>
#include <sched.h>

USING_FF_NAMESPACE

/**
 * ExecThread singleton initialization
 */
ExecThread ExecThread::_instance;

/**
 * This method is for testing purposes only.
 *
 * Configure the ExecThread using options described in the specified
 * input file.
 *
 * @param fileName file with ExecConfiguration information
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ExecThread::configure(std::string fileName) {
    _configFileName = fileName;
    return 0;
}

int ExecThread::initialize() {
    // Configure the system based on PV configuration
    if (ExecConfiguration::getInstance().initialize() != 0) {
        Log::getInstance() << "ERROR: Failed to configure system (PV)" << Log::flush;
        return -1;
    }

    if (createLoopThreads() != 0) {
        Log::getInstance() << "ERROR: Failed to create LoopThreads" << Log::flush;
        return -1;
    }

    return 0;
}

/**
 * Create the LoopThreads based on information from the ExecConfiguration.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ExecThread::createLoopThreads() {
    LoopConfigurationMap::iterator it;
    for (it = ExecConfiguration::getInstance()._loopConfigurations.begin();
            it != ExecConfiguration::getInstance()._loopConfigurations.end();
            ++it) {
        LoopThread *loopThread = new LoopThread((*it).first, (*it).second);
        _loopThreads.insert(std::pair<std::string, LoopThread *>((*it).first,
                loopThread));

        // Add LoopThread to the list of receivers of HEARTBEAT_EVENTs
        PatternManager::getInstance().addHeartBeat(*loopThread);
    }
    return 0;
}

/**
 * Return the ExecThread singleton
 *
 * @return the ExecThread
 * @author L.Piccoli
 */
ExecThread &ExecThread::getInstance() {
    return _instance;
}

/**
 * The Thread::run() method invokes preRun() before getting into the main
 * event loop waiting for events.
 *
 * The FastFeedback configuration is performed by this method, which currently
 * only loads information from an input text file.
 *
 * If initialization fails, this method return -1, and the ExecThread exits
 * the run() method.
 *
 * @return 0 on success, -1 on failure.
 * @author L.Piccoli
 */
int ExecThread::preRun() {
  
#ifdef CONTROL_ACTUATORS	      
  std::cout << "**********************************************************************" << std::endl;
  std::cout << "*** The feedbacks running on this IOC will control actuators       ***" << std::endl;
  std::cout << "*** Software compiled with CONTROL_ACTUATORS flag - for production ***" << std::endl;
  std::cout << "**********************************************************************" << std::endl;
#else
  std::cout << "********************************************************************" << std::endl;
  std::cout << "*** The feedbacks running on this IOC will NOT control actuators ***" << std::endl;
  std::cout << "*** Software compiled without the CONTROL_ACTUATORS flag         ***" << std::endl;
  std::cout << "*** This version is safe for read-only feedback tests            ***" << std::endl;
  std::cout << "********************************************************************" << std::endl;
#endif
 
    // If the ExecConfiguration::_hasPatternGenerator is true, then start
    // Generating patterns -> This must be called here, after the ca_context
    // is created!
    if (ExecConfiguration::getInstance()._hasPatternGenerator) {
        std::cout << "--- Starting PatternGenerator ... ";
        PatternGenerator::getInstance().start();
        std::cout << "done." << std::endl;
    }

    if (initialize() != 0) {
        Log::getInstance() << "ERROR: Failed to initialize system." << Log::flush;
        return -1;
    }
    std::cout << "INFO: FastFeedback configured." << std::endl;

    sleep(5);

    // Lastly, let the fiducial callback start!
    PatternManager::getInstance().enable();

    _proceedWithIocInit = true;

    return 0;
}

/**
 * Starts the ExecThread and other threads in the system:
 * - LoopThreads
 * - CollectorThread
 *
 * Enables the PatternManager to respond to fiducial callbacks.
 *
 * If configured, enables the PatternGenerator (for testing only)
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
extern ExecConfiguration *EXEC_CONFIGURATION_INSTANCE;

int ExecThread::start() {
    EXEC_CONFIGURATION_INSTANCE = new ExecConfiguration();

    // Let the ExecThread run, which first calls preRun() that initializes
    // all data structures
    Thread::start();

    // After structures are ready, then iocInit() is called. The device support
    // code connects the PVs to the data structures initialized in the preRun()
    // method
    // First need to wait for preRun() to finish

    int counter = 0;
    while (counter < 30 && !_proceedWithIocInit) {
        sched_yield();
        sleep(1); // TODO: change sleep() to a blocking operation
        counter++;
    }

    if (_done) {
        return -1;
    }

    if (iocInit() != 0) {
        Log::getInstance() << "ERROR: failed on iocInit()." << Log::flush;
        return -1;
    }
    std::cout << "INFO: iocInit()." << std::endl;

    // Start all LoopThreads
    LoopThreadMap::iterator it;
    std::cout << "--- Starting LoopThreads ----------------" << std::endl;
    for (it = _loopThreads.begin(); it != _loopThreads.end(); ++it) {
        LoopThread *loopThread = it->second;
        std::cout << " -> " << loopThread->getName();
        loopThread->start();
        std::cout << " ...done." << std::endl;
    }
    std::cout << "-----------------------------------------" << std::endl;

    // Start the CollectorThread
    CollectorThread::getInstance().start();

    // Enable the PatternManager
    if (PatternManager::getInstance().configure() != 0) {
        return -1;
    }

    return 0;
}

/**
 * Stops all threads in the system in preparation to shutdown.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ExecThread::stop() {
    // Stop broadcasting patterns
    PatternManager::getInstance().disable();

    // If the ExecConfiguration::_hasPatternGenerator is true, then start
    // Generating patterns
    if (ExecConfiguration::getInstance()._hasPatternGenerator) {
        PatternGenerator::getInstance().stop();
    }

    // Send QUIT_EVENT to CollectorThread
    Event quitEvent(QUIT_EVENT);
    CollectorThread::getInstance().send(quitEvent);
#ifdef RTEMS
    sleep(2);
    sched_yield();
#endif
    CollectorThread::getInstance().join();

    // Send QUIT_EVENT to all LoopThreads
    LoopThreadMap::iterator it;
    for (it = _loopThreads.begin(); it != _loopThreads.end(); ++it) {
        LoopThread *loopThread = it->second;
        loopThread->send(quitEvent);
#ifdef RTEMS
        sleep(1);
        sched_yield();
#endif
        loopThread->join();
    }

    return 0;
}

/**
 * Send STATE=OFF to all loops.
 *
 * @return always 0
 * @author L.Piccoli
 */
int ExecThread::stopLoops() {
    Event modeEvent(RECONFIGURE_EVENT);
    LoopThreadMap::iterator it;
    for (it = _loopThreads.begin(); it != _loopThreads.end(); ++it) {
        LoopThread *loopThread = it->second;
        loopThread->disable();
        loopThread->send(modeEvent);
    }
    return 0;
}

/**
 * @return true if all Loops are OFF, false if at least one is ON
 */
bool ExecThread::areLoopsOff() {
    bool loopsOff = true;
    LoopThreadMap::iterator it;
    for (it = _loopThreads.begin(); it != _loopThreads.end(); ++it) {
        LoopThread *loopThread = it->second;
        // If at least one Loop is configured, then it is still running
        if (loopThread->isConfigured()) {
            loopsOff = false;
        }
    }
    return loopsOff;
}

int ExecThread::disconnectDevices() {
    Event disconnectEvent(DISCONNECT_EVENT);
    LoopThreadMap::iterator it;
    for (it = _loopThreads.begin(); it != _loopThreads.end(); ++it) {
        LoopThread *loopThread = it->second;
        loopThread->send(disconnectEvent);
    }
    return 0;
}

/**
 * Enable the LoopThread whose name is passed as parameter.
 *
 * @param loopName name of the LoopThread to be enabled
 * @return 0 if LoopThread was found and enabled, -1 if name is not registered.
 * @author L.Piccoli
 */
int ExecThread::enableLoop(std::string loopName) {
    LoopThreadMap::iterator it;
    it = _loopThreads.find(loopName);
    if (it == _loopThreads.end()) {
        return -1;
    }
    LoopThread *loopThread = it->second;
    loopThread->enable();
    return 0;
}

/**
 * Disable the LoopThread whose name is passed as parameter.
 *
 * @param loopName name of the LoopThread to be disabled
 * @return 0 if LoopThread was found and disabled, -1 if name is not registered.
 * @author L.Piccoli
 */
int ExecThread::disableLoop(std::string loopName) {
    LoopThreadMap::iterator it;
    it = _loopThreads.find(loopName);
    if (it == _loopThreads.end()) {
        return -1;
    }
    LoopThread *loopThread = it->second;
    loopThread->disable();
    return 0;
}

void ExecThread::show() {
  try {
    LoopThreadMap::iterator it;
    std::cout << "--- Exec Info ---" << std::endl;
    std::cout << "Release: ";
    if (FFCONTROLLER_VERSION.compare(1, 5, "Name:") == 0) {
        std::cout << "Development" << std::endl;
    } else {
        std::cout << FFCONTROLLER_VERSION << std::endl;
    }
    std::cout << "Timer delay: "
            << ExecConfiguration::getInstance()._timerDelayPv.getValue()
            << std::endl;
    std::cout << "Generated patterns: "
            << PatternGenerator::getInstance().getGeneratedPatternCount()
            << std::endl;
    std::cout << "Fiducial callbacks: "
            << PatternManager::getInstance().getFiductialCallbackCount()
            << std::endl;
    std::cout << "Low TMIT: "
            << ExecConfiguration::getInstance()._tmitLowPv.getValue()
            << std::endl;

    // CollectorThread
    std::cout << "--- CollectorThread: " << std::endl;
    CollectorThread::getInstance().show();

    std::cout << "--- Loops: " << std::endl;
    for (it = _loopThreads.begin(); it != _loopThreads.end(); ++it) {
        LoopThread *loopThread = it->second;
        std::cout << "   -> " << it->first << " - ";
        loopThread->isOn() ? std::cout << "on, " : std::cout << "off, ";
        loopThread->isActive() ? std::cout << "enabled" : std::cout << "compute";
	if (loopThread->isOn()) {
	  std::cout << " [" << loopThread->getRate() << " Hz]";
	}
        std::cout << std::endl;
    }

    // PatternManager
    std::cout << "--- PatternManager: " << std::endl;
    PatternManager::getInstance().show();

  } catch (std::exception& e) {
      std::cerr << "ffs command ERROR: " << e.what() << std::endl;
  }

}

void ExecThread::showDebug() {
  try {
    // PatternManager
    std::cout << "--- PatternManager: " << std::endl;
    PatternManager::getInstance().showDebug();

    // CollectorThread
    std::cout << "--- CollectorThread: " << std::endl;
    CollectorThread::getInstance().showDebug();
  } catch (std::exception& e) {
      std::cerr << "showDebug command ERROR: " << e.what() << std::endl;
  }
}

void ExecThread::showDebugLoop(std::string loopName) {
  try{
    std::cout << "--- " << loopName << " Debug Info ---" << std::endl;

    LoopThreadMap::iterator it;
    it = _loopThreads.find(loopName);

    if (it == _loopThreads.end()) {
        std::cout << "INFO: No loop named \"" << loopName << "\" found."
                << std::endl;
    } else {
        LoopThread *loopThread = it->second;
        loopThread->showDebug();
    }
  } catch (std::exception& e) {
      std::cerr << "showdebugloop command ERROR: " << e.what() << std::endl;
  }
}

void ExecThread::showLoop(std::string loopName) {
  try {
    std::cout << "--- " << loopName << " Info ---" << std::endl;

    LoopThreadMap::iterator it;
    it = _loopThreads.find(loopName);

    if (it == _loopThreads.end()) {
        std::cout << "INFO: No loop named \"" << loopName << "\" found."
                << std::endl;
    } else {
        LoopThread *loopThread = it->second;
        loopThread->show();
    }
  } catch (std::exception& e) {
      std::cerr << "showLoop command ERROR: " << e.what() << std::endl;
  }
}

void ExecThread::showLoopConfig(std::string loopName) {
  try {
    std::cout << "--- " << loopName << " Config Info ---" << std::endl;

    LoopThreadMap::iterator it;
    it = _loopThreads.find(loopName);

    if (it == _loopThreads.end()) {
        std::cout << "INFO: No loop named \"" << loopName << "\" found."
                << std::endl;
    } else {
        LoopThread *loopThread = it->second;
        loopThread->showConfig();
    }
  } catch (std::exception& e) {
      std::cerr << "showLoopConfig command ERROR: " << e.what() << std::endl;
  }
}

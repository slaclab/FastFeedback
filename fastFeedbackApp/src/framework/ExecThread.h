/* 
 * File:   ExecThread.h
 * Author: lpiccoli
 *
 * Created on June 10, 2010, 2:12 PM
 */

#ifndef _EXECTHREAD_H
#define	_EXECTHREAD_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <epicsMutex.h>
#include "Thread.h"
#include "LoopThread.h"
#include "Defs.h"
#include "LoopConfiguration.h"

//class ExecThreadTest;

FF_NAMESPACE_START

typedef std::map<std::string, LoopThread *> LoopThreadMap;

/**
 * The ExecThread is responsible for orchestrating the whole FastFeedback IOC.
 *
 * It controls all threads in the system, setting their priorities accordingly.
 *
 * Commands issued from the shell command line create events handled by the
 * ExecThread. For example, to start/stop a loop a command line function creates
 * a event and sent to the ExecThread by invoking ExecThread::send(event).
 *
 * The ExecThread handles a few PVs, through the ExecConfiguration singleton:
 *
 * - STATE: Controls the LoopThread, if set to 1 the LoopThread is started, if
 *          set to 0 a QUIT_EVENT is sent to the LoopThread. (binary)
 * - TIMERDELAY: Delay time after fiducial before starting collecting
 *               measurements. This delay is used by all Loops. (double)
 *  
 * @author L.Piccoli
 */
class ExecThread : public Thread {
public:

    ExecThread() :
    Thread("ExecThread", 77),
    _configFileName(""),
    _proceedWithIocInit(false) {
    };

    virtual ~ExecThread() {
    };

    int configure(std::string fileName);
    int initialize();
    int createLoopThreads();

    virtual int preRun();

    int start();
    int stop();
    int stopLoops();
    bool areLoopsOff();
    void show();
    void showDebug();
    void showDebugLoop(std::string loopName);
    void showLoop(std::string loopName);
    void showLoopConfig(std::string loopName);

    int enableLoop(std::string loopName);
    int disableLoop(std::string loopName);

    int disconnectDevices();

    static ExecThread &getInstance();
    void setPatternGenMode(bool patternGenMode);

    friend class ExecThreadTest;

private:
    //    int configureLoopThread(std::string name, LoopConfiguration *configuration);
    //    int registerPatterns(LoopThread *loopThread, std::vector<Pattern> patterns);

    /** Map of LoopThreads, keyed by the Loop name */
    LoopThreadMap _loopThreads;

    /** There is only one ExecThread instance */
    static ExecThread _instance;

    /** Holds name of the configuration file -> TEST ONLY */
    std::string _configFileName;

    /** Mutex used to initialize system on RTEMS */
    epicsMutex initMutex;

    /** Flags that iocInit() can proceed -> used at statup time only */
    bool _proceedWithIocInit;
    
    bool _hasPatternGenerator;
};

FF_NAMESPACE_END

#endif	/* _EXECTHREAD_H */


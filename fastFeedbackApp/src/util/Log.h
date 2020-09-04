/*
 * File:   Log.h
 * Author: lpiccoli
 *
 * Created on September 28, 2010, 2:31 PM
 */

#ifndef _LOG_H
#define	_LOG_H

#include "Defs.h"
#include "PvData.h"
#include "debugPrint.h"
#include <string>
#include <sstream>
#include <epicsMutex.h>
#include "time.h"

FF_NAMESPACE_START

/**
 * This class is a wrapper for the errlogPrintfNoConsole().
 *
 * It automatically prepends tags for the loop name (fac tag) and slot name
 * (process tag) to the message string.
 *
 * There is one static object Log that can be used for messages not
 * related to a feedback loop.
 *
 * This class is *not* thread safe!
 *
 * @author L.Piccoli
 */
class Log {
public:
    /**
     * Possible Log operations. These must be added at the end of a sequence
     * of << operators:
     * o cout: output using std::cout
     * o flush: message is sent to Log only
     * o flushpv: message is written to the STATUSSTR PV besides going to the Log
     * o flushpvnoalarm: message is written to the STATUSSTR PV besides going to the Log, no alarm set
     * o flushpvonly: message is written to STATUSSTR PV only
     * o flushpvonlynoalarm: message is written to STATUSSTR PV only, no alarm set
     * o clearpv: erase status message in STATUSSTR PV
     */
    enum LogOperatorType {
        cout,
        flush,
        flushpv,
	flushpvnoalarm,
        flushpvonly,
	flushpvonlynoalarm,
        clearpv,
	dp,
	showtime,
    };

    enum LogDebugPrintLevel {
      dpNone = DP_NONE,
      dpFatal = DP_FATAL,
      dpError = DP_ERROR,
      dpWarn = DP_WARN,
      dpInfo = DP_INFO,
      dpDebug = DP_DEBUG
    };

    enum LogDebugFlag {
      flagPulseId,
      flagEvent,
      flagCa,
      flagConfig,
      flagAlgo,
      flagFbckPv,
      flagOffset,
      flagBuffer,
    };

    Log(std::string loopName = "Fbck", std::string slotName = "Fbck");

    Log &operator<<(char const *message);
    Log &operator<<(int const value);
    Log &operator<<(long int const value);
    Log &operator<<(double const value);
    Log &operator<<(LogOperatorType const operation);
    Log &operator<<(LogDebugPrintLevel const debugLevel);
    Log &operator<<(LogDebugFlag const debugFlag);
    
    void debugPrint();
    void flushMessage();
    void clear();

    void logToConsole(bool log = true);
    void setLoopName(std::string loopName);
    void setSlotName(std::string slotName);
    void setAllLogToConsole(bool log = true);
    void setStatusstrPv(PvData<std::string> *pvCharWaveform);

    void setDebugLevel(int flag, int level);

    std::string getLastMessage();
    
    static Log &getInstance();

private:
    void setStatusStrPv(bool alarm = true);
    void clearStatusPv();

    /** The message */
    std::ostringstream _message;

    /** The last printed message :) */
    std::string _lastMessage;

    /** Header sent to the logger */
    std::string _messageHeader;

    /** Loop name */
    std::string _loopName;

    /** Slot name */
    std::string _slotName;

    /**
     * Defines whether messages are logged only or are printed out to the
     * console
     */
    bool _logToConsole;

    /**
     * Pointer to the STATUSSTR PV if this Log instance is used for a particular
     * feedback loop.
     */
    //    PvDataString *_statusStrPv;
    PvData<std::string> *_statusStrPv;

    /**
     * Static variable that defines if *all* logs should print out to
     * the console.
     */
    static bool _allLogToConsole;

    /** Defines which DEBUGPRINT flag is to be used when the operator dp is used */
    LogDebugFlag _dpFlag;

    /** Defines which DEBUGPRINT leves is to be used when the operator dp is used */
    LogDebugPrintLevel _dpLevel;

    /**
     * The same Log may be shared among different tasks, specially
     * the _instance singleton. We need a mutex to avoid any conflicts!
     */
    epicsMutex _mutex;

    /** Timestamp used to stamp messages */
    epicsTimeStamp _timestamp;

    /** String that holds the converted epicsTimeStamp */
    char _timeString[256];


    /** Time is seconds of the last alarm set */
    time_t _alarmTime;
};

FF_NAMESPACE_END

#endif

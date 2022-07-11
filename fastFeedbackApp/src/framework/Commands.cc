#include "iocsh.h"
#include "Commands.h"
#include "ExecThread.h"
#include "ExecConfiguration.h"
#include "PatternGenerator.h"
#include "ChannelAccess.h"
#include "CaChannel.h"
#include "Log.h"
#include "EventLogger.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#ifdef RTEMS
#include <bsp.h>
#endif

extern int DEBUG_PULSEID_FLAG;

USING_FF_NAMESPACE

        /** ffHelp ********************************************************************/
        static const iocshArg ffHelp_Arg0 = {"command", iocshArgString};
static const iocshArg * const ffHelp_Args[1] = {&ffHelp_Arg0};
static const iocshFuncDef ffHelp_FuncDef = {"ffHelp", 0, NULL};

typedef std::map<std::string, std::string> HelpMap;
static HelpMap *helpMap;

static void Help(const iocshArgBuf *args) {
    std::cout << "FastFeedback available commands:" << std::endl;
    HelpMap::iterator it;
    for (it = helpMap->begin(); it != helpMap->end(); ++it) {
        std::cout << "  " << it->first << std::endl;
    }

    std::cout << "Type a command: ";
    std::string command;
    std::cin >> command;

    it = helpMap->find(command);
    if (it == helpMap->end()) {
        std::cout << "Unknown command \"" << command << "\"" << std::endl;
    } else {
        std::cout << "----------------------------------------------------------";
        std::cout << std::endl;
        std::cout << it->second;
        std::cout << "----------------------------------------------------------";
        std::cout << std::endl;
    }
}

static void HelpCommand(std::string command) {
    HelpMap::iterator it;
    it = helpMap->find(command);
    if (it == helpMap->end()) {
        std::cout << "Unknown command \"" << command << "\"" << std::endl;
    } else {
        std::cout << "----------------------------------------------------------";
        std::cout << std::endl;
        std::cout << it->second;
        std::cout << "----------------------------------------------------------";
        std::cout << std::endl;
    }
}

void IocshRegister::registerHelp() {
    helpMap = new HelpMap();
    std::pair<std::string, std::string> command;
    command.first = "ffDebug";
    command.second = "Usage: ffDebug (or ffd) <LoopName>\n";
    command.second += "  Show system debugging information.\n";
    command.second += " Where:\n";
    command.second += "  LoopName - (optional parameter) name of the loop (see ffShow)\n";
    helpMap->insert(command);

    command.first = "ffHelp";
    command.second = "Usage: ffHelp";
    command.second = "  Describes available FastFeedback (ff) commands.\n";
    helpMap->insert(command);

    command.first = "ffLogOff";
    command.second = "Usage: ffLogOff\n";
    command.second += "  Disables error messages to the console.\n";
    helpMap->insert(command);

    command.first = "ffLogOn";
    command.second = "Usage: ffLogOn\n";
    command.second += "  Enables error messages to the console.\n";
    helpMap->insert(command);

    command.first = "ffLogLevel";
    command.second = "Usage: ffLogLevel (or ffll) <flag> <level>\n";
    command.second += "  flag:\n";
    command.second += "    0 - PULSEID\n";
    command.second += "    1 - EVENT\n";
    command.second += "    2 - CHANNEL ACCESS\n";
    command.second += "    3 - LOOP CONFIG\n";
    command.second += "    4 - ALGORITHM\n";
    command.second += "    5 - FEEDBACK PV\n";
    command.second += "  level:\n";
    command.second += "    0 - DP_NONE\n";
    command.second += "    1 - DP_FATAL\n";
    command.second += "    2 - DP_ERROR\n";
    command.second += "    3 - DP_WARN\n";
    command.second += "    4 - DP_INFO\n";
    command.second += "    5 - DP_DEBUG\n";
    helpMap->insert(command);

    command.first = "ffPeek";
    command.second = "Usage: ffPeek (or ffp) <LoopName> <DeviceName> <PatternIndex>\n";
    command.second += "  Prints the lastest value used as input or output for the loop.\n";
    command.second += " Where:\n";
    command.second += "  LoopName - name of the loop (see ffShow)\n";
    command.second += "  DeviceName - (see ffShowDevices <LoopName>)\n";
    command.second += "  PatternIndex - pattern number for the loop (see ffShow)\n";
    helpMap->insert(command);

    command.first = "ffReboot";
    command.second = "Usage: ffReboot\n";
    command.second += "  Stops all running loops and reboots the IOC by\n";
    command.second += "  invoking bsp_reset().\n";
    helpMap->insert(command);

    command.first = "ffShowDevices";
    command.second = "Usage: ffShowDevices (or ffsd) <LoopName>\n";
    command.second += "  Display device information for the specified loop.\n";
    helpMap->insert(command);

    command.first = "ffShowEvents";
    command.second = "Usage: ffShowEvents (or ffse)\n";
    command.second += "  Show all events recorded in the system.\n";
    helpMap->insert(command);

    command.first = "ffShow";
    command.second = "Usage: ffShow (or ffs) <LoopName>\n";
    command.second += "  Shows general information about the FastFeedback system\n";
    command.second += "  or about the specified loop.\n";
    command.second += " Where:\n";
    command.second += "  LoopName - (optional parameter) name of the loop (see ffShow)\n";
    helpMap->insert(command);

    command.first = "ffShowLoopConfig";
    command.second = "Usage: ffShowLoopConfig (or ffslc) <LoopName>\n";
    command.second += "  Shows detailed configuration information about the given loop.\n";
    helpMap->insert(command);

    command.first = "ffShowMeas";
    command.second = "Usage: ffShowMeas (or ffsm)\n";
    command.second += "  Shows detailed information about current collected\n";
    command.second += "  measurements.\n";
    helpMap->insert(command);

    command.first = "ffShowPv";
    command.second = "Usage: ffShowPv (or ffsp)\n";
    command.second += "  Shows PvData information.\n";
    helpMap->insert(command);

    command.first = "ffStart";
    command.second = "Usage: ffStart\n";
    command.second += "  Starts the FastFeedback system using configuration from.\n";
    command.second += "  loaded with the ffConfig command.\n";
    helpMap->insert(command);

    command.first = "ffStopLoops";
    command.second = "Usage: ffStopLoops (or ffsl)\n";
    command.second += "  Turns STATE=OFF to all loops. This releases all Fcom\n";
    command.second += "  and ChannelAccess connections.\n";
    helpMap->insert(command);

    command.first = "ffPatternGenMode";
    command.second = "Usage: ffPatternMode <mode>\n";
    command.second += "  Sets the pattern generator mode.\n";
    command.second += "  Mode:\n";
    command.second += "    0 - Disabled\n";
    command.second += "    1 - Enabled\n";
    helpMap->insert(command);
}

/** ffReboot ******************************************************************/
static const iocshFuncDef ffReboot_FuncDef = {"ffReboot", 0, NULL};

static void ExecThreadReboot(const iocshArgBuf *args) {
#ifdef RTEMS
    std::cout << "Stopping all feedback loops..." << std::flush;
    ExecThread::getInstance().stopLoops();
    // Must wait until LoopConfiguration::_configured == false for all Loops
    while (!ExecThread::getInstance().areLoopsOff()) {
        epicsThreadSleep(1);
	std::cout << "." << std::flush;
    }
    std::cout << " done. Rebooting..." << std::endl << std::flush;
    epicsThreadSleep(1);
    bsp_reset();
#endif
}

/** Allow ffStart to be invoked from cexp shell */
extern "C" {

    void ffReboot() {
        ExecThreadReboot(0);
    }
}

/** ffStart *******************************************************************/
static const iocshFuncDef ffStart_FuncDef = {"ffStart", 0, NULL};

static void ExecThreadStart(const iocshArgBuf *args) {
    try {
        ExecThread::getInstance().start();
    } catch (...) {
        std::cerr << "ERROR: failed to start FastFeedback system."
                << std::endl;
    }
}

/** Allow ffStart to be invoked from cexp shell */
extern "C" {

    void ffStart() {
        ExecThreadStart(0);
    }
}

/** ffStop ********************************************************************/
static const iocshFuncDef ffStopLoops_FuncDef = {"ffStopLoops", 0, NULL};
static const iocshFuncDef ffsl_FuncDef = {"ffsl", 0, NULL};

static void ExecThreadStopLoops(const iocshArgBuf *args) {
    ExecThread::getInstance().stopLoops();
}

/** Allow ffStop to be invoked from cexp shell */
extern "C" {

    void ffStopLoops() {
        ExecThreadStopLoops(0);
    }
}

/** ffLogOn *******************************************************************/
static const iocshFuncDef ffLogOn_FuncDef = {"ffLogOn", 0, NULL};

static void ConsoleLogEnable(const iocshArgBuf *args) {
    Log::getInstance().setAllLogToConsole(true);
}

/** ffLogOff ******************************************************************/
static const iocshFuncDef ffLogOff_FuncDef = {"ffLogOff", 0, NULL};

static void ConsoleLogDisable(const iocshArgBuf *args) {
    Log::getInstance().setAllLogToConsole(false);
}

/** ffLogLevel ********************************************************************/
static const iocshArg ffLogLevel_Arg0 = {"Flag", iocshArgInt};
static const iocshArg ffLogLevel_Arg1 = {"Level", iocshArgInt};
static const iocshArg * const ffLogLevel_Args[3] = {&ffLogLevel_Arg0, &ffLogLevel_Arg1};
static const iocshFuncDef ffLogLevel_FuncDef = {"ffLogLevel", 2, ffLogLevel_Args};
static const iocshFuncDef ffll_FuncDef = {"ffll", 2, ffLogLevel_Args};

static void LoopLogLevel(const iocshArgBuf *args) {
    int flag = args[0].ival;
    int level = args[1].ival;

    Log::getInstance().setDebugLevel(flag, level);
}

/** ffPeek ********************************************************************/
static const iocshArg ffPeek_Arg0 = {"LoopName", iocshArgString};
static const iocshArg ffPeek_Arg1 = {"DeviceName", iocshArgString};
static const iocshArg ffPeek_Arg2 = {"PatternIndex", iocshArgInt};
static const iocshArg * const ffPeek_Args[3] = {&ffPeek_Arg0, &ffPeek_Arg1, &ffPeek_Arg2};
static const iocshFuncDef ffPeek_FuncDef = {"ffPeek", 3, ffPeek_Args};
static const iocshFuncDef ffp_FuncDef = {"ffp", 3, ffPeek_Args};

static void LoopPeek(const iocshArgBuf *args) {
    if (args[0].sval == NULL || args[1].sval == NULL) {
        HelpCommand("ffPeek");
        return;
    }

    std::string loopName(args[0].sval);
    std::transform(loopName.begin(), loopName.end(), loopName.begin(), toupper);

    std::string deviceName(args[1].sval);
    std::transform(deviceName.begin(), deviceName.end(), deviceName.begin(), toupper);

    int patternIndex = args[2].ival;

    LoopConfigurationMap::iterator it;
    it = ExecConfiguration::getInstance()._loopConfigurations.find(loopName);
    LoopConfiguration *loopConfig = it->second;
    if (it != ExecConfiguration::getInstance()._loopConfigurations.end()) {
        if (patternIndex >= (int) loopConfig->_patternMasks.size()) {
            std::cout << "ERROR: invalid PatternIndex, value must be between 0 and "
                    << loopConfig->_patternMasks.size() - 1 << std::endl;
            return;
        }
        PatternMask pattern = loopConfig->_patternMasks[patternIndex];

        // Look for the desired DeviceName/Pattern
	DataPoint dataPoint;

	if (loopConfig->peek(deviceName, pattern, dataPoint) != 0) {
            std::cout << "ERROR: invalid device." << std::endl;
            return;
        }

	char timeStr[200];
	epicsTimeToStrftime(timeStr, 200, "%Y/%m/%d %H:%M:%S.%06f",
			    &dataPoint._timestamp);

        std::cout << dataPoint._value << " at " << timeStr
		  << ", pulse id: " << PULSEID(dataPoint._timestamp)
		  << " (status=" << dataPoint._status << ")" << std::endl;
    } else {
        std::cout << "ERROR: unknown loop named \"" << args[0].sval << "\""
                << std::endl;
    }
}

/** ffPeekPos ********************************************************************/
static const iocshArg ffPeekPos_Arg0 = {"LoopName", iocshArgString};
static const iocshArg ffPeekPos_Arg1 = {"DeviceName", iocshArgString};
static const iocshArg ffPeekPos_Arg2 = {"PatternIndex", iocshArgInt};
static const iocshArg ffPeekPos_Arg3 = {"Index", iocshArgInt};
static const iocshArg * const ffPeekPos_Args[4] = {&ffPeekPos_Arg0, &ffPeekPos_Arg1,
						   &ffPeekPos_Arg2, &ffPeekPos_Arg3};
static const iocshFuncDef ffPeekPos_FuncDef = {"ffPeekPos", 4, ffPeekPos_Args};
static const iocshFuncDef ffpp_FuncDef = {"ffpp", 3, ffPeekPos_Args};

static void LoopPeekPos(const iocshArgBuf *args) {
    if (args[0].sval == NULL || args[1].sval == NULL) {
        HelpCommand("ffPeekPos");
        return;
    }

    std::string loopName(args[0].sval);
    std::transform(loopName.begin(), loopName.end(), loopName.begin(), toupper);

    std::string deviceName(args[1].sval);
    std::transform(deviceName.begin(), deviceName.end(), deviceName.begin(), toupper);

    int patternIndex = args[2].ival;

    int pos = args[3].ival;

    LoopConfigurationMap::iterator it;
    it = ExecConfiguration::getInstance()._loopConfigurations.find(loopName);
    LoopConfiguration *loopConfig = it->second;
    if (it != ExecConfiguration::getInstance()._loopConfigurations.end()) {
        if (patternIndex >= (int) loopConfig->_patternMasks.size()) {
            std::cout << "ERROR: invalid PatternIndex, value must be between 0 and "
                    << loopConfig->_patternMasks.size() - 1 << std::endl;
            return;
        }
        PatternMask pattern = loopConfig->_patternMasks[patternIndex];

        // Look for the desired DeviceName/Pattern
	DataPoint dataPoint;

	if (loopConfig->peek(deviceName, pattern, dataPoint, pos) != 0) {
            std::cout << "ERROR: invalid device." << std::endl;
            return;
        }

	char timeStr[200];
	epicsTimeToStrftime(timeStr, 200, "%Y/%m/%d %H:%M:%S.%06f",
			    &dataPoint._timestamp);

        std::cout << dataPoint._value << " at " << timeStr
		  << ", pulse id: " << PULSEID(dataPoint._timestamp)
		  << " (status=" << dataPoint._status << ")" << std::endl;
    } else {
        std::cout << "ERROR: unknown loop named \"" << args[0].sval << "\""
                << std::endl;
    }
}

/** ffShow ********************************************************************/
/*
static const iocshFuncDef ffShow_FuncDef = {"ffShow", 0, NULL};
static const iocshFuncDef ffs_FuncDef = {"ffs", 0, NULL};

static void ExecThreadShow(const iocshArgBuf *args) {
    ExecThread::getInstance().show();
}

extern "C" {

    void ffShow() {
        ExecThreadShow(0);
    }
}
*/
/** ffDebug *******************************************************************/
static const iocshArg ffDebug_Arg0 = {"LoopName", iocshArgString};
static const iocshArg * const ffDebug_Args[1] = {&ffDebug_Arg0};
static const iocshFuncDef ffDebug_FuncDef = {"ffDebug", 1, ffDebug_Args};
static const iocshFuncDef ffd_FuncDef = {"ffd", 1, ffDebug_Args};

static void ExecThreadShowDebug(const iocshArgBuf *args) {
    if (args[0].sval != NULL) {
        std::string loopName(args[0].sval);
        std::transform(loopName.begin(), loopName.end(), loopName.begin(), toupper);
        ExecThread::getInstance().showDebugLoop(loopName);
    } else {
      ExecThread::getInstance().showDebug();
    }
}

extern "C" {

    void ffDebug() {
        ExecThreadShowDebug(0);
    }
}

/** ffShowLoop ****************************************************************/
static const iocshArg ffShowLoop_Arg0 = {"LoopName", iocshArgString};
static const iocshArg * const ffShowLoop_Args[1] = {&ffShowLoop_Arg0};
static const iocshFuncDef ffShow_FuncDef = {"ffShow", 1, ffShowLoop_Args};
static const iocshFuncDef ffs_FuncDef = {"ffs", 1, ffShowLoop_Args};

static void ExecThreadShowLoop(const iocshArgBuf *args) {
    if (args[0].sval != NULL) {
        std::string loopName(args[0].sval);
        std::transform(loopName.begin(), loopName.end(), loopName.begin(), toupper);
        ExecThread::getInstance().showLoop(loopName);
    } else {
      ExecThread::getInstance().show();
    }
}

extern "C" {

    void ffShow(char *loopName) {
        iocshArgBuf args;
        args.sval = loopName;
        ExecThreadShowLoop(&args);
    }
}

/** ffShowLoopConfig **********************************************************/
static const iocshArg ffShowLoopConfig_Arg0 = {"LoopName", iocshArgString};
static const iocshArg * const ffShowLoopConfig_Args[1] = {&ffShowLoopConfig_Arg0};
static const iocshFuncDef ffShowLoopConfig_FuncDef = {"ffShowLoopConfig", 1, ffShowLoopConfig_Args};
static const iocshFuncDef ffslc_FuncDef = {"ffslc", 1, ffShowLoopConfig_Args};

static void ExecThreadShowLoopConfig(const iocshArgBuf *args) {
    if (args[0].sval != NULL) {
        std::string loopName(args[0].sval);
        std::transform(loopName.begin(), loopName.end(), loopName.begin(), toupper);
        ExecThread::getInstance().showLoopConfig(loopName);
    } else {
        HelpCommand("ffShowLoopConfig");
    }
}

extern "C" {

    void ffShowLoopConfig(char *loopName) {
        iocshArgBuf args;
        args.sval = loopName;
        ExecThreadShowLoopConfig(&args);
    }
}

/** ffShowMeas ****************************************************************/
static const iocshFuncDef ffShowMeas_FuncDef = {"ffShowMeas", 0, NULL};
static const iocshFuncDef ffsm_FuncDef = {"ffsm", 0, NULL};

static void MeasurementCollectorShowMeas(const iocshArgBuf *args) {
    MeasurementCollector::getInstance().showMeasurements();
}

/** ffShowPv ****************************************************************/
static const iocshFuncDef ffShowPv_FuncDef = {"ffShowPv", 0, NULL};
static const iocshFuncDef ffsp_FuncDef = {"ffsp", 0, NULL};

static void PvDataShow(const iocshArgBuf *args) {
  std::cout << "=== PvData<char>:" << std::endl;
  PvData<char>::showMap();
  std::cout << "=== PvData<unsigned char>:" << std::endl;
  PvData<unsigned char>::showMap();
  std::cout << "=== PvData<int>:" << std::endl;
  PvData<int>::showMap();
  std::cout << "=== PvData<unsigned int>:" << std::endl;
  PvData<unsigned int>::showMap();
  std::cout << "=== PvData<long>:" << std::endl;
  PvData<long>::showMap();
  std::cout << "=== PvData<unsigned long>:" << std::endl;
  PvData<unsigned long>::showMap();
  std::cout << "=== PvData<double>:" << std::endl;
  PvData<double>::showMap();
  std::cout << "=== PvData<short>:" << std::endl;
  PvData<short>::showMap();
  std::cout << "=== PvData<unsigned short>:" << std::endl;
  PvData<unsigned short>::showMap();
  std::cout << "=== PvData<std::string>:" << std::endl;
  PvData<std::string>::showMap();
  std::cout << "=== PvData<bool>:" << std::endl;
  PvData<bool>::showMap();
  std::cout << "=== PvDataWaveform<double>:" << std::endl;
  PvDataWaveform<double>::showMap();
  std::cout << "=== PvDataWaveform<unsigned short>:" << std::endl;
  PvDataWaveform<unsigned short>::showMap();
  std::cout << "=== PvDataWaveform<std::string>:" << std::endl;
  PvDataWaveform<std::string>::showMap();
}

/** ffShowDevices *************************************************************/
static const iocshArg ffShowDevices_Arg0 = {"LoopName", iocshArgString};
static const iocshArg * const ffShowDevices_Args[1] = {&ffShowDevices_Arg0};
static const iocshFuncDef ffShowDevices_FuncDef = {"ffShowDevices", 1, ffShowDevices_Args};
static const iocshFuncDef ffsd_FuncDef = {"ffsd", 1, ffShowDevices_Args};

static void LoopShowDevices(const iocshArgBuf *args) {
    std::string loopName;
    if (args[0].sval != NULL) {
        loopName = args[0].sval;
        std::transform(loopName.begin(), loopName.end(), loopName.begin(), toupper);
    } else {
        HelpCommand("ffShowDevices");
    }

    LoopConfigurationMap::iterator it;
    it = ExecConfiguration::getInstance()._loopConfigurations.find(loopName);
    if (it != ExecConfiguration::getInstance()._loopConfigurations.end()) {
        it->second->showDevices();
    } else {
        std::cout << "ERROR: unknown loop named \"" << loopName << "\""
                << std::endl;
    }
}

/** ffShowEvents *************************************************************/
static const iocshFuncDef ffShowEvents_FuncDef = {"ffShowEvents", 0, NULL};
static const iocshFuncDef ffse_FuncDef = {"ffse", 0, NULL};

static void LoopShowEvents(const iocshArgBuf *args) {
  EventLogger::getInstance().dump();
}

/** ffPatternGenMode *************************************************************/
static const iocshArg ffPatternGenMode_Arg0 = {"Mode (0-Disabled, 1-Enabled)", iocshArgInt};
static const iocshArg * const ffPatternGenMode_Args[1] = {&ffPatternGenMode_Arg0};
static const iocshFuncDef ffPatternGenMode_FuncDef = {"ffPatternGenMode", 1, ffPatternGenMode_Args};

static void ExecThreadSetPatternGenMode(const iocshArgBuf *args) {

    std::cout << "Setting Pattern Gen Mode to Specified\n";
    bool patternGenMode = args[0].ival;

    ExecThread::getInstance().setPatternGenMode(patternGenMode);
}


/** Register all commands */
IocshRegister::IocshRegister() {
    registerHelp();

    iocshRegister(&ffDebug_FuncDef, ExecThreadShowDebug);
    iocshRegister(&ffd_FuncDef, ExecThreadShowDebug);

    iocshRegister(&ffStart_FuncDef, ExecThreadStart);

    iocshRegister(&ffReboot_FuncDef, ExecThreadReboot);

    iocshRegister(&ffStopLoops_FuncDef, ExecThreadStopLoops);
    iocshRegister(&ffsl_FuncDef, ExecThreadStopLoops);

    iocshRegister(&ffLogOn_FuncDef, ConsoleLogEnable);
    iocshRegister(&ffLogOff_FuncDef, ConsoleLogDisable);

    iocshRegister(&ffLogLevel_FuncDef, LoopLogLevel);
    iocshRegister(&ffll_FuncDef, LoopLogLevel);

    iocshRegister(&ffPeek_FuncDef, LoopPeek);
    iocshRegister(&ffp_FuncDef, LoopPeek);

    iocshRegister(&ffPeekPos_FuncDef, LoopPeekPos);
    iocshRegister(&ffpp_FuncDef, LoopPeekPos);

    iocshRegister(&ffShow_FuncDef, ExecThreadShowLoop);
    iocshRegister(&ffs_FuncDef, ExecThreadShowLoop);

    iocshRegister(&ffShowEvents_FuncDef, LoopShowEvents);
    iocshRegister(&ffse_FuncDef, LoopShowEvents);

    //    iocshRegister(&ffShowLoop_FuncDef, ExecThreadShowLoop);
    //    iocshRegister(&ffsl_FuncDef, ExecThreadShowLoop);

    iocshRegister(&ffShowLoopConfig_FuncDef, ExecThreadShowLoopConfig);
    iocshRegister(&ffslc_FuncDef, ExecThreadShowLoopConfig);

    iocshRegister(&ffShowMeas_FuncDef, MeasurementCollectorShowMeas);
    iocshRegister(&ffsm_FuncDef, MeasurementCollectorShowMeas);

    iocshRegister(&ffShowPv_FuncDef, PvDataShow);
    iocshRegister(&ffsp_FuncDef, PvDataShow);

    iocshRegister(&ffShowDevices_FuncDef, LoopShowDevices);
    iocshRegister(&ffsd_FuncDef, LoopShowDevices);
    
    iocshRegister(&ffPatternGenMode_FuncDef, ExecThreadSetPatternGenMode);

    iocshRegister(&ffHelp_FuncDef, Help);
}

static FastFeedback::IocshRegister iocshRegisterObj;

/*----------------------------------------------------------------------*/
/*
#include <fcom_api.h>
#include <fcomUtil.h>
#include <fcom_api.h>
#include <fcomLclsBpm.h>
#include <fcomLclsBlen.h>
#include <fcomLclsFFCtrl.h>
extern "C" {
  void FCOMWrite(char *pvName, double value) {
    FcomID _id;

    _id = fcomLCLSPV2FcomID(pvName);
    if (_id == 0) {
      std::cout << "Failed to get FCOM ID for " << pvName << std::endl;
      return;
    }

    FcomBlob blob;

    // Code common for Fcom channel instances
    blob.fc_vers = FCOM_PROTO_VERSION;
    blob.fc_tsHi = 0xDD;//timestamp.secPastEpoch;
    blob.fc_tsLo = 0xEE;//timestamp.nsec;
    blob.fc_stat = 0;

    // Code for this specific channel
    blob.fc_idnt = _id;

    blob.fc_type = FCOM_EL_DOUBLE;
    blob.fc_nelm = 1;
    blob.fc_dbl = &value;

    int status = fcomPutBlob(&blob);
    if (status != 0) {
      std::cout << "ERROR: " << fcomStrerror(status) << " - "
		<< pvName << " FCOM ID: "
		<< (int) _id << std::endl;
      return;
    }
    std::cout << pvName << " = " << value << std::endl;
  }
}
*/

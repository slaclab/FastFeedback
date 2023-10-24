/*
 * File:   Log.cc
 * Author: lpiccoli
 *
 * Created on September 28, 2010, 2:33 PM
 */

#include <iostream>

#include "Log.h"

#include "errlog.h"

int DEBUG_PULSEID_FLAG = 0;
int DEBUG_CA_FLAG = 0;
int DEBUG_EVENT_FLAG = 0;
int DEBUG_CONFIG_FLAG = 0;
int DEBUG_ALGO_FLAG = 0;
int DEBUG_FBCKPV_FLAG = 0;
int DEBUG_OFFSET_FLAG = 0;
int DEBUG_BUFFER_FLAG = 0;

USING_FF_NAMESPACE

bool Log::_allLogToConsole = false;

/**
 * Default constructor.
 * 
 * @author L.Piccoli
 */
Log::Log(std::string loopName, std::string slotName) :
  _lastMessage(""),
  _loopName(loopName),
  _slotName(slotName),
  _logToConsole(false),
  _statusStrPv(NULL),
  _alarmTime(0) {
    clear();
    _messageHeader = "fac=FF:" + _loopName + " process=" + _slotName + " ";
}

/**
 * Defines if the messages are sent to the console besides being logged.
 *
 * @param log if true messages are sent to the logger and the console, false
 * means that messages will be sent to the logger only (default is true)
 * @author L.Piccoli
 */
void Log::logToConsole(bool log) {
#ifdef LOG
    _logToConsole = log;
#endif
}

Log &Log::operator<<(char const *message) {
#ifdef LOG
  _mutex.lock();
  _message << message;
  _mutex.unlock();
#endif
  return *this;
}

Log &Log::operator<<(int const value) {
#ifdef LOG
  _mutex.lock();
  _message << value;
  _mutex.unlock();
#endif
  return *this;
}

Log &Log::operator<<(long int const value) {
#ifdef LOG
  _mutex.lock();
  _message << value;
  _mutex.unlock();
#endif
  return *this;
}

Log &Log::operator<<(double const value) {
#ifdef LOG
  _mutex.lock();
  _message << value;
  _mutex.unlock();
#endif
  return *this;
}

Log &Log::operator<<(LogOperatorType const operation) {
#ifdef LOG
  _mutex.lock();
  bool alarm = true;
  switch (operation) {
  case cout:
    std::cout << _message.str() << std::endl;
    clear();
    break;
  case clearpv:
    clearStatusPv();
    break;
  case flushpvnoalarm:
    alarm = false;
  case flushpv:
    setStatusStrPv(alarm);
  case flush:
    flushMessage();
    break;
  case flushpvonlynoalarm:
    alarm = false;
  case flushpvonly:
    setStatusStrPv(alarm);
    clear();
    break;
  case dp:
    debugPrint();
    clear();
    break;
  case showtime:
    epicsTimeGetCurrent(&_timestamp);
    epicsTimeToStrftime(_timeString, sizeof(_timeString), "[%H:%M:%S] ", &_timestamp);
    _message << _timeString;
    break;
  default:
    _mutex.unlock();
    return *this;
  }
  _mutex.unlock();
#endif
  return *this;
}

void Log::debugPrint() {
#ifdef LOG
#ifdef DEBUG_PRINT
  _mutex.lock();
  int flagLevel = 0;
  switch(_dpFlag) {
  case flagPulseId:
    flagLevel = DEBUG_PULSEID_FLAG;
    break;
  case flagEvent:
    flagLevel = DEBUG_EVENT_FLAG;
    break;
  case flagCa:
    flagLevel = DEBUG_CA_FLAG;
    break;
  case flagConfig:
    flagLevel = DEBUG_CONFIG_FLAG;
    break;
  case flagAlgo:
    flagLevel = DEBUG_ALGO_FLAG;
    break;
  case flagFbckPv:
    flagLevel = DEBUG_FBCKPV_FLAG;
    break;
  case flagOffset:
    flagLevel = DEBUG_OFFSET_FLAG;
    break;
  case flagBuffer:
    flagLevel = DEBUG_BUFFER_FLAG;
    break;
  default:
    flagLevel = 0;
  }

  if ((int) _dpLevel <= flagLevel) {
    std::cout << _message.str() << std::endl;
  }
  _mutex.unlock();
#endif
#endif
}

std::string Log::getLastMessage() {
  return _lastMessage;
}

Log &Log::operator<<(LogDebugPrintLevel const debugLevel) {
#ifdef LOG
  _mutex.lock();
  _dpLevel = debugLevel;
  _mutex.unlock();
#endif
  return *this;
}

Log &Log::operator<<(LogDebugFlag const debugFlag) {
#ifdef LOG
  _mutex.lock();
  _dpFlag = debugFlag;
  _mutex.unlock();
#endif
  return *this;
}

void Log::flushMessage() {
#ifdef LOG
  _mutex.lock();
  
  _message << "\n";
  std::string outgoingMessage = _messageHeader + _message.str();
  if (_allLogToConsole || _logToConsole) {
    //errlogPrintf(outgoingMessage.c_str());
    printf(outgoingMessage.c_str());
  } //else {
    //printf(outgoingMessage.c_str());
    //errlogPrintfNoConsole(outgoingMessage.c_str());
  //}
  clear();
#endif
  _mutex.unlock();
}

void Log::setStatusStrPv(bool alarm) {
#ifdef LOG
  _mutex.lock();
  if (_statusStrPv != NULL) {
    *_statusStrPv = _message.str();
    time_t now = time(0);
    if (_alarmTime == 0) {
      _alarmTime = now - 10;
    }
    if (!alarm ||
	(alarm && now - _alarmTime > 5)) {
      _statusStrPv->setAlarm(alarm);
      if (alarm) {
	_alarmTime = now;
      }
    }
    _statusStrPv->scanIoRequest();
  }
  _mutex.unlock();
#endif
}

void Log::clearStatusPv() {
#ifdef LOG
  _mutex.lock();
  if (_statusStrPv != NULL) {
    *_statusStrPv = "";
    _statusStrPv->scanIoRequest();
    _statusStrPv->setAlarm(false);
  }
  _mutex.unlock();
#endif
}

void Log::clear() {
#ifdef LOG
  _mutex.lock();
  _lastMessage = _message.str();
  _message.str(std::string());
  _message.clear();
  _message.seekp(0);
  _mutex.unlock();
#endif
}

void Log::setLoopName(std::string loopName) {
#ifdef LOG
  _mutex.lock();
  _loopName = loopName;
  _messageHeader = "fac=FF:" + _loopName + " process=" + _slotName + " ";
  _mutex.unlock();
#endif
}

void Log::setSlotName(std::string slotName) {
#ifdef LOG
  _mutex.lock();
  _slotName = slotName;
  _messageHeader = "fac=FF:" + _loopName + " process=" + _slotName + " ";
  _mutex.unlock();
#endif
}

void Log::setDebugLevel(int flag, int level) {
#ifdef LOG
  _mutex.lock();
  std::cout << "Setting flag " << flag;
  switch (flag) {
  case flagPulseId:
    DEBUG_PULSEID_FLAG = level;
    std::cout << " (PULSEID)";
    break;
  case flagEvent:
    DEBUG_EVENT_FLAG = level;
    std::cout << " (EVENT)";
    break;
  case flagCa:
    DEBUG_CA_FLAG = level;
    std::cout << " (CA)";
    break;
  case flagConfig:
    DEBUG_CONFIG_FLAG = level;
    std::cout << " (CONFIG)";
    break;
  case flagAlgo:
    DEBUG_ALGO_FLAG = level;
    std::cout << " (ALGO)";
    break;
  case flagFbckPv:
    DEBUG_FBCKPV_FLAG = level;
    std::cout << " (FBCKPV)";
    break;
  case flagOffset:
    DEBUG_OFFSET_FLAG = level;
    std::cout << " (OFFSET)";
    break;
  case flagBuffer:
    DEBUG_BUFFER_FLAG = level;
    std::cout << " (BUFFER)";
    break;
  }
  std::cout << " to level " << level << " ";
  switch (level) {
      case 0:
          std::cout << "(DP_NONE)" << std::endl;
          break;
      case 1:
          std::cout << "(DP_FATAL)" << std::endl;
          break;
      case 2:
          std::cout << "(DP_ERROR)" << std::endl;
          break;
      case 3:
          std::cout << "(DP_WARN)" << std::endl;
          break;
      case 4:
          std::cout << "(DP_INFO)" << std::endl;
          break;
      case 5:
          std::cout << "(DP_DEBUG)" << std::endl;
          break;
  };
  _mutex.unlock();
#endif
}

/**
 * Return the Log singleton
 *
 * @return the Log
 * @author L.Piccoli
 */
Log &Log::getInstance() {
    static Log _instance;
    return _instance;
}

void Log::setAllLogToConsole(bool log) {
#ifdef LOG
    _allLogToConsole = log;
#endif
}

void Log::setStatusstrPv(PvData<std::string> *pvDataCharWaveform) {
#ifdef LOG
  _statusStrPv = pvDataCharWaveform;
#endif
}

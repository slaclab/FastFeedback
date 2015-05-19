/* 
 * File:   PvData.cc
 * Author: lpiccoli
 * 
 * Created on July 28, 2010, 9:55 AM
 */

#include "PvData.h"

USING_FF_NAMESPACE

/**
 * PvMap<...> static variable initialization.
 *
 * Had to make one instance per used type in order to initialize all maps.
 */
PvDataChar __dummyPvChar(true, 0);
PvDataUChar __dummyPvUChar(true, 0);
PvDataInt __dummyPvInt(true, 0);
PvDataUInt __dummyPvUInt(true, 0);
PvDataLong __dummyPvLong(true, 0);
PvDataULong __dummyPvULong(true, 0);
PvDataDouble __dummyPvDouble(true, 0);
PvDataShort __dummyPvShort(true, 0);
PvDataUShort __dummyPvUShort(true, 0);
PvDataString __dummyPvString(true, 0);
PvDataBool __dummyPvBool(true, 0);
PvDataDoubleWaveform __dummyPvDoubleWaveform(true, 0);
PvDataUShortWaveform __dummyPvUShortWaveform(true, 0);
PvDataStringWaveform __dummyPvStringWaveform(true, 0);

/**
 * Initialization of the _pvMap, which will contain pointers to all PvData
 * instances.
 */
template <class T>
std::map<std::string, std::vector<PvData<T>*> *> PvData<T>::_pvMap;

// To be removed
extern "C" {
  void createPvDataMutexes() {
  }
}

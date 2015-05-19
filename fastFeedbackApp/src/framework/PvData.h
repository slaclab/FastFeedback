/* 
 * File:   PvData.h
 * Author: lpiccoli
 *
 * Created on July 28, 2010, 9:55 AM
 */

#ifndef _PVDATA_H
#define	_PVDATA_H

#include <dbScan.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <aiRecord.h>
#include <alarm.h>
#include "Defs.h"
#include <epicsMutex.h>

FF_NAMESPACE_START

class LoopConfigurationTest;

template <class Type> class PvData;

/**
 * Definition of templates for all used data types
 */

/** PvData definition for CHAR PV */
typedef PvData<char> PvDataChar;

/** PvData definition for UCHAR PV */
typedef PvData<unsigned char> PvDataUChar;

/** PvData definition for INT PV */
typedef PvData<int> PvDataInt;

/** PvData definition for UINT PV */
typedef PvData<unsigned int> PvDataUInt;

/** PvData definition for LONG PV */
typedef PvData<long> PvDataLong;

/** PvData definition for ULONG PV */
typedef PvData<unsigned long> PvDataULong;

/** PvData definition for DOUBLE PV */
typedef PvData<double> PvDataDouble;

/** PvData definition for SHORT PV */
typedef PvData<short> PvDataShort;

/** PvData definition for USHORT PV */
typedef PvData<unsigned short> PvDataUShort;

/** PvData definition for STRING PV */
typedef PvData<std::string> PvDataString;

/** PvData definition for BOOL PV */
typedef PvData<bool> PvDataBool;

/** PvData definition for DOUBLE waveform PV */
typedef PvData<std::vector<double> > PvDataDoubleWaveform;

/** PvData definition for USHORT waveform PV */
typedef PvData<std::vector<unsigned short> > PvDataUShortWaveform;

/** PvData definition for STRING waveform PV */
typedef PvData<std::vector<std::string> > PvDataStringWaveform;

/** PvData definition for CHAR waveform PV */
typedef PvData<std::string> PvDataCharWaveform;

/**
 * Definition of template maps for the different types of PvData.
 * See comment on PvData::_pvMap bellow.
 */

/** Map of PvDataChar */
typedef std::map<std::string, std::vector<PvDataChar*>*> PvMapChar;

/** Map of PvDataUChar */
typedef std::map<std::string, std::vector<PvDataUChar*>*> PvMapUChar;

/** Map of PvDataInt */
typedef std::map<std::string, std::vector<PvDataInt*>*> PvMapInt;

/** Map of PvDataUInt */
typedef std::map<std::string, std::vector<PvDataUInt*>*> PvMapUInt;

/** Map of PvDataLong */
typedef std::map<std::string, std::vector<PvDataLong*>*> PvMapLong;

/** Map of PvDataULong */
typedef std::map<std::string, std::vector<PvDataULong*>*> PvMapULong;

/** Map of PvDataDouble */
typedef std::map<std::string, std::vector<PvDataDouble*>*> PvMapDouble;

/** Map of PvDataShort */
typedef std::map<std::string, std::vector<PvDataShort*>*> PvMapShort;

/** Map of PvDataUShort */
typedef std::map<std::string, std::vector<PvDataUShort*>*> PvMapUShort;

/** Map of PvDataString */
typedef std::map<std::string, std::vector<PvDataString*>*> PvMapString;

/** Map of PvDataBool */
typedef std::map<std::string, std::vector<PvDataBool*>*> PvMapBool;

/** Map of PvDataDoubleWaveform */
typedef std::map<std::string, std::vector<PvDataDoubleWaveform*>*> PvMapDoubleWaveform;

/** Map of PvDataUShortWaveform */
typedef std::map<std::string, std::vector<PvDataUShortWaveform*>*> PvMapUShortWaveform;

/** Map of PvDataStringWaveform */
typedef std::map<std::string, std::vector<PvDataStringWaveform*>*> PvMapStringWaveform;

/** Map of PvDataCharWaveform */
typedef std::map<std::string, std::vector<PvDataCharWaveform*>*> PvMapCharWaveform;

/**
 * This is a template class used for connecting object attributes to PV Device
 * Support. Any attribute (not a DeviceView) that is accessible through a PV
 * should be of this template class. The parameter to the template should be
 * the attribute type (long, int, double, etc).
 *
 * As instances are created they are added to the static PvData::_pvMap. When
 * the Device Support is created the specified INST_IO string is used to search
 * for the correct device in the _pvMap.
 *
 * TODO: Need to add locking, to avoid race conditions between the code that updates
 * the PvData::_value and the device support code that uses it. These operations
 * happen on different threads/tasks.
 *
 * TODO: Add a second parameter to the template, the record type (e.g. aiRecord,
 * stringinRecord, etc...)
 * 
 * @author L.Piccoli
 */
template <class Type>
class PvData {
public:

    /**
     * Create a PvData with the given PvName and add it to the _pvMap
     * singleton
     *
     * @param pvName name of the PV associated with this data
     * @author L.Piccoli
     */
    PvData(std::string pvName) :
      _externalValuePtr(NULL),
      _pvName(pvName),
      _scanlist(NULL),
      _record(NULL),
      _mutex(NULL) {
        insert();
   };

    PvData() :
    _externalValuePtr(NULL),
      _pvName("ERROR"),
      _scanlist(NULL),
      _record(NULL),
      _mutex(NULL) {
    };

    PvData(std::string pvName, Type value) :
      _value(value),
      _externalValuePtr(NULL),
      _pvName(pvName),
      _scanlist(NULL),
      _record(NULL),
      _mutex(NULL) {
        insert();
    }

    /**
     * Empty constructor, used only to instantiate classes in
     * order to initialize the _pvMap static variable.
     *
     * This constructor must *not* be used for other purposes besides
     * initializing the maps.
     *
     * @author L.Piccoli
     */
    PvData(bool clear, int a) :
      _externalValuePtr(NULL),
      _pvName("INVALID"),
      _mutex(NULL) {
    	if (clear) {
    	  _pvMap.clear();
    	}
    }

    virtual ~PvData() {
      if (_mutex != NULL) {
     	  delete _mutex;
      }
    };
    /*
    PvData & operator=(const PvData &) {
        return *this;
    };
    */

    /* used by threads to set the value of the data, must
     * lock so that pv records don't read while a thread
     * is setting this data
     */
    PvData & operator=(const Type &value) {
      if (_mutex != NULL) {
	_mutex->lock();
      }

      _value = value;

      if (_mutex != NULL) {
	_mutex->unlock();
      }

      return *this;
    }

    bool operator==(const PvData &other) {
        return _value == other._value;
    }

    bool operator==(const Type &value) {
        return value == _value;
    }

    bool operator!=(const PvData &other) {
        return !(*this == other);
    }

    bool operator!=(const Type &value) {
        return !(*this == value);
    }

    bool operator<(const Type &value) {
        return _value < value;
    }

    static std::map<std::string, std::vector<PvData<Type> *> *> &getPvMap() {
        return _pvMap;
    }

    Type *getValueAddress() {
        if (_externalValuePtr == NULL) {
            return &_value;
        } else {
            std::cerr << ">>> RETURN EXTERNAL ADDRESS" << std::endl;
            return _externalValuePtr;
        }
    }

    Type getValue() {
    	Type val;
    	if (_mutex != NULL) {
	  _mutex->lock();
	}

        if (_externalValuePtr == NULL) {
            val = _value;
        } else {
            std::cerr << ">>> RETURN EXTERNAL VALUE ("
		      << _pvName << ")" << std::endl;
            val = *_externalValuePtr;
        }

       	if (_mutex != NULL) {
	  _mutex->unlock();
	}

        return val;
    }

    /**
     * Create the mutex to protect the value of the PvData - this must be done by the
     * device support init routine since the mutex cannot exist before the init
     */
    void createMutex() {
      /*
      if (_mutex == NULL) {
	_mutex = new epicsMutex();

	size_t state_position = _pvName.find(" STATE");
	size_t mode_position = _pvName.find(" MODE");

	if (state_position != std::string::npos ||
	    mode_position != std::string::npos) {
	  std::cout << "Mutex info for " << _pvName << std::endl;
	  _mutex->show(4);
	}
      }
      */
    }

    /**
     * This allows values to be read from/written to an area external to the
     * PvData class.
     *
     * For example, suppose Class A declares a PvDataInt, but
     * the actual data resides in an external package. The address of the value
     * from the external package is passed to this method, such that the
     * read/write operations now use the specified address.
     *
     * @param externalValue pointer to the value to be used by this PvData
     * @author L.Piccoli
     */
    void setExternalValuePtr(Type *externalValuePtr) {
        _externalValuePtr = externalValuePtr;
        //std::cerr << ">>> SET EXTERNAL VALUE PTR("
	    //  << _pvName << ")" << std::endl;
    }

    std::string getPvName() {
        return _pvName;
    }

    /**
     * This method is called by the Device Support code to update the internal
     * value based on the data received from Channel Access. It is used
     * by output devices such as Ao, Bo, Stringout and Waveformout.
     *
     * If the Waveformout is used, this method must be overriden by a subclass
     * implementation. The process of copying from the void * parameter to the
     * internal representation depends on the element of the waveform (typically
     * represented as std::vector<...>).
     *
     * @param val pointer to the new data to be assigned. The val pointer is
     * the precord->val field.
     * @param size number of elements pointed by the parameter val (defaul is 1)
     * @return 0 on success, -1 on failure
     * @author L.Piccoli
     */
    virtual int write(Type *val, int size = 1) {
      if (_mutex != NULL) {
	_mutex->lock();
      }

      _value = *val;
      if (_externalValuePtr != NULL) {
    	  *_externalValuePtr = *val;
      }

      if (_mutex != NULL) {
	_mutex->unlock();
      }

      return 0;
    }

    /**
     * This method is called by the Device Support code to update the PV record
     * value based on the internal data. It is used by input devices such as Ai,
     * Bi, Stringin and Waveformin.
     *
     * If the Waveformin is used, this method must be overriden by a subclass
     * implementation. The process of copying from the waveform (typically
     * represented as std::vector<...>) to the void * parameter depends on the
     * type of the waveform elements.
     *
     * @param val pointer to the data that will be updated. The val pointer is
     * the precord->val field.
     * @param size number of elements pointed by the parameter val (defaul is 1)
     * @return 0 on success, -1 on failure
     * @author L.Piccoli
     */
    int read(Type *val, int size = 1) {
      if (_mutex != NULL) {
	_mutex->lock();
      }

      if (_externalValuePtr == NULL) {
	*val = _value;
      } else {
	*val = *_externalValuePtr;
      }
      
      if (_mutex != NULL) {
	_mutex->unlock();
      }

      return 0;
    }

    IOSCANPVT getScanList() {
        return _scanlist;
    }

    void initScanList() {
        scanIoInit(&_scanlist);
    }

    void scanIoRequest() {
      if (_scanlist != NULL) {
	::scanIoRequest(_scanlist);
      }
    }

    void setRecord(void *precord) {
      _record = reinterpret_cast<aiRecord *>(precord);
    }

    void setAlarm(bool set = true) {
      if (_record != NULL) {
	if (set) {
	  _record -> nsev = MAJOR_ALARM;
	  _record -> nsta = HIHI_ALARM;
	}
	else {
	  _record -> nsev = NO_ALARM;
	  _record -> nsta = NO_ALARM;
	}
      }
    }

    void show() {
      std::cout << _pvName << "\t";
      if (_record != NULL) {
	std::cout << _record -> name;
      }
      else {
	std::cout << "NULL";
      }
    }

    void lock() {
      if (_mutex != NULL) {
	_mutex->lock();
      }
    }

    void unlock() {
      if (_mutex != NULL) {
	_mutex->unlock();
      }
    }

    friend class LoopConfigurationTest; // For unit tests

    static void showMap() {
      typename std::map<std::string, std::vector<PvData<Type> *> *>::iterator it;
      for (it = _pvMap.begin(); it != _pvMap.end(); ++it) {
	std::cout << it->first << ": " << it->second->size() << std::endl;
      }
    }

protected:
    /** Actual value */
    Type _value;

    /** pointer to External value */
    Type *_externalValuePtr;

    /** Name of the PV associated with this PvData */
    std::string _pvName;

private:
    /** Device support scanlist, used for a single PvData instance */
    IOSCANPVT _scanlist;

    /**
     * Points to the PV record. Even though an aiRecord is used here, most
     * fields at the start of the struct are the same for all type of records.
     * Remember that the value for the record is provided by the _value
     * attribute, which is accessed through device support.
     */
    aiRecord *_record;

    // one mutex per PV, protects any reads/writes with the PV no matter what the type
    epicsMutex *_mutex;

    /**
     * Insert a new PvData in the proper Map
     *
     * @author L.Piccoli
     */
    void insert() {
      // This code is to avoid the insertion of PvDatas created by Devices
      // used for seachingn only (see LoopConfiguration::getDevice())
      size_t position = _pvName.find("XX00");
      if (position != std::string::npos && position == 0) {
	return;
      }
      
        // First check if there is a PV with the same name, and then append it
        // to the end of the vector
        std::vector<PvData<Type> *> *vector;
        typename std::map<std::string, std::vector<PvData<Type> *> *>::iterator it;
        it = _pvMap.find(_pvName);
        if (it == _pvMap.end()) {
            vector = new std::vector<PvData<Type> *>();
            _pvMap.insert(std::pair<std::string, std::vector<PvData<Type> *> *>(_pvName, vector));
        } else {
            vector = it->second;
        }
        vector->push_back(this);
    }

    /**
     * Unique map of PvName -> vector of PvData for the specified Type. This map
     * is only used to lookup PvData instances based on a PvName when the Device
     * Support code is created.
     *
     * The object file containing this code must be loaded and initialized
     * before the iocInit() call is performed. During iocInit the device support
     * code is called, which will search for the PvData instances in this _pvMap.
     *
     * For devices used as output (e.g. Ao, Bo, Stringout, Waveformout), the
     * pointer to std::vector<PvData<Type> *> > is saved to the precord->dvpt
     * field.
     *
     * If devices are used as input (e.g. Ai, Bi, Stringin, Waveformin) the
     * pointer to the PvData<Type> is saved to precord-dpvt.
     *
     * The assignment to precord->dpvt is performed in the device support code
     * (see file devFfConfig.cc)
     */
    static std::map<std::string, std::vector<PvData<Type> *> *> _pvMap;

};


FF_NAMESPACE_END

#endif	/* _PVDATA_H */


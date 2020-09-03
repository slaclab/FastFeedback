/* 
 * File:   PvData.h
 * Author: lpiccoli
 *
 * Created on July 28, 2010, 9:55 AM
 */

#ifndef _PVDATA_H
#define	_PVDATA_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <aiRecord.h>
#include <alarm.h>
#include <dbScan.h>
#include <epicsMutex.h>

#include "Defs.h"

FF_NAMESPACE_START

class LoopConfigurationTest;
template <class Type> class PvData;

template <typename T>
using PvDataWaveform = PvData<std::vector<T>>;

template <typename T>
using PvMap = std::map<std::string, std::vector<PvData<T>*>*>;

template <typename T>
using PvMapWaveform = std::map<std::string, std::vector<PvDataWaveform<T>*>*>;

/**
 * This is a template class used for connecting object attributes to PV Device
 * Support. Any attribute (not a DeviceView) that is accessible through a PV
 * should be of this template class. The parameter to the template should be
 * the attribute type (long, int, double, etc).
 *
 * As instances are created they are added to the static PvData<Type>::_pvMap. When
 * the Device Support is created the specified INST_IO string is used to search
 * for the correct device in the _pvMap.
 *
 * TODO: Add a second parameter to the template, the record type (e.g. aiRecord,
 * stringinRecord, etc...)
 * 
 */
template <class Type>
class PvData {
public:

    PvData() = default;

    /**
     * Create a PvData with the given PvName and add it to the static _pvMap
     *
     * @param pvName name of the PV associated with this data
     */
    PvData(std::string pvName) :
      _pvName(pvName)
    {
        insert();
    }

    /**
     * Create a PvData with the given PvName and add it to the static _pvMap
     *
     * @param pvName name of the PV associated with this data
     * @param value initial value of the PV
     */

    PvData(std::string pvName, Type value) :
      _value(value),
      _pvName(pvName)
    {
        insert();
    }

    virtual ~PvData() {
      if (_mutex != nullptr) {
     	  delete _mutex;
      }
    }

    /* used by threads to set the value of the data, must
     * lock so that pv records don't read while a thread
     * is setting this data
     */
    PvData & operator=(const Type &value) {
        lock();
        _value = value;
        unlock();

        return *this;
    }

    template <typename U>
    bool operator==(const PvData<U> &other) const {
        lock();
        auto res =  _value == other.getValue();
        unlock();

        return res;
        
    }

    bool operator==(const Type &value) const {
        lock();
        auto res = value == _value;
        unlock();

        return res;
    }

    // do a comparison with implicit conversion
    template <typename U>
    bool operator==(const U &value) const {
        lock();
        auto res = value == _value; 
        unlock();

        return res;
    }

    bool operator!=(const PvData<Type> &other) const {
        return !(*this == other);
    }

    bool operator!=(const Type &value) const {
        return !(*this == value);
    }

    bool operator<(const Type &value) const {
        lock();
        auto res =  _value < value;
        unlock();

        return res;
    }

    // Conversion of PvData<Type> to Type.
    // Allows us to use something like 1 == a where a is a PvData<int>
    operator auto() const {
        return getValue();
    }

    static auto& getPvMap() {
        return _pvMap;
    }

    /**
     * Get the value address for device support
     * Should not need to lock the mutex here since we are giving the pointer
     * to the thing we would be locking against.
     */
    Type* getValueAddress() {
        if (_externalValuePtr == nullptr)
            return &_value;
        else {
            std::cerr << ">>> RETURN EXTERNAL ADDRESS\n";
            return _externalValuePtr;
        }
    }

    Type getValue() const {
    	Type val;
        lock();

        if (_externalValuePtr == nullptr)
            val = _value;
        else {
            std::cerr << ">>> RETURN EXTERNAL VALUE ("
		      << _pvName << ")" << std::endl;
            val = *_externalValuePtr;
        }

        unlock();

        return val;
    }

    /**
     * Create the mutex to protect the value of the PvData - this must be done by the
     * device support init routine since the mutex cannot exist before the init
     *
     */
    void createMutex() { _mutex = newEpicsMutex; }

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
    }

    std::string getPvName() const {
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
        lock();

        _value = *val;
        if (_externalValuePtr != nullptr)
            *_externalValuePtr = *val;

        unlock();

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
        lock();

        if (_externalValuePtr == nullptr)
            *val = _value;
        else 
            *val = *_externalValuePtr;

        unlock();

        return 0;
    }

    IOSCANPVT getScanList() {
        return _scanlist;
    }

    void initScanList() {
        scanIoInit(&_scanlist);
    }

    void scanIoRequest() {
        if (_scanlist != nullptr)
            ::scanIoRequest(_scanlist);
    }

    void setRecord(void *precord) {
        _record = reinterpret_cast<aiRecord *>(precord);
    }

    void setAlarm(bool set = true) {
        if (_record != nullptr) {
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

    void show() const {
        std::cout << _pvName << "\t";
        if (_record != nullptr)
	        std::cout << _record -> name;
        else
            std::cout << "NULL";
    }

    void lock() const {
        if (_mutex != nullptr)
            _mutex->lock();
    }

    void unlock() const {
        if (_mutex != nullptr)
            _mutex->unlock();
    }


    static void showMap() {
        for (const auto& item : _pvMap)
            std::cout << item.first << ": " << item.second->size() << '\n';
    }

    friend class LoopConfigurationTest;            // For unit tests

protected:
    Type         _value;                           // Actual value
    Type        *_externalValuePtr = nullptr;      // Optional pointer to external value
    std::string  _pvName           = "ERROR";      // Name of the EPICS record

private:
    IOSCANPVT    _scanlist         = nullptr;      // Device support scanlist, used by a single record

    /**
     * Points to the PV record. Even though an aiRecord is used here, most
     * fields at the start of the struct are the same for all type of records.
     * Remember that the value for the record is provided by the _value
     * attribute, which is accessed through device support.
     */
    aiRecord    *_record           = nullptr;

    mutable epicsMutex *_mutex     = nullptr;      // Declared mutable to allow locking in const-qualified functions

    /**
     * Insert this object if not already in the map and not created by Devices
     */
    void insert() {
        // Do not insert if created by Devices
        auto position = _pvName.find("XX00");
        if (position != std::string::npos && position == 0)
            return;
     
        auto *vec = new std::vector<PvData<Type>*> { this };
        const auto [element, inserted] = _pvMap.insert({ _pvName, vec});
        if (!inserted) {
            element->second->push_back(this);
            delete vec;
        }
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
    inline static PvMap<Type> _pvMap;

};


FF_NAMESPACE_END

#endif	/* _PVDATA_H */


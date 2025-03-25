/* 
 * File:   CaChannel.cc
 * Author: lpiccoli
 * 
 * Created on June 29, 2010, 1:23 PM
 */

#include <iostream>
#include <sstream>
#include "CaChannel.h"
#include "Log.h"

USING_FF_NAMESPACE


/**
 * Create a ChannelAccess Channel using the specified name as the PV name,
 * using EPICS ca_create_channel() and ca_pend_io() calls.
 *
 * @param accessType defines whether this channel is for read access
 * (CommunicationChannel::READ_ONLY) or write access
 * (CommunicationChannel::WRITE_ONLY) (Although the channel should be defined
 * as either input or output, nothing is preventing a CaChannel set as
 * READ_ONLY to be written and vice-versa)
 * @param name name of the ChannelAccess PV.
 * @author L.Piccoli
 */
CaChannel::CaChannel(CommunicationChannel::AccessType accessType,
        std::string name) :
  CommunicationChannel(accessType),
  _name(name),
  _readStats(50, "CA Read Stats") {
	int maxTentatives = 4;//7; // Tries 7 times with 20 sec. timeout each - 2:20 min total.
    bool connected = false;
    int tentative = 0;

    while (!connected && tentative < maxTentatives) {
        tentative++;
        int status = ca_create_channel(_name.c_str(), NULL, NULL, 10, &_channelId);
        if (status != ECA_NORMAL) {
        	//            std::stringstream s;
        	_stringStream.str(std::string()); // Clear the stream
        	_stringStream << "ERROR: Could not create ChannelAccess channel for PV "
        			<< _name.c_str() << "(status=" << status
        			<< ") from ca_create_channel (ca_state()="
        			<< ca_state(_channelId) << ")";

        	Log::getInstance() << Log::flagCa << Log::dpError << _stringStream.str().c_str() << Log::dp;

        	Log::getInstance() << _stringStream.str().c_str() << Log::flush;
            throw Exception(_stringStream.str());
        }
        status = ca_pend_io(20.0);
        if (status == ECA_NORMAL) {
            connected = true;
        } else {
        	Log::getInstance() << Log::flagCa << Log::dpWarn << "WARN: CaChannel for PV "
                    << _name.c_str() << " failed (tentative #"
                    << tentative << ") (ca_state()="
                    << ca_state(_channelId) << ")" << Log::dp;
        }
    }

    if (!connected) {
      //        std::stringstream s;
      _stringStream.str(std::string()); // Clear the stream
      _stringStream << "INFO: CaChannel for PV "
		      << _name.c_str() << " failed after "
		      << maxTentatives << " tries";
        Log::getInstance() << Log::flagCa << Log::dpInfo << _stringStream.str().c_str() << Log::dp;
        throw Exception(_stringStream.str());
    }

    Log::getInstance() << Log::flagCa << Log::dpInfo << "INFO: Created CaChannel for PV "
            << _name.c_str() << Log::dp;

}

/**
 * Clears the ChannelAccess channel created in the constructor by invoking
 * EPICS ca_clear_channel().
 *
 * @author L.Piccoli
 */
CaChannel::~CaChannel() {
    ca_clear_channel(_channelId);
	int status = ca_pend_io(5.0);
	if (status != ECA_NORMAL) {
      Log::getInstance() << "ERROR: clear_channel ca_pend_io(" << _name.c_str()
			       << "): " << ca_message(status) << Log::flush;
	}
}

/**
 * Read a double value from ChannelAccess.
 *
 * TODO: fill up timestamp parameter
 *
 * @param value its an output parameter that contains the read value. In
 * case of error then the value is set to ZERO - and should *not* be used.
 * @return 0 in case of success reading from ChannelAccess, -1 if
 * there is an error.
 * @author L.Piccoli
 */
int CaChannel::read(double &value, epicsTimeStamp &timestamp, double timeout) {
    value = -1;
    struct dbr_time_double * data;
    unsigned nBytes;
    unsigned elementCount;
    int tentative = 0;
    int maxTentatives = 3; // Tries 3 times with 3 sec. timeout each
    int success = false;

    _readStats.start();
    //    if (timeout >= 1) {
      elementCount = ca_element_count ( _channelId );
      nBytes = dbr_size_n(DBR_TIME_DOUBLE,elementCount);
      data = (struct dbr_time_double *) malloc(nBytes);
      if ( ! data ) {
          Log::getInstance() << "insufficient memory to complete request\n" << Log::flush;
          return -1;
      }
      while (!success && tentative < maxTentatives) {
        tentative++;
	
        //int status = ca_get(DBR_DOUBLE, _channelId, (void *) & value);
        int status = ca_array_get(DBR_TIME_DOUBLE,elementCount, _channelId, data);
        if (status != ECA_NORMAL) {
	      Log::getInstance() << "ERROR: ca_get(" << _name.c_str()
			     << "): " << ca_message(status) << Log::flush;
	      value = 0;
	      _readErrorCount++;
        } else {
	      status = ca_pend_io(timeout);
	      if (status != ECA_NORMAL) {
	        Log::getInstance() << "ERROR: ca_pend_io(" << _name.c_str()
			       << "): " << ca_message(status) << Log::flush;
	        value = 0;
	      } else {
	        _readCount++;
          value = data->value;
	        success = true;
	      }
        }
      }
      /*
    }
    else {
      int status = ca_get(DBR_DOUBLE, _channelId, (void *) & value);
      status = ca_pend_io(timeout);
      if (status != ECA_NORMAL) {
	Log::getInstance() << "ERROR: ca_get(" << _name.c_str()
			   << "): " << ca_message(status) << Log::flush;
	_readErrorCount++;
	value = _lastValue;
      }
    }
      */
    _readStats.end();
    timestamp = data->stamp;
    
    free(data);
    
    _lastValue = value;
    if (!success) {
        return -1;
    }

    return 0;
}

/**
 * Write a double value to ChannelAccess.
 *
 * @param value value written to the PV.
 * @return 0 in case of success writing from ChannelAccess, -1 if
 * there is an error.
 * @author L.Piccoli
 */
int CaChannel::write(double value) {
    int status = ca_put(DBR_DOUBLE, _channelId, &value);

    if (status != ECA_NORMAL) {
        Log::getInstance() << "ERROR: ca_put(" << _name.c_str()
                << ") " << ca_message(status) << Log::flush;
        value = 0;
        _writeErrorCount++;
        return -1;
    }
    // Must call ca_pend_io or ca_flush_io to avoid buffering.
    status = ca_pend_io(1.0); // TODO: fix time out value
    if (status != ECA_NORMAL) {
        Log::getInstance() << "ERROR: ca_pend_io(" << _name.c_str()
                << ") " << ca_message(status) << Log::flush;
        value = 0;
        _readErrorCount++;
        return -1;
    }
    _writeCount++;

    _lastValue = value;

    return 0;
}

int CaChannel::write(double value, epicsTimeStamp timestamp) {
    return write(value);
}

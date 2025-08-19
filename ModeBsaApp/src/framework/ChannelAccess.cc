/* 
 * File:   ChannelAccess.cc
 * Author: lpiccoli
 * 
 * Created on July 13, 2010, 9:07 AM
 */

#include <iostream>

#include "ChannelAccess.h"
#include "ExecConfiguration.h"
#include "Log.h"

USING_FF_NAMESPACE

        int ChannelAccess::_referenceCount = 0;
struct ca_client_context *ChannelAccess::_channelAccessContext = NULL;

/**
 * Creates the ChannelAccess context, if the init parameter is true (default
 * value is false).
 *
 * @param initialize if true the CA context is created in the constructor,
 * otherwise the initialize() method must be called to create the CA context
 * (Default).
 * @author L.Piccoli
 */
ChannelAccess::ChannelAccess(bool init) {
}

int ChannelAccess::attach() {
    if (_referenceCount == 0) {
        if (createContext() != 0) {
            Log::getInstance() << "FATAL: Failed to create ChannelAccess context." << Log::flush;
            return -1; //throw std::exception();
        }
    } else {
        if (attachContext() != 0) {
            Log::getInstance() << "FATAL: Failed to attach to ChannelAccess context." << Log::flush;
            return -1; //throw std::exception();
        }
    }
    ++_referenceCount;
    return 0;
}

int ChannelAccess::detach() {
    --_referenceCount;
    if (_referenceCount == 0) {
        ca_context_destroy();
    } else {
        /* ca_detach_contex() not defined in cadef.h??
        if (ca_detach_context() != ECA_NORMAL) {
            std::cerr << "ERROR: Failed to detach from ChannelAccess context"
                    << std::endl;
        }
         */
    }
    return 0;
}

/**
 * Clean up the ChannelAccess context.
 *
 * @author L.Piccoli
 */
ChannelAccess::~ChannelAccess() {
}

/**
 * ChannelAccess initialization. Creates the ca_context and waits for
 * ChannelAccess to be available.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ChannelAccess::createContext() {
    int status = ca_context_create(ca_enable_preemptive_callback);

    if (status != ECA_NORMAL) {
        Log::getInstance() << "ERROR: " << ca_message(status) << Log::flush;
        return -1;
    }

    status = ca_pend_io(10.0);
    if (status != ECA_NORMAL) {
        Log::getInstance() << "ERROR: " << ca_message(status) << Log::flush;
        return -1;
    }

    _channelAccessContext = ca_current_context();
    return 0;
}

int ChannelAccess::attachContext() {
    int status = ca_attach_context(_channelAccessContext);
    if (status != ECA_NORMAL) {
        return -1;
    }
    return 0;
}

/**
 * ChannelAccess initialization. Creates the ca_context and waits for
 * ChannelAccess to be available.
 *
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
int ChannelAccess::initialize() {
    int status = ca_context_create(ca_enable_preemptive_callback);

    if (status != ECA_NORMAL) {
        Log::getInstance() << "ERROR: " << ca_message(status) << Log::flush;
        return -1;
    }

    status = ca_pend_io(10.0);
    if (status != ECA_NORMAL) {
        Log::getInstance() << "ERROR: " << ca_message(status) << Log::flush;
        return -1;
    }

    _channelAccessContext = ca_current_context();
    return 0;
}

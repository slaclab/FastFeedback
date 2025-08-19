/* 
 * File:   ChannelAccess.h
 * Author: lpiccoli
 *
 * Created on July 13, 2010, 9:07 AM
 */

#ifndef _CHANNELACCESS_H
#define	_CHANNELACCESS_H

#include "Defs.h"

#include <cadef.h>

FF_NAMESPACE_START

/**
 * This is a helper class to keep any ChannelAccess global information.
 *
 * @author L.Piccoli
 */
class ChannelAccess {
public:
    ChannelAccess(bool init = false);
    ~ChannelAccess();

    int initialize();
    int createContext();
    int attachContext();
//    int initDeviceSupport();

    int attach();
    int detach();

    static ChannelAccess &getInstance() {
        return _instance;
    }

private:
    /** There is only one ChannelAccess instance */
    static ChannelAccess _instance;

    /** Counts the number of threads attached to the context */
    static int _referenceCount;

    /** Pointer to Channel Access context */
    static struct ca_client_context *_channelAccessContext;
};

FF_NAMESPACE_END
        
#endif	/* _CHANNELACCESS_H */


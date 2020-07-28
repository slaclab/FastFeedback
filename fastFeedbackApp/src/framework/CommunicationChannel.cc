/* 
 * File:   CommunicationChannel.cc
 * Author: lpiccoli
 * 
 * Created on May 28, 2010, 12:09 PM
 */

#include "CommunicationChannel.h"

USING_FF_NAMESPACE

CommunicationChannel::CommunicationChannel(AccessType accessType) :
_accessType(accessType),
_readCount(0),
_writeCount(0),
_readErrorCount(0),
_writeErrorCount(0),
_lastValue(0) 
{
}

CommunicationChannel::~CommunicationChannel() 
{
}

/*=============================================================================
 
  Name: ffRegister.h

  Abs:  Public prototypes for ffRegister.c   
        Reference web pages on LCLS Controls | Facilities |FF-Aware IOC | :
                        Database Service (DBS) Design Document 
                        Functional Requirements | FF Database Service (DBS)
                        Database Input/Output Utilities

  Auth:26 May 2009, drogind created
  Rev: 
 
-----------------------------------------------------------------------------*/
#include "copyright_SLAC.h"    
/*----------------------------------------------------------------------------- 
  Mod:  (newest to oldest)  
        DD-MMM-YYYY, My Name:
           Changed such and such to so and so. etc. etc.
        DD-MMM-YYYY, Your Name:
           More changes ... The ordering of the revision history 
           should be such that the NEWEST changes are at the HEAD of
           the list.
 
=============================================================================*/
#ifndef INCffRegisterH
#define INCffRegisterH

#include "shareLib.h"

#ifdef __cplusplus
extern "C" {
#endif

void ffRegistrar(void);

#ifdef __cplusplus
}
#endif

#endif /*INCffRegisterH*/

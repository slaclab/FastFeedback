/*=============================================================================
 
  Name: ffRegister.c
        ffRegistrar - registers all "ffxxx" commands with the iocsh

  Abs:  shell commands must be registered before they can be recognized by
        IOC shell. Registration is achieved by calling a registration function
		with the appropiate parameters for each command.

  Auth: 02-Feb-2005, Debbie Rogind (drogind): created with some initial code 
               split from ffMain.c
  Rev:  02-Feb-2005 dfairley


-----------------------------------------------------------------------------*/

#include "copyright_SLAC.h"	/* SLAC copyright comments */
 
/*-----------------------------------------------------------------------------
 
  Mod:  (newest to oldest)  
       dec 2009 modified for Fast Feedback drogind, dfairley
        01-Apr-2005, Ron MacKenzie (ronm):
	   Add ffAsyncDumpGlob
        
=============================================================================*/
#include "debugPrint.h"
#ifdef  DEBUG_PRINT
  int ffRegFlag = DP_DEBUG;
#endif


/* c library includes */
#include	<string.h>

/* epics includes */
#include    <dbDefs.h>        /* NELEMENTS */
#include    <iocsh.h>         /* ioc shell stuff */
#include    <registryFunction.h>
#include    <epicsExport.h>   /* for registering ffRestart, ffStop */

/* thread prototypes  */
#include    "ffMain.h"    /* ffStart, ffRestart, ffStop */
#include    "ffRegister.h"

/* ffShowFcomDiags */
static const iocshArg ffShowFcomDiagsArg0 = { "loop", iocshArgInt};
static const iocshArg * const ffShowFcomDiagsArgs[1] = {&ffShowFcomDiagsArg0};
LOCAL iocshFuncDef ffShowFcomDiagsFuncDef = 
  {"ffShowFcomDiags", 1,ffShowFcomDiagsArgs };
LOCAL void ffShowFcomDiagsCallFunc(const iocshArgBuf *args)
{
  ffShowFcomDiags(args[0].ival);
}
/* ffStart */
LOCAL iocshFuncDef ffStartFuncDef = 
  {"ffStart", 0, NULL };
LOCAL void ffStartCallFunc(const iocshArgBuf *args)
{
  ffStart();
}
/* ffStartLoopNum */
static const iocshArg ffStartLoopNumArg0 = { "loopNum", iocshArgInt};
static const iocshArg ffStartLoopNumArg1 = { "loopString", iocshArgString};
static const iocshArg * const ffStartLoopNumArgs[2] = {&ffStartLoopNumArg0,&ffStartLoopNumArg1};
LOCAL iocshFuncDef ffStartLoopNumFuncDef = 
  {"ffStartLoopNum", 2, ffStartLoopNumArgs };
LOCAL void ffStartLoopNumCallFunc(const iocshArgBuf *args)
{
  ffStartLoopNum(args[0].ival, args[1].sval);
}
/* ffStop */
LOCAL iocshFuncDef ffStopFuncDef = 
  {"ffStop", 0, NULL };
LOCAL void ffStopCallFunc(const iocshArgBuf *args)
{
  ffStop();
}

/* ffRestart */
LOCAL iocshFuncDef ffRestartFuncDef = 
  {"ffRestart", 0, NULL };
LOCAL void ffRestartCallFunc(const iocshArgBuf *args)
{
  ffRestart();
}
/* ffFreeLists */
LOCAL iocshFuncDef ffFreeListsFuncDef = 
  {"ffFreeLists", 0, NULL };
LOCAL void ffFreeListsCallFunc(const iocshArgBuf *args)
{
  ffFreeLists();
}
/* ffdbAddr */
LOCAL iocshFuncDef ffdbAddrFuncDef = 
  {"ffdbAddr", 0, NULL };
LOCAL void ffdbAddrCallFunc(const iocshArgBuf *args)
{
  ffdbAddr();
}


LOCAL registryFunctionRef ffRef[] = {
  {"ffShowFcomDiags", (REGISTRYFUNCTION)ffShowFcomDiags},
  {"ffStart", (REGISTRYFUNCTION)ffStart},
  {"ffStartLoop", (REGISTRYFUNCTION)ffStartLoopNum},
  {"ffStop", (REGISTRYFUNCTION)ffStop},
  {"ffRestart",(REGISTRYFUNCTION)ffRestart},
  {"ffFreeLists",(REGISTRYFUNCTION)ffFreeLists},
  {"ffdbAddr", (REGISTRYFUNCTION)ffdbAddr},


};

/*=============================================================================

  Name: ffRegistrar

  Abs: add ffStart and ffStop to the function registry

  Args: none

  Rem: epics requires global functions to be registered for proper linking

  Side: 

  Ret:  none

==============================================================================*/
void ffRegistrar(void)
{
    registryFunctionRefAdd(ffRef,NELEMENTS(ffRef));
    iocshRegister(&ffShowFcomDiagsFuncDef, ffShowFcomDiagsCallFunc);
    iocshRegister(&ffStartFuncDef, ffStartCallFunc);
    iocshRegister(&ffStartLoopNumFuncDef, ffStartLoopNumCallFunc);
	/*    iocshRegister(&ffGetFcomIDFuncDef, ffGetFcomIDCallFunc);*/
    iocshRegister(&ffStopFuncDef, ffStopCallFunc);
    iocshRegister(&ffRestartFuncDef, ffRestartCallFunc);
    iocshRegister(&ffFreeListsFuncDef, ffFreeListsCallFunc);
    iocshRegister(&ffdbAddrFuncDef, ffdbAddrCallFunc);
}
epicsExportRegistrar(ffRegistrar);


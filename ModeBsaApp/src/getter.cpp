//Getter.cpp

#include "getter.h"
#include <unistd.h>

static epicsEventId EVRFireEvent = NULL;
static int testing_var = 4;

void hxrTask(void *driverPointer);
void sxrTask(void *driverPointer);
//void EVRFireTest(void* pBlobSet);

/*
 * This function will be registered with EVR for callback each fiducial at 360hz
 * The argument is the bldBlobSet ptr allocated by fcomAllocBlobSet()
 */
// void EVRFire( void * pBlobSet )
void EVRFireTest(void*)
{
  errlogPrintf("Testing to see if anything is happening in EVRFireTest");
  printf("Is this a print problem");
  //std::cout << "Is this being called at all" << std::endl;
  testing_var = 5;
	epicsTimeStamp time40;
	int		fid40, fidpipeline;
	unsigned long long	tscLast;
	/* evrRWMutex is locked while calling these user functions so don't do anything that might block. */
	epicsTimeStamp time_s;

	/* get the current pattern data - check for good status */
	evrModifier_ta modifier_a;
	epicsUInt32  patternStatus; /* see evrPattern.h for values */
	int status = evrTimeGetFromPipeline(&time_s,  evrTimeCurrent, modifier_a, &patternStatus, 0,0,0);
	if ( status != 0 )
	{
		/* Error from evrTimeGetFromPipeline! */
		// bldFiducialTime.nsec = PULSEID_INVALID;
    std::cout << "Status wasn't 0, evrTimeGetFromPipeline failed" << std::endl;
		return;
	}
#if defined(BLD_SXR)
	/* check for No LCLS SXR beam */
	if ( (modifier_a[MOD3_IDX] & BKRCUS) == 0)
#else
	/* check for No LCLS HXR beam */
	if ( (modifier_a[MOD5_IDX] & MOD5_BEAMFULL_MASK) == 0)
#endif
	{
		/* This is 360Hz. So printf will really screw timing. Only enable briefly */
		// if(BLD_MCAST_DEBUG >= 6)
    //         errlogPrintf("EVR fires (status %i, mod5 0x%08x, fid %d)\n",
    //                 status, (unsigned)modifier_a[MOD5_IDX], PULSEID(time_s) );
		/* No beam */
		//return;
	}

	fidpipeline = PULSEID(time_s);
	tscLast	= evrGetFiducialTsc();
	epicsTimeGetEvent( &time40, 40 );
	fid40 = PULSEID(time40);

	/* Get timestamps for beam fiducial */
	// bldFiducialTime = time_s;
	// bldFiducialTsc	= GetHiResTicks();

	// if(BLD_MCAST_DEBUG >= 5) {
	// 	double			deltaLastFid;
	// 	deltaLastFid	= HiResTicksToSeconds( bldFiducialTsc - tscLast   ) * 1e3;
	// 	errlogPrintf( "pipeline fid %d (-%0.2f)\n", fidpipeline, deltaLastFid );
	// /* HACK */
	// 	return;
	// }

	/* This is 120Hz. So printf will screw timing. Only enable briefly. */
	// if(BLD_MCAST_DEBUG >= 4) errlogPrintf("EVR fires (status %i, mod5 0x%08x, fid %d, fid40 %d)\n", status, (unsigned)modifier_a[4], fidpipeline, fid40 );

	/* Signal the EVRFireEvent to trigger the fcomGetBlobSet call */
  std::cout << "Did we get to the Event Signal?" << std::endl;
	epicsEventSignal( EVRFireEvent);

// #ifdef RTEMS
// 	if ( pBlobSet == NULL ) {
// 		BSP_timer_start( BSPTIMER, timer_delay_clicks );
// 	}
// #endif
	return;
}


GetterDriver::GetterDriver(const char *portName): asynPortDriver(
    portName,
    1,
    asynDrvUserMask | asynInt32ArrayMask | asynInt16ArrayMask | asynUInt32DigitalMask | asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask,
    asynInt32ArrayMask | asynInt16ArrayMask | asynInt32Mask | asynUInt32DigitalMask | asynFloat64Mask | asynFloat64ArrayMask,
    0,
    1,
    0,
    0
    )
{
  // int initialization_status = evrInitialize();
  // if (!initialization_status) 
  // {
  //   std::cout << "Initialization was successful" << std::endl;
  // }

  createParam("HXR_STATE", asynParamInt32, &hxr_state_idx);
  createParam("SXR_STATE", asynParamInt32, &sxr_state_idx);

  createParam("SHUTTER", asynParamInt32, &shutter_idx);
  createParam("BCS_FAULT", asynParamInt32, &bcs_fault_idx);
  createParam("GUN_OFF", asynParamInt32, &gun_off_idx);
  createParam("GUN_RATE", asynParamFloat64, &gun_rate_idx);

  createParam("HXR_PERMIT", asynParamFloat64, &hxr_permit_idx);
  createParam("HARD_INJRATE", asynParamFloat64, &hard_injrate_idx);

  createParam("SXR_PERMIT", asynParamFloat64, &sxr_permit_idx);
  createParam("SOFT_INJRATE", asynParamFloat64, &soft_injrate_idx);

  createParam("SPECTROMETER_STATE", asynParamInt32, &spectrometer_state_idx);
  createParam("TD_11_IN", asynParamInt32, &td_11_in_idx);

  createParam("D2_IN_A", asynParamInt32, &d2_in_1_idx);
  createParam("D2_IN_B", asynParamInt32, &d2_in_2_idx);
  createParam("BYKIK", asynParamInt32, &bykik_idx);
  createParam("TDUND_IN", asynParamInt32, &tdund_in_idx);

  createParam("ST_CLTS_IN_A", asynParamInt32, &st_clts_in_1_idx);
  createParam("ST_CLTS_IN_B", asynParamInt32, &st_clts_in_2_idx);
  createParam("BYKIKS", asynParamInt32, &bykiks_idx);
  createParam("TDUNDB_IN", asynParamInt32, &tdundb_in_idx);

  asynStatus status_hxr;
  status_hxr = (asynStatus)(epicsThreadCreate("HXRGetterTask", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::hxrTask, this) == NULL);
  if (status_hxr)
  {
    printf("HXR thread shot itself for some reason");
    return;
  }

  asynStatus status_sxr;
  status_sxr = (asynStatus)(epicsThreadCreate("SXRGetterTask", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::sxrTask, this) == NULL);
  if (status_sxr)
  {
    printf("SXR thread shot itself for some reason");
    return;
  }
}

void hxrTask(void *driverPointer)
{
  GetterDriver *pPvt = (GetterDriver *) driverPointer;
  pPvt->hxrTask();
}

void GetterDriver::hxrTask(void)
{
  int state = 0;
  int shutter;
  int bcs_fault;
  int gun_off;
  double gun_rate;
  double hxr_permit;
  double hard_injrate;

  std::cout << "After all that" << std::endl;

  epicsTimeStamp epics_time_previous;
  epics_time_previous.secPastEpoch = 0;
  epics_time_previous.nsec = 0;
  epicsTimeStamp epics_time_next;
  //int val = evrTimeGet(&epics_time_next, 40); 
  epicsTimeStamp epics_time_current;
  unsigned int event_code = 0;
  const unsigned int EPSILON = 300; // 5 minutes
  //epicsTimeGetCurrent(&epics_time_actual);

  // epicsTimeStamp time_s;
  // evrModifier_ta modifier_a;
  // epicsUInt32  patternStatus;
  //evrTimeCurrent = 0;
//   typedef enum {
//   evrTimeCurrent=0, evrTimeNext1=1, evrTimeNext2=2, evrTimeNext3=3, evrTimeActive
// } evrTimeId_te;
	/* All ready to go, create event and register with EVR */
	EVRFireEvent = epicsEventMustCreate(epicsEventEmpty);

	/* Register EVRFire */
	// evrTimeRegister(EVRFire, bldBlobSet);
  int function_registration = evrTimeRegister(EVRFireTest, NULL);
  std::cout << "Func registration: " << function_registration << std::endl;
  if (function_registration != 0) {std::cout << "Registration didn't work" << std::endl;}
  else {std::cout << "All good with evr function registration" << std::endl;}

  // EVRFireTest(bldBlobSet);

  // fiducial event
  epicsTimeStamp time1;

  std::cout << "The value of testing var is: " << testing_var << std::endl;

  // std::cout << "Manually calling evrTask now" << std::endl;
  // evrTask();
  // std::cout << "After calling evr Task" << std::endl;

  std::cout << "Printing all threads here: " << std::endl;
  epicsThreadShowAll(2);

  while (true)
  {
    epicsTimeGetEvent( &time1, 1 );
    //std::cout << "Fiducial time: " << time1.secPastEpoch << std::endl;
    //sleep(5);
    // int status = evrTimeGetFromPipeline(&time_s,  evrTimeCurrent, modifier_a, &patternStatus, 0,0,0);
    // std::cout << "Updated time: " << time_s.secPastEpoch << std::endl;
    // evrTimeGet(&epics_time_next, event_code);
    // std::cout << "epics_time_next: " << epics_time_next.secPastEpoch << std::endl;
    // sleep(5);
    // epicsTimeGetCurrent(&epics_time_current);
    // std::cout << "After time get current" << std::endl;
    // evrTimeGet(&epics_time_next, event_code);
    // epicsUInt32 time_diff = epics_time_next.secPastEpoch - epics_time_previous.secPastEpoch;
    // epicsUInt32 actual_time_diff = epics_time_current.secPastEpoch - epics_time_next.secPastEpoch;
    // std::cout << "epics_time_current: " << epics_time_current.secPastEpoch << " and epics_time_next: " << epics_time_next.secPastEpoch << std::endl;
    // epics_time_previous = epics_time_next;

    // std::cout << "Time diff: " << time_diff << " and actual time diff: " << actual_time_diff << std::endl;
     
    // if (time_diff == 0 || actual_time_diff > EPSILON) {sleep(5); continue;} // SHOULD WE ADD A SLEEP HERE SO IT DOESN'T RUN TOO OFTEN

    int status;
		{
			status = epicsEventWaitWithTimeout(EVRFireEvent, DEFAULT_EVR_TIMEOUT);
			if(status != epicsEventWaitOK)
			{
				if(status == epicsEventWaitTimeout)
				{
					// if(BLD_MCAST_DEBUG >= 3) 
          errlogPrintf("Timed out waiting for Beam event\n");
				}
				else
				{
					errlogPrintf("Wait EVR Error, what happened? Let's sleep 2 seconds.\n");
					epicsThreadSleep(2.0);
				}

        //std::cout << "Status wasn't okay, next iteration" << std::endl;

				continue;
			}
      std::cout << "Status was okay, now proceeding" << std::endl;
		}

    getIntegerParam(shutter_idx, &shutter);
    getIntegerParam(bcs_fault_idx, &bcs_fault);
    getIntegerParam(gun_off_idx, &gun_off);
    getDoubleParam(gun_rate_idx, &gun_rate);
    getDoubleParam(hxr_permit_idx, &hxr_permit);
    getDoubleParam(hard_injrate_idx, &hard_injrate);
    
    if (not(shutter == 1 or bcs_fault == 0 or gun_off == 1 or gun_rate == 0 or hxr_permit == 1 or hard_injrate == 0))
    {
      int spectrometer_state;
      int td_11_in;
      int d2_in_1;
      int d2_in_2;
      int bykik;
      int tdund_in;

      getIntegerParam(spectrometer_state_idx, &spectrometer_state);
      getIntegerParam(td_11_in_idx, &td_11_in);
      getIntegerParam(d2_in_1_idx, &d2_in_1);
      getIntegerParam(d2_in_2_idx, &d2_in_2);
      getIntegerParam(bykik_idx, &bykik);
      getIntegerParam(tdund_in_idx, &tdund_in);

      if (spectrometer_state == 0)
      {
        state = 1;
      }
      else if (td_11_in == 2) 
      {
        state = 2;
      }
      else if (d2_in_1 == 0 or d2_in_2 == 0)
      {
        state = 3;
      }
      else if (bykik == 0)
      {
        state = 4;
      }
      else if (tdund_in == 2)
      {
        state = 5;
      }
      else 
      {
        state = 6;  
      }
    }

    //Set param of state
    setIntegerParam(hxr_state_idx, state);
    callParamCallbacks();
  }
}

void sxrTask(void *driverPointer)
{
  GetterDriver *pPvt = (GetterDriver *) driverPointer;
  pPvt->sxrTask();
}

void GetterDriver::sxrTask(void)
{
  int state = 0;
  int shutter;
  int bcs_fault;
  int gun_off;
  double gun_rate;
  double sxr_permit;
  double soft_injrate;

  getIntegerParam(shutter_idx, &shutter);
  getIntegerParam(bcs_fault_idx, &bcs_fault);
  getIntegerParam(gun_off_idx, &gun_off);
  getDoubleParam(gun_rate_idx, &gun_rate);
  getDoubleParam(sxr_permit_idx, &sxr_permit);
  getDoubleParam(soft_injrate_idx, &soft_injrate);
  
  if (not(shutter == 1 or bcs_fault == 0 or gun_off == 1 or gun_rate == 0 or sxr_permit == 1 or soft_injrate == 0))
  {
    int spectrometer_state;
    int td_11_in;
    int st_clts_in_1;
    int st_clts_in_2;
    int bykiks;
    int tdundb_in;

    getIntegerParam(spectrometer_state_idx, &spectrometer_state);
    getIntegerParam(td_11_in_idx, &td_11_in);
    getIntegerParam(st_clts_in_1_idx, &st_clts_in_1);
    getIntegerParam(st_clts_in_2_idx, &st_clts_in_2);
    getIntegerParam(bykiks_idx, &bykiks);
    getIntegerParam(tdundb_in_idx, &tdundb_in);

    if (spectrometer_state == 0)
    {
      state = 1;
    }
    else if (td_11_in == 2) 
    {
      state = 2;
    }
    else if (st_clts_in_1 == 0 or st_clts_in_2 == 0)
    {
      state = 3;
    }
    else if (bykiks == 0)
    {
      state = 4;
    }
    else if (tdundb_in == 2)
    {
      state = 5;
    }
    else 
    {
      state = 6;  
    }
  }

  //Set param of state
  setIntegerParam(sxr_state_idx, state);
  callParamCallbacks();
}




extern "C" {
  int GetterDriverConfigure(const char* portName) {
    new GetterDriver(portName);
    return asynSuccess;
  }
  static const iocshArg getterArg0 ={"portName", iocshArgString};
  static const iocshArg * const getterArgs[] = {&getterArg0};
  static const iocshFuncDef getterFuncDef = {"GetterDriverConfigure", 1, getterArgs};
  static void getterCallFunc(const iocshArgBuf *args)
  {
    GetterDriverConfigure(args[0].sval);
  }
  void GetterDriverRegister(void) {
    iocshRegister(&getterFuncDef, getterCallFunc);
  }
  epicsExportRegistrar(GetterDriverRegister);
}


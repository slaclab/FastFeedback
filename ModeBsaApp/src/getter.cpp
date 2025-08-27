//Getter.cpp

#include "getter.h"
#include <unistd.h>

static epicsEventId EVRFireEvent = NULL;
static epicsTimeStamp time40;

void xrayTask(void *driverPointer);
void EVRFireTest(void*);


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

  _bsaHXRChannel = BSA_CreateChannel("HXR_CHANNEL");
  _bsaSXRChannel = BSA_CreateChannel("SXR_CHANNEL");

  asynStatus status_xray;
  status_xray = (asynStatus)(epicsThreadCreate("XRAYGetterTask", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::xrayTask, this) == NULL);
  if (status_xray)
  {
    printf("X-ray thread shot itself for some reason");
    return;
  }
}

void xrayTask(void *driverPointer)
{
  GetterDriver *pPvt = (GetterDriver *) driverPointer;
  pPvt->xrayTask();
}

void GetterDriver::xrayTask(void)
{
  int hxr_state = 0;
  int sxr_state = 0;

  int shutter;
  int bcs_fault;
  int gun_off;
  double gun_rate;

  double hxr_permit;
  double hard_injrate;

  double sxr_permit;
  double soft_injrate;

  int spectrometer_state;
  int td_11_in;

	/* All ready to go, create event and register with EVR */
	EVRFireEvent = epicsEventMustCreate(epicsEventEmpty);

	/* Register EVRFire */
  int function_registration = evrTimeRegister(EVRFireTest, NULL);
  if (function_registration != 0) {std::cout << "Registration didn't work" << std::endl;}
  else {std::cout << "All good with evr function registration" << std::endl;}

  while (true)
  {
    int status;
		{
			status = epicsEventWaitWithTimeout(EVRFireEvent, DEFAULT_EVR_TIMEOUT);
			if(status != epicsEventWaitOK)
			{
				if(status == epicsEventWaitTimeout)
				{
          errlogPrintf("Timed out waiting for Beam event\n");
				}
				else
				{
					errlogPrintf("Wait EVR Error, what happened? Let's sleep 2 seconds.\n");
					epicsThreadSleep(2.0);
				}

				continue;
			}
		}

    getIntegerParam(shutter_idx, &shutter);
    getIntegerParam(bcs_fault_idx, &bcs_fault);
    getIntegerParam(gun_off_idx, &gun_off);
    getDoubleParam(gun_rate_idx, &gun_rate);

    getDoubleParam(hxr_permit_idx, &hxr_permit);
    getDoubleParam(hard_injrate_idx, &hard_injrate);

    getDoubleParam(sxr_permit_idx, &sxr_permit);
    getDoubleParam(soft_injrate_idx, &soft_injrate);

    // check conditions for HXR
    if (not(shutter == 1 or bcs_fault == 0 or gun_off == 1 or gun_rate == 0 or hxr_permit == 1 or hard_injrate == 0))
    {
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
        hxr_state = 1;
      }
      else if (td_11_in == 2) 
      {
        hxr_state = 2;
      }
      else if (d2_in_1 == 0 or d2_in_2 == 0)
      {
        hxr_state = 3;
      }
      else if (bykik == 0)
      {
        hxr_state = 4;
      }
      else if (tdund_in == 2)
      {
        hxr_state = 5;
      }
      else 
      {
        hxr_state = 6;  
      }
    }

    // check conditions for SXR
    if (not(shutter == 1 or bcs_fault == 0 or gun_off == 1 or gun_rate == 0 or sxr_permit == 1 or soft_injrate == 0))
    {
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
        sxr_state = 1;
      }
      else if (td_11_in == 2) 
      {
        sxr_state = 2;
      }
      else if (st_clts_in_1 == 0 or st_clts_in_2 == 0)
      {
        sxr_state = 3;
      }
      else if (bykiks == 0)
      {
        sxr_state = 4;
      }
      else if (tdundb_in == 2)
      {
        sxr_state = 5;
      }
      else 
      {
        sxr_state = 6;  
      }
    }

    //Set param of state
    setIntegerParam(hxr_state_idx, hxr_state);
    setIntegerParam(sxr_state_idx, sxr_state);
    callParamCallbacks();

    double hxr_state_double = (double)hxr_state;
    double sxr_state_double = (double)sxr_state;

    int return_status_hxr = BSA_StoreData(_bsaHXRChannel, time40, hxr_state_double, epicsAlarmNone, epicsSevNone);
    if (return_status_hxr) {std::cout << "BSA_StoreData failed for HXR" << std::endl;}

    int return_status_sxr = BSA_StoreData(_bsaSXRChannel, time40, sxr_state_double, epicsAlarmNone, epicsSevNone);
    if (return_status_sxr) {std::cout << "BSA_StoreData failed for SXR" << std::endl;}

    // RELEASE CHANNELS HERE?
    //BSA_ReleaseChannel(_bsaHXRChannel);
    //BSA_ReleaseChannel(_bsaSXRChannel);
  }
}

/*
 * This function will be registered with EVR for callback each fiducial at 360hz
 */
void EVRFireTest(void*)
{
	// /* evrRWMutex is locked while calling these user functions so don't do anything that might block. */

	// /* get the current pattern data - check for good status */
	evrModifier_ta modifier_a;
	epicsUInt32  patternStatus; /* see evrPattern.h for values */
	int status = evrTimeGetFromPipeline(&time40,  evrTimeCurrent, modifier_a, &patternStatus, 0,0,0);
	if ( status != 0 )
	{
		/* Error from evrTimeGetFromPipeline! */
    std::cout << "Status wasn't 0, evrTimeGetFromPipeline failed" << std::endl;
		return;
	}

  // indicates the presence of event 40
  if (((modifier_a[MOD2_IDX] & TIMESLOT1_MASK) || (modifier_a[MOD2_IDX] & TIMESLOT4_MASK)) == 1)
  {
    /* Signal the EVRFireEvent */
    epicsEventSignal( EVRFireEvent);
    return;
  }
  
	return;
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


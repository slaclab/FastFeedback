//Getter.cpp

#include "getter.h"
#include <unistd.h>

void hxrTask(void *driverPointer);
void sxrTask(void *driverPointer);

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

  // epicsTimeStamp bsaTS;
	// evrTimeGet(&bsaTS, 0);  

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
  // int val = evrTimeGet(&epics_time_current, 0); 
  epicsTimeStamp epics_time_current;
  unsigned int event_code = 40;
  const unsigned int EPSILON = 300; // 5 minutes
  //epicsTimeGetCurrent(&epics_time_actual);



  while (true)
  {
    epicsTimeGetCurrent(&epics_time_current);
    // std::cout << "After time get current" << std::endl;
    evrTimeGet(&epics_time_next, event_code);
    epicsUInt32 time_diff = epics_time_next.secPastEpoch - epics_time_previous.secPastEpoch;
    epicsUInt32 actual_time_diff = epics_time_current.secPastEpoch - epics_time_next.secPastEpoch;
    std::cout << "epics_time_current: " << epics_time_current.secPastEpoch << " and epics_time_next: " << epics_time_next.secPastEpoch << std::endl;
    epics_time_previous = epics_time_next;

    std::cout << "Time diff: " << time_diff << " and actual time diff: " << actual_time_diff << std::endl;
     
    if (time_diff == 0 || actual_time_diff > EPSILON) {sleep(5); continue;} // SHOULD WE ADD A SLEEP HERE SO IT DOESN'T RUN TOO OFTEN

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


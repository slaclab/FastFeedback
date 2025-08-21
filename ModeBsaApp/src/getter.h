//Getter.h 

#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>

#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>
#include <evrTime.h>
#include <evrPattern.h>
#include <epicsTime.h>
#include <errlog.h>
//#include "PatternManager.h"

using namespace std;

#define DEFAULT_EVR_TIMEOUT	0.2		/* Default EVR event timeout, for 30Hz */

class GetterDriver : public asynPortDriver {
  public:
    GetterDriver(const char *portName);
    void hxrTask(void);
    void sxrTask(void);

  protected:

    int hxr_state_idx;
    int sxr_state_idx;

    int shutter_idx;
    int bcs_fault_idx;
    int gun_off_idx;
    int gun_rate_idx;

    int hxr_permit_idx;
    int hard_injrate_idx;

    int sxr_permit_idx;
    int soft_injrate_idx;

    int spectrometer_state_idx;
    int td_11_in_idx;

    int d2_in_1_idx;
    int d2_in_2_idx;
    int bykik_idx;
    int tdund_in_idx;

    int st_clts_in_1_idx;
    int st_clts_in_2_idx;
    int bykiks_idx;
    int tdundb_in_idx;
};

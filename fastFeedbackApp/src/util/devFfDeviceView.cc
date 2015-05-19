
#include <dbCommon.h>
#include <devSup.h>
#include <recGbl.h>
#include <aiRecord.h>
#include <epicsExport.h>

#include "Defs.h"
#include "PatternManager.h"
#include "ExecConfiguration.h"
#include "DeviceView.h"

USING_FF_NAMESPACE

/**
 * FIXME: This is defined here because there are symbol redefinitions
 * in the epics headers -> I couldn't figure out how to include this
 * one without redefining other symbols.
 */
#define FF_S_db_badField (501 <<16) | 15

/**
 * Struct defining functions for ai FfDeviceView support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ai;
    DEVSUPFUN special_linconv;
} devAiFfDevViewFunctions;

/**
 * Definition of functions for ai FfDeviceView support
 */
static long devAiFfDeviceView_init_record(aiRecord *precord);
static long devAiFfDeviceView_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *pvt_ps);
static long devAiFfDeviceView_read_ai(aiRecord *precord);

/**
 * Instantiation of data structure holding functions for handling AI FfDeviceView
 * support.
 */
devAiFfDevViewFunctions devAiFfDeviceView = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devAiFfDeviceView_init_record,
    (DEVSUPFUN) devAiFfDeviceView_get_ioint_info,
    (DEVSUPFUN) devAiFfDeviceView_read_ai,
    NULL
};

epicsExportAddress(dset, devAiFfDeviceView);

/**
 * Parse the INST_IO string containing parameters for the FfDeviceView
 * device type. The expected format is:
 *
 *   "[loop name] [device] [pattern index]"
 *
 * If [pattern index] is 0, then the DeviceView will monitor data
 * for all patterns, otherwise only the data for a particular pattern, given
 * by the index.
 *
 * The [device] is the device name (e.g. M1, A2, S3). The example names are
 * used for devices that monitor data from all patterns. For devices using
 * specific patterns (e.g. M1P1, M1P2) event though the PV name has the suffix
 * P1, P2, P3 or P4, the INST_IO [device] should be only M1 (or equivalent).
 *
 * @param precord pointer to the record from which the INST_IO parameter string
 * is read from
 * @param loopName name of the loop extracted from the instio record field
 * @author L.Piccoli
 */
static long parseInstioString(char *configString, char *loopName,
        char *deviceName, int *patternIndex, int *deviceIndex) {

    long status;
    status = sscanf(configString, "%s %s %d", loopName, deviceName, patternIndex);
    if (status < 0) {
        return status;
    }

    // The deviceIndex is within the deviceName, which is in the format
    // [A,M,S][index]
    *deviceIndex = atoi(&deviceName[1]);
    if (*deviceIndex < 1 || *deviceIndex > 40) {
        return -1;
    }

    return 0;
}

/**
 * Create a DeviceView and attach it to the specified record. The DeviceView
 * configuration is parsed from the records INST_IO string.
 *
 * If the DeviceView is created (all INST_IO parameters are correct) and
 * assigned to the records dpvt fielt.
 *
 * The DeviceView is accessed after a feedback cycle finishes (i.e. Loop
 * actuators receive new settings). The read_ai device support routine
 * updates the DeviceView with the current pattern and saves the latest value
 * to the record val field.
 *
 * @param precord pointer to the record that will receive the DeviceView
 * @return a valid DeviceView on success or NULL on failure
 * @author L.Piccoli
 */
static DeviceView *createDeviceView(char *deviceConfig) {
    char loopName[20];
    char deviceName[20];
    int patternIndex = -1;
    int deviceIndex = -1;

    if (parseInstioString(deviceConfig, &(loopName[0]), &(deviceName[0]), &patternIndex,
            &deviceIndex) != 0) {
        return NULL;
    }

    LoopConfiguration *loopConfig = ExecConfiguration::getInstance().getLoopConfiguration(loopName);
    if (loopConfig == NULL) {
        std::cerr << "ERROR: Failed to find LoopConfiguration for Loop \""
                << loopName << "\"" << std::endl;
        return NULL;
    }

    DeviceView *deviceView = NULL;
    // Device can be 'M' for Measurement, 'A' for actuator, 'S' for State
    char deviceType = deviceName[0];
    switch (deviceType) {
        case 'M':
            deviceView = loopConfig->createDeviceView<MeasurementMap,
                    MeasurementMap::iterator, MeasurementSet, MeasurementSet::iterator,
                    MeasurementDevice >
	      (loopConfig->_measurements, deviceName, patternIndex, deviceIndex);
            break;
        case 'A':
            deviceView = loopConfig->createDeviceView<ActuatorMap,
                    ActuatorMap::iterator, ActuatorSet, ActuatorSet::iterator,
                    ActuatorDevice >
	      (loopConfig->_actuators, deviceName, patternIndex, deviceIndex);
            break;
        case 'S':
            deviceView = loopConfig->createDeviceView<StateMap,
                    StateMap::iterator, StateSet, StateSet::iterator,
                    StateDevice >
                    (loopConfig->_states, deviceName, patternIndex, deviceIndex);
            break;
        default:
            std::cerr << "ERROR: Invalid device type \"" << deviceType
                    << "\", expected M, A or S." << std::endl;
            return NULL;
    }

    if (deviceView == NULL) {
        std::cerr << "ERROR: NULL DeviceView for " << deviceConfig << std::endl;
        return NULL;
    }

    // Initialize the device support scan list
    scanIoInit(&(loopConfig->_scanlist));

    return deviceView;
}

/**
 * Initialize the Analog Input (Ai) record for the DeviceView.
 * Gets the parameters passed for the record through the INP field.
 * The string is expected to have the following format:
 *
 *   "[loop name] [device] [pattern index]"
 *
 * For example, the first measurement device for the loop TR01
 * should have the following INP string:
 *
 *   "TR01 M1 0"
 *
 * Where pattern index of 0 means that the device should be monitored
 * for all patterns. If pattern index is greater than 0 then the
 * device is monitored for the specified pattern only.
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devAiFfDeviceView_init_record(aiRecord *precord) {
    long status = -1;

    DeviceView *deviceView = NULL;
    switch (precord->inp.type) {
        case INST_IO:
            deviceView = createDeviceView(precord->inp.value.instio.string);
            // Set the dpvt field in the record to point to the newly created
            // DeviceView. When the device is scanned the new data is retrieved from the
            // DeviceView.
            if (deviceView != NULL) {
                precord->udf = FALSE;
                precord->dpvt = (void*) deviceView;
                status = 0;
            } else {
                std::cerr << "ERROR: Failed to create DeviceView for input \""
                        << precord->inp.value.instio.string << "\"" << std::endl;
                status = -1;
            }
            break;
        default:
            precord->val = 0.;
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * Return the scanlist (saved in the LoopConfiguration) for the specified
 * record.
 *
 * @param cmd (not used)
 * @param precord pointer to the record being set up
 * @param pvt_ps returning parameter, containing the pointer to the scanlist
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
static long devAiFfDeviceView_get_ioint_info(int cmd, struct dbCommon *precord,
        IOSCANPVT *pvt_ps) {
    DeviceView *deviceView = reinterpret_cast<DeviceView *> (precord->dpvt);

    if (deviceView == NULL) {
      std::cout << "ERROR: precord->dpvt is NULL" << std::endl;
        return -1;
    }

    LoopConfiguration *loopConfig = ExecConfiguration::getInstance().
            getLoopConfiguration(deviceView->getLoopName());
    if (loopConfig == NULL) {
        std::cerr << "ERROR: Failed to find LoopConfiguration for record \""
                <<  precord->name << "\", Loop \"" << deviceView->getLoopName()
                << "\"" << std::endl;
        return -1;
    }
    *pvt_ps = loopConfig->_scanlist;

    return 0;
}

/**
 * This function is called by the LoopThread at the end of a feedback cycle.
 * It is indirectly invoked by calling the function ''scanIoRequest'', which
 * signals the ChannelAccess threads to call this function to update the record.
 *
 * The DeviceView associated with the record is updated here. The current
 * pattern processed by the PatternManager is passed to the DeviceView update()
 * method. The pattern is needed because there are DeviceViews that are updated
 * only when a predefined pattern is received.
 *
 * The most recent value from the DeviceView is then copied into the record
 * val field.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devAiFfDeviceView_read_ai(aiRecord *precord) {
    DeviceView *deviceView = reinterpret_cast<DeviceView *> (precord->dpvt);

    if (deviceView == NULL) {
      precord->val = 999.;//0.;
        return 2;
    }
    deviceView->update(PatternManager::getInstance().getCurrentPattern());
    double value = deviceView->get();
    precord->val = value;

    return 2; /* no conversion */
}

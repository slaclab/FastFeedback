#include <iostream>

#include <alarm.h>
#include <dbCommon.h>
#include <devSup.h>
#include <recGbl.h>
#include <aiRecord.h>
#include <boRecord.h>
#include <biRecord.h>
#include <aoRecord.h>
#include <stringoutRecord.h>
#include <stringinRecord.h>
#include <waveformRecord.h>
#include <longinRecord.h>
#include <longoutRecord.h>
#include <calcoutRecord.h>
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
 * TODO: investigate use of templates here
 */

/** Binary Output Device Support **********************************************/

/**
 * Struct defining functions for bo FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write_bo;
    DEVSUPFUN special_linconv;
} devBoFfConfigFunctions;

static long devBoFfConfig_init_record(boRecord *precord);
static long devBoFfConfig_write_bo(boRecord *precord);

/**
 * Instantiation of data structure holding functions for handling bo FfConfig
 * support.
 */
devBoFfConfigFunctions devBoFfConfig = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devBoFfConfig_init_record,
    NULL,
    (DEVSUPFUN) devBoFfConfig_write_bo,
    NULL
};

epicsExportAddress(dset, devBoFfConfig);

/**
 * Initialize the Binary Output (BO) record. The precord->dpvt pointer
 * is set to the memory location of the controlled PvDataBool. The correct
 * PvDataBool is found based on the INST_IO string defined for the PV in the
 * database. The string (following the @ sign) must be the same string used
 * to initialize the PvDataBool (e.g. "TR01 MODE" -> for the enabling/disabling
 * the TR01 loop).
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devBoFfConfig_init_record(boRecord *precord) {
    long status = -1;

    std::string pvName;
    PvMapBool::iterator it;
    switch (precord->out.type) {
        case INST_IO:
            pvName = precord->out.value.instio.string;
            it = PvDataBool::getPvMap().find(pvName);
            if (it == PvDataBool::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found." << std::endl;
                status = -1;
            } else {
                precord->udf = FALSE;
                precord->dpvt = it->second;
                // now create the mutex for this pvData
                // BO records have mutex to protect state and mode PVs
                std::vector<PvDataBool *> *vector =
                        reinterpret_cast<std::vector<PvDataBool *> *> (precord->dpvt);
                try {
                  for (int i = 0; i < (int) vector->size(); ++i) {
                    vector->at(i)->createMutex();
                  }
                  status = 0;
                } catch (std::out_of_range& e) {
                    std::cerr << "ERROR: " << e.what() << std::endl;
                    status = -1;
                }
            }
            break;

        default:
            precord->val = 0;
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * This function is invoked when a new value is written to the PV. This new
 * value is copied over to the address location defined in the init_record
 * function.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devBoFfConfig_write_bo(boRecord *precord) {
    if (!precord->dpvt) {
        return 0;
    }

    std::vector<PvDataBool *> *vector =
            reinterpret_cast<std::vector<PvDataBool *> *> (precord->dpvt);
    try {
        for (int i = 0; i < (int) vector->size(); ++i) {
            bool newValue = false; // false is equivalent to 0
            if (precord->rval != 0) {
                newValue = true; // non-zero values are true
            }
            vector->at(i)->write(&newValue);
        }
    } catch (std::out_of_range& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

/** Analog Input (AI) Device Support *****************************************************/

/**
 * Struct defining functions for ai FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ai;
    DEVSUPFUN special_linconv;
} devAiFfConfigFunctions;

static long devAiFfConfig_init_record(aiRecord *precord);
static long devAiFfConfig_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *pvt_ps);
static long devAiFfConfig_read_ai(aiRecord *precord);

/**
 * Instantiation of data structure holding functions for handling ai
 * FfConfig support.
 */
devAiFfConfigFunctions devAiFfConfig = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devAiFfConfig_init_record,
    (DEVSUPFUN) devAiFfConfig_get_ioint_info,
    (DEVSUPFUN) devAiFfConfig_read_ai,
    NULL
};

epicsExportAddress(dset, devAiFfConfig);

/**
 * Initialize the Analog Input (AI) record. The precord->dpvt pointer
 * is set to the memory location of a double attribute the controlled PvDataDouble.
 * The correct PvDataDouble instance is found based on the INST_IO string
 * defined for the PV in the database. The string (following the @ sign) must
 * be the same string used to initialize the PvDataDouble (e.g.
 * "LG01 A1HIHIIN" -> for setting the HIHI alarm limit for A1).
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devAiFfConfig_init_record(aiRecord *precord) {
    long status = -1;

    std::string pvName;
    PvMapDouble::iterator it;
    switch (precord->inp.type) {
        case INST_IO:
	        pvName = precord->inp.value.instio.string;
            it = PvDataDouble::getPvMap().find(pvName);
            if (it == PvDataDouble::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found (ai)." << std::endl;
                status = -1;
            } else {
                precord->udf = FALSE;
                precord->dpvt = it->second;
                std::vector<PvDataDouble *> *doubles = it->second;
                precord->dpvt = doubles->at(0);
                if (doubles->size() > 0) {
                    doubles->at(0)->initScanList();
                    status = 0;
                } else {
                    status = -1;
                }
            }
            break;

        default:
            precord->val = 0;
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
static long devAiFfConfig_get_ioint_info(int cmd, struct dbCommon *precord,
        IOSCANPVT *pvt_ps) {
    PvDataDouble *pvDataDouble = reinterpret_cast<PvDataDouble *> (precord->dpvt);

    if (pvDataDouble != NULL) {
        IOSCANPVT scanlist = pvDataDouble->getScanList();
        *pvt_ps = scanlist;
    }

    return 0;
}

/**
 * This function is called when there is a new input. It is indirectly invoked by calling
 * the function ''scanIoRequest'', which signals the ChannelAccess threads to call this
 * function to update the record.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devAiFfConfig_read_ai(aiRecord *precord) {
	PvDataDouble *pvDataDouble = reinterpret_cast<PvDataDouble *> (precord->dpvt);

    if (pvDataDouble != NULL) {
      precord->val = pvDataDouble->getValue();
    }

    return 2;
}

/** Analog Output Device Support **********************************************/

/**
 * Struct defining functions for bo FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write_bo;
    DEVSUPFUN special_linconv;
} devAoFfConfigFunctions;

static long devAoFfConfig_init_record(aoRecord *precord);
static long devAoFfConfig_write_ao(aoRecord *precord);

/**
 * Instantiation of data structure holding functions for handling bo FfConfig
 * support.
 */
devAoFfConfigFunctions devAoFfConfig = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devAoFfConfig_init_record,
    NULL,
    (DEVSUPFUN) devAoFfConfig_write_ao,
    NULL
};

epicsExportAddress(dset, devAoFfConfig);

/**
 * Initialize the Analog Output (ao) record. The precord->dpvt pointer
 * is set to the memory location of the controlled PvDataDouble. The correct
 * PvDataDouble is found based on the INST_IO string defined for the PV in the
 * database. The string (following the @ sign) must be the same string used
 * to initialize the PvDataDouble (e.g. "TR01 A1HIHI" -> for setting the
 * high alarm limit for A1).
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devAoFfConfig_init_record(aoRecord *precord) {
    long status = -1;

    std::string pvName;
    PvMapDouble::iterator it;
    switch (precord->out.type) {
        case INST_IO:
            pvName = precord->out.value.instio.string;
            it = PvDataDouble::getPvMap().find(pvName);
            if (it == PvDataDouble::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found." << std::endl;
                status = -1;
            } else {
                precord->udf = FALSE;
                precord->dpvt = it->second;
                status = 0;
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
 * This function is invoked when a new value is written to the PV. This new
 * value is copied over to the address location defined in the init_record
 * function.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devAoFfConfig_write_ao(aoRecord *precord) {
    if (!precord->dpvt) {
        return 0;
    }

    std::vector<PvDataDouble *> *vector =
            reinterpret_cast<std::vector<PvDataDouble *> *> (precord->dpvt);
    try {
        for (int i = 0; i < (int) vector->size(); ++i) {
            vector->at(i)->write(&precord->val);
        }
    } catch (std::out_of_range& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

/** Calc Output Device Support **********************************************/

/**
 * Struct defining functions for Calcout FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write_bo;
    DEVSUPFUN special_linconv;
} devCalcoutFfConfigFunctions;

static long devCalcoutFfConfig_init_record(calcoutRecord *precord);
static long devCalcoutFfConfig_write_calcout(calcoutRecord *precord);

/**
 * Instantiation of data structure holding functions for handling Calcout
 * FfConfig support.
 */
devCalcoutFfConfigFunctions devCalcoutFfConfig = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devCalcoutFfConfig_init_record,
    NULL,
    (DEVSUPFUN) devCalcoutFfConfig_write_calcout,
    NULL
};

epicsExportAddress(dset, devCalcoutFfConfig);

/**
 * Initialize the Calcout record. The precord->dpvt pointer
 * is set to the memory location of the controlled PvDataDouble. The correct
 * PvDataDouble is found based on the INST_IO string defined for the PV in the
 * database. The string (following the @ sign) must be the same string used
 * to initialize the PvDataDouble (e.g. "TR01 A1HIHI" -> for setting the
 * high alarm limit for A1).
 *
 * @param precord pointer to the calc record being initialized
 * @author L.Piccoli
 */
static long devCalcoutFfConfig_init_record(calcoutRecord *precord) {
    long status = -1;

    std::string pvName;
    PvMapDouble::iterator it;
    switch (precord->out.type) {
        case INST_IO:
            pvName = precord->out.value.instio.string;
            it = PvDataDouble::getPvMap().find(pvName);
            if (it == PvDataDouble::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found for calcout." << std::endl;
                status = -1;
            } else {
                precord->udf = FALSE;
                precord->dpvt = it->second;
                status = 0;
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
 * This function is invoked when a new value is calculated by the PV. This new
 * value is copied over to the address location defined in the init_record
 * function.
 *
 * @param precord pointer to the calc record being processed
 * @author L.Piccoli
 */
static long devCalcoutFfConfig_write_calcout(calcoutRecord *precord) {
    if (!precord->dpvt) {
        return 0;
    }

    std::vector<PvDataDouble *> *vector =
            reinterpret_cast<std::vector<PvDataDouble *> *> (precord->dpvt);
    try {
        for (int i = 0; i < (int) vector->size(); ++i) {
            vector->at(i)->write(&precord->val);
        }
    } catch (std::out_of_range& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

/** String Output Device Support **********************************************/

/**
 * Struct defining functions for stringout FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write_stringout;
    DEVSUPFUN special_linconv;
} devStringoutFfConfigFunctions;

static long devStringoutFfConfig_init_record(stringoutRecord *precord);
static long devStringoutFfConfig_write_stringout(stringoutRecord *precord);

/**
 * Instantiation of data structure holding functions for handling stringout
 * FfConfig support.
 */
devStringoutFfConfigFunctions devStringoutFfConfig = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devStringoutFfConfig_init_record,
    NULL,
    (DEVSUPFUN) devStringoutFfConfig_write_stringout,
    NULL
};

epicsExportAddress(dset, devStringoutFfConfig);

/**
 * Initialize the String Output (Stringout) record. The precord->dpvt pointer
 * is set to the memory location of a std::string the controlled PvDataString.
 * The correct PvDataString instance is found based on the INST_IO string
 * defined for the PV in the database. The string (following the @ sign) must
 * be the same string used to initialize the PvDataString (e.g. "TR01 NAME" ->
 * for assignind a name for the TR01 loop).
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devStringoutFfConfig_init_record(stringoutRecord *precord) {
    long status = -1;

    std::string pvName;
    PvMapString::iterator it;
    switch (precord->out.type) {
        case INST_IO:
            pvName = precord->out.value.instio.string;
            it = PvDataString::getPvMap().find(pvName);
            if (it == PvDataString::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found." << std::endl;
                status = -1;
            } else {
                precord->udf = FALSE;
                precord->dpvt = it->second;
                status = 0;
            }
            break;
        default:
            precord->val[0] = '\0';
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * This function is invoked when a new value is written to the PV. This new
 * value is copied over to the address location defined in the init_record
 * function.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devStringoutFfConfig_write_stringout(stringoutRecord *precord) {
    if (!precord->dpvt) {
        return 0;
    }

    std::vector<PvDataString *> *vector =
            reinterpret_cast<std::vector<PvDataString *> *> (precord->dpvt);
    try {
        for (int i = 0; i < (int) vector->size(); ++i) {
            std::string newValue = precord->val;
            vector->at(i)->write(&newValue);
        }
    } catch (std::out_of_range& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

/** String Input Device Support **********************************************/

/**
 * Struct defining functions for stringin FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_stringin;
} devStringinFfConfigFunctions;

static long devStringinFfConfig_init_record(stringinRecord *precord);
static long devStringinFfConfig_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *pvt_ps);
static long devStringinFfConfig_read_stringin(stringinRecord *precord);

/**
 * Instantiation of data structure holding functions for handling stringin
 * FfConfig support.
 */
devStringinFfConfigFunctions devStringinFfConfig = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN) devStringinFfConfig_init_record,
    (DEVSUPFUN) devStringinFfConfig_get_ioint_info,
    (DEVSUPFUN) devStringinFfConfig_read_stringin,
};

epicsExportAddress(dset, devStringinFfConfig);

/**
 * Initialize the String Output (Stringin) record. The precord->dpvt pointer
 * is set to the memory location of a std::string the controlled PvDataString.
 * The correct PvDataString instance is found based on the INST_IO string
 * defined for the PV in the database. The string (following the @ sign) must
 * be the same string used to initialize the PvDataString (e.g. "TR01 NAME" ->
 * for assignind a name for the TR01 loop).
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devStringinFfConfig_init_record(stringinRecord *precord) {
    long status = -1;

    std::string pvName = precord->inp.value.instio.string;

    PvMapString::iterator it;
    switch (precord->inp.type) {
        case INST_IO:
            //pvName = precord->inp.value.instio.string;
            it = PvDataString::getPvMap().find(pvName);
            if (it == PvDataString::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found (si)." << std::endl;
                std::cout << "There are " << PvDataString::getPvMap().size()
                        << " registered PvData(s)" << std::endl;
                status = -1;
            } else {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" found." << std::endl;
                precord->udf = FALSE;
                std::vector<PvDataString *> *strings = it->second;
                 precord->dpvt = strings->at(0);

                // Initialize the device support scan list
                if (strings->size() > 0) {
                    strings->at(0)->initScanList();
                    status = 0;
                } else {
                    status = -1;
                }
            }
            break;
        default:
            precord->val[0] = 0;
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * Return the scanlist (saved in the PvDataString object) for the specified
 * record.
 *
 * @param cmd (not used)
 * @param precord pointer to the record being set up
 * @param pvt_ps returning parameter, containing the pointer to the scanlist
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
static long devStringinFfConfig_get_ioint_info(int cmd, struct dbCommon *precord,
        IOSCANPVT *pvt_ps) {
    PvDataString *pvDataString = reinterpret_cast<PvDataString *> (precord->dpvt);

    if (pvDataString != NULL) {
        IOSCANPVT scanlist = pvDataString->getScanList();
        *pvt_ps = scanlist;
    }

    return 0;
}

/**
 * This function is invoked when a new value is written to the PV. This new
 * value is copied over to the address location defined in the init_record
 * function.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devStringinFfConfig_read_stringin(stringinRecord *precord) {
    PvDataString *pvDataString = reinterpret_cast<PvDataString *> (precord->dpvt);

    if (pvDataString != NULL) {
        std::string stringin = pvDataString->getValue();
        stringin.copy(precord->val, MAX_STRING_SIZE);
    }

    return 2;
}

/** Binary Input Device Support **********************************************/

/**
 * Struct defining functions for Bi FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_bi;
} devBiFfConfigFunctions;

static long devBiFfConfig_init_record(biRecord *precord);
static long devBiFfConfig_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *pvt_ps);
static long devBiFfConfig_read_bi(biRecord *precord);

/**
 * Instantiation of data structure holding functions for handling Bi
 * FfConfig support.
 */
devBiFfConfigFunctions devBiFfConfig = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN) devBiFfConfig_init_record,
    (DEVSUPFUN) devBiFfConfig_get_ioint_info,
    (DEVSUPFUN) devBiFfConfig_read_bi,
};

epicsExportAddress(dset, devBiFfConfig);

/**
 * Initialize the Binari Input (Bi) record. The precord->dpvt pointer
 * is set to the memory location of a bool controlled by a PvDataBool.
 * The correct PvDataBool instance is found based on the INST_IO string
 * defined for the PV in the database. The string (following the @ sign) must
 * be the same string used to initialize the PvDataBool.
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devBiFfConfig_init_record(biRecord *precord) {
    long status = -1;

    std::string pvName = precord->inp.value.instio.string;

    PvMapBool::iterator it;
    switch (precord->inp.type) {
        case INST_IO:
            it = PvDataBool::getPvMap().find(pvName);
            if (it == PvDataBool::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found (bi)." << std::endl;
                std::cout << "There are " << PvDataBool::getPvMap().size()
                        << " registered PvData(s)" << std::endl;
                status = -1;
            } else {
                precord->udf = FALSE;
                std::vector<PvDataBool *> *bools = it->second;
                precord->dpvt = bools->at(0);

                // Initialize the device support scan list
                if (bools->size() > 0) {
                    bools->at(0)->initScanList();
                    status = 0;
                } else {
                    status = -1;
                }
            }
            break;
        default:
            precord->val = 0;
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * Return the scanlist (saved in the PvDataBool object) for the specified
 * record.
 *
 * @param cmd (not used)
 * @param precord pointer to the record being set up
 * @param pvt_ps returning parameter, containing the pointer to the scanlist
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
static long devBiFfConfig_get_ioint_info(int cmd, struct dbCommon *precord,
        IOSCANPVT *pvt_ps) {
    PvDataBool *pvDataBool = reinterpret_cast<PvDataBool *> (precord->dpvt);

    if (pvDataBool != NULL) {
        IOSCANPVT scanlist = pvDataBool->getScanList();
        *pvt_ps = scanlist;
    }

    return 0;
}

/**
 * This function is invoked when a new value is written to the PV. This new
 * value is copied over to the address location defined in the init_record
 * function.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devBiFfConfig_read_bi(biRecord *precord) {
    PvDataBool *pvDataBool = reinterpret_cast<PvDataBool *> (precord->dpvt);

    if (pvDataBool != NULL) {
      precord->val = pvDataBool->getValue();
    }

    return 2;
}

/** Waveform OUT Device Support ***********************************************/

/**
 * Struct defining functions for waveformout (wo) FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write_waveform;
} devWoFfConfigFunctions;

static long devWoFfConfig_init_record(waveformRecord *precord);
static long devWoFfConfig_write_waveform(waveformRecord *precord);

/**
 * Instantiation of data structure holding functions for handling waveform out
 * FfConfig support.
 */
devWoFfConfigFunctions devWoFfConfig = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN) devWoFfConfig_init_record,
    NULL,
    (DEVSUPFUN) devWoFfConfig_write_waveform,
};

epicsExportAddress(dset, devWoFfConfig);

/**
 * @author L.Piccoli
 */
static long devWoFfConfig_init_record(waveformRecord *precord) {
    long status = -1;

    std::string pvName = precord->inp.value.instio.string;

    if (precord->ftvl == 10) {//DBR_STS_SHORT DBF_DOUBLE) {
        PvMapDoubleWaveform::iterator it;
        switch (precord->inp.type) {
            case INST_IO:
                it = PvDataDoubleWaveform::getPvMap().find(pvName);
                if (it == PvDataDoubleWaveform::getPvMap().end()) {
                    std::cout << "PvData: \"" << pvName;
                    std::cout << "\" not found." << std::endl;
                    status = -1;
                } else {
                    precord->udf = FALSE;
                    precord->dpvt = it->second;
                    // now create the mutex for this pvData
                    // waveform out records have mutex to protect multi-byte PVs
                    std::vector<PvDataDoubleWaveform *> *vector =
                            reinterpret_cast<std::vector<PvDataDoubleWaveform *> *> (precord->dpvt);
                    try {
                      for (int i = 0; i < (int) vector->size(); ++i) {
                        vector->at(i)->createMutex();
                      }
                      status = 0;
                    } catch (std::out_of_range& e) {
                        std::cerr << "ERROR: " << e.what() << std::endl;
                        status = -1;
                    }
                    status = 0;
                }
                break;
            default:
                precord->val = 0;
        }
    } else if (precord->ftvl == 4) {//DBR_SHORT) {
        PvMapUShortWaveform::iterator it;
        switch (precord->inp.type) {
            case INST_IO:
                it = PvDataUShortWaveform::getPvMap().find(pvName);
                if (it == PvDataUShortWaveform::getPvMap().end()) {
                    std::cout << "PvData: \"" << pvName;
                    std::cout << "\" not found." << std::endl;
                    status = -1;
                } else {
                    precord->udf = FALSE;
                    precord->dpvt = it->second;
                    // waveform out records have mutex to protect multi-byte PVs
                    std::vector<PvDataUShortWaveform *> *vector =
                            reinterpret_cast<std::vector<PvDataUShortWaveform *> *> (precord->dpvt);
                    try {
                      for (int i = 0; i < (int) vector->size(); ++i) {
                        vector->at(i)->createMutex();
                      }
                      status = 0;
                    } catch (std::out_of_range& e) {
                        std::cerr << "ERROR: " << e.what() << std::endl;
                        status = -1;
                    }
                    status = 0;
                }
                break;
            default:
                precord->val = 0;
        }
    } else {
        std::cout << "PvData: \"" << pvName << "\" has invalid ftvl field. (inp.type="
                << precord->inp.type << ", ftvl=" << precord->ftvl << ")" << std::endl;
        std::cout << "DBF_DOUBLE = " << DBF_DOUBLE << std::endl;
        status = -1;
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * @author L.Piccoli
 */
static long devWoFfConfig_write_waveform(waveformRecord *precord) {
    if (precord->dpvt == NULL || precord->bptr == NULL) {
        //        std::cerr << "ERROR: invalid dpvt and bptr fields for record "
        //                << precord->name << std::endl;
        return 0;
    }

    if (precord->nord == 0) {
        return 0;
    }

    if (precord->ftvl == 10) { //DBF_DOUBLE DBR_STS_SHORT) {
        std::vector<PvDataDoubleWaveform *> *vector =
                reinterpret_cast<std::vector<PvDataDoubleWaveform *> *> (precord->dpvt);
        try {
            for (int i = 0; i < (int) vector->size(); ++i) {
                // vector->at(i)->write(newValue); // TODO: Need to override the write() method in a subclass
                double *data = reinterpret_cast<double *> (precord->bptr);
                PvDataDoubleWaveform *waveform = vector->at(i);
                std::vector<double> *waveformVector = waveform->getValueAddress();
                waveformVector->assign(data, data + precord->nord);
            }
        } catch (std::out_of_range& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return -1;
        }
    } else if (precord->ftvl == 4) {//DBR_SHORT) {
        std::vector<PvDataUShortWaveform *> *vector =
                reinterpret_cast<std::vector<PvDataUShortWaveform *> *> (precord->dpvt);
        try {
            for (int i = 0; i < (int) vector->size(); ++i) {
                unsigned short *data = reinterpret_cast<unsigned short *> (precord->bptr);
                PvDataUShortWaveform *waveform = vector->at(i);
                std::vector<unsigned short> *waveformVector = waveform->getValueAddress();
                waveformVector->assign(data, data + precord->nord);
            }
        } catch (std::out_of_range& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return -1;
        }
    } else {
        std::cerr << "ERROR: Invalid waveform type of "
                << precord->ftvl << std::endl;
        return -1;
    }
    return 0;
}

/** Waveform IN Device Support ************************************************/

/**
 * Struct defining functions for waveformout (wi) FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_waveform;
} devWiFfConfigFunctions;

static long devWiFfConfig_init_record(waveformRecord *precord);
static long devWiFfConfig_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *pvt_ps);
static long devWiFfConfig_read_waveform(waveformRecord *precord);

/**
 * Instantiation of data structure holding functions for handling stringout
 * FfConfig support.
 */
devWiFfConfigFunctions devWiFfConfig = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN) devWiFfConfig_init_record,
    (DEVSUPFUN) devWiFfConfig_get_ioint_info,
    (DEVSUPFUN) devWiFfConfig_read_waveform,
};

epicsExportAddress(dset, devWiFfConfig);

/**
 * @author L.Piccoli
 */
static long devWiFfConfig_init_record(waveformRecord *precord) {
    long status = -1;

    std::string pvName = precord->inp.value.instio.string;

    if (precord->ftvl == DBF_STRING) {
        PvMapStringWaveform::iterator it;
        switch (precord->inp.type) {
            case INST_IO:
                //pvName = precord->inp.value.instio.string;
                it = PvDataStringWaveform::getPvMap().find(pvName);
                if (it == PvDataStringWaveform::getPvMap().end()) {
                    std::cout << "PvData: \"" << pvName;
                    std::cout << "\" not found (wi)." << std::endl;
                    std::cout << "There are " << PvDataStringWaveform::getPvMap().size()
                            << " registered PvData(s)" << std::endl;
                    status = -1;
                } else {
                    std::cout << "PvData: \"" << pvName;
                    std::cout << "\" found." << std::endl;
                    precord->udf = FALSE;
                    precord->dpvt = it->second;
                    std::vector<PvDataStringWaveform *> *strWf = it->second;
                     // Init scan list for the waveform
                    if (strWf->size() > 0) {
                    	strWf->at(0)->initScanList();
                    	strWf->at(0)->setRecord(precord);
                    	status = 0;
                    } else {
                    	status = -1;
                    }
                }
                break;
            default:
                precord->val = 0;
        }
    } else if (precord->ftvl == 1) { //DBF_CHAR) {
        PvMapCharWaveform::iterator it;
        switch (precord->inp.type) {
            case INST_IO:
              it = PvDataCharWaveform::getPvMap().find(pvName);
              if (it == PvDataCharWaveform::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found (wi)." << std::endl;
                std::cout << "There are " << PvDataCharWaveform::getPvMap().size()
                            << " registered PvData(s)" << std::endl;
                status = -1;
              } else {
                precord->udf = FALSE;
                precord->dpvt = it->second;
                status = 0;
                std::vector<PvDataCharWaveform *> *charWf = it->second;
                // Init scan list for the waveform
                if (charWf->size() > 0) {
                     charWf->at(0)->initScanList();
                	charWf->at(0)->setRecord(precord);
                	status = 0;
                } else {
                	status = -1;
                }
              }
              break;
            default:
              precord->val = 0;
        }
    } else {
        std::cout << "PvData: \"" << pvName << "\" has invalid ftvl field. ("
                << precord->ftvl << ") waveformin" << std::endl;
	std::cout << "DBF_CHAR=" << DBF_CHAR << std::endl;
        status = -1;
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * Return the scanlist (saved in the PvDataString object) for the specified
 * record. The waveform is updated with values from the first PvChar instance.
 *
 * @param cmd (not used)
 * @param precord pointer to the record being set up
 * @param pvt_ps returning parameter, containing the pointer to the scanlist
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
static long devWiFfConfig_get_ioint_info(int cmd, struct dbCommon *precord,
        IOSCANPVT *pvt_ps) {
  // Need to cast to a waveformRecord to find the type of element in it...
  waveformRecord *pwaveform = reinterpret_cast<waveformRecord *>(precord);

    if (pwaveform->ftvl == DBF_STRING) {
      std::vector<PvDataStringWaveform *> *vector =
	reinterpret_cast<std::vector<PvDataStringWaveform *> *> (precord->dpvt);
      
      if (vector != NULL) {
	if (vector->size() > 0) {
	  PvDataStringWaveform *waveform = vector->at(0);
	  IOSCANPVT scanlist = waveform->getScanList();
	  *pvt_ps = scanlist;
	  std::cout << ">>> SCANLIST FOR STRING <<<" << std::endl;
	}
      }
    }
    else if (pwaveform->ftvl == 1) { //DBF_CHAR) {
      std::vector<PvDataCharWaveform *> *vector =
	reinterpret_cast<std::vector<PvDataCharWaveform *> *> (precord->dpvt);
      
      if (vector != NULL) {
	if (vector->size() > 0) {
	  PvDataCharWaveform *waveform = vector->at(0);
	  IOSCANPVT scanlist = waveform->getScanList();
	  *pvt_ps = scanlist;
	}
      }
    }
    else {
      return -1;
    }


    return 0;
}

/**
 * Copy the strings from the vector in the PvData object (pointed by dpvt)
 * to the bptr area.
 *
 * The bptr field is a pointer to an array of characters, representing nelm
 * strings of MAX_STRING_SIZE.
 * 
 * TODO: MUTEXES
 *
 * @author L.Piccoli
 */
static long devWiFfConfig_read_waveform(waveformRecord *precord) {
    if (precord->dpvt == NULL || precord->bptr == NULL) {
        return 0;
    }

    precord->nord = 0;

    if (precord->ftvl == DBF_STRING) {
        std::vector<PvDataStringWaveform *> *vector =
                reinterpret_cast<std::vector<PvDataStringWaveform *> *> (precord->dpvt);

        try {
            // Update the PV using values from the first waveform
            PvDataStringWaveform *waveform = vector->at(0);
	    //	    waveform->lock();
            std::vector<std::string>::iterator it;
            unsigned int i = 0;
            char *str = reinterpret_cast<char *> (precord->bptr);
            for (it = waveform->getValueAddress()->begin();
                    it != waveform->getValueAddress()->end(); ++it, ++i) {
                if (i < precord->nelm) {
                	memset(str, ' ', MAX_STRING_SIZE);
                	it->copy(str, MAX_STRING_SIZE);
                	precord->nord++;
                	str += MAX_STRING_SIZE;
                }
            }
	    //	    waveform->unlock();
        } catch (std::out_of_range& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return -1;
        }
    } else if (precord->ftvl == 1) {//DBF_CHAR) {
        std::vector<PvDataCharWaveform *> *vector =
                reinterpret_cast<std::vector<PvDataCharWaveform *> *> (precord->dpvt);
        try {
            // Update the PV using values from the first waveform
            PvDataCharWaveform *waveform = vector->at(0);
	    //	    waveform->lock();
            char *str = reinterpret_cast<char *> (precord->bptr);
            memset(str, ' ', precord->nelm);
            // The char waveform is a std::string
            std::string *charWf = waveform->getValueAddress();
            charWf->copy(str, precord->nelm);
            precord->nord = precord->nelm;
	    //	    waveform->unlock();
        } catch (std::out_of_range& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return -1;
        }
    } else {
        std::cerr << "ERROR: Invalid waveform type of "
                << precord->ftvl << std::endl;
        return -1;
    }
    return 0;
}

/** Longin Device Support *****************************************************/

/**
 * Struct defining functions for longin FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_longin;
    DEVSUPFUN special_linconv;
} devLonginFfConfigFunctions;

static long devLonginFfConfig_init_record(longinRecord *precord);
static long devLonginFfConfig_get_ioint_info(int cmd, struct dbCommon *precord, IOSCANPVT *pvt_ps);
static long devLonginFfConfig_read_longin(longinRecord *precord);

/**
 * Instantiation of data structure holding functions for handling longin
 * FfConfig support.
 */
devLonginFfConfigFunctions devLonginFfConfig = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devLonginFfConfig_init_record,
    (DEVSUPFUN) devLonginFfConfig_get_ioint_info,
    (DEVSUPFUN) devLonginFfConfig_read_longin,
    NULL
};

epicsExportAddress(dset, devLonginFfConfig);

/**
 * Initialize the Long Input (Longin) record. The precord->dpvt pointer
 * is set to the memory location of a long attribute the controlled PvDataLong.
 * The correct PvDataLong instance is found based on the INST_IO string
 * defined for the PV in the database. The string (following the @ sign) must
 * be the same string used to initialize the PvDataLong (e.g.
 * "TR01 MUSEDCNT" -> for reading the number of measurement devices in
 * use by the TR01 loop).
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devLonginFfConfig_init_record(longinRecord *precord) {
	long status = -1;
	std::string pvName;
	PvMapLong::iterator it;

	switch (precord->inp.type) {
		case INST_IO:
		  pvName = precord->inp.value.instio.string;
		  it = PvDataLong::getPvMap().find(pvName);
		  if (it == PvDataLong::getPvMap().end()) {
			std::cout << "PvData: \"" << pvName;
			std::cout << "\" not found." << std::endl; status = -1;
		  } else {
			precord->udf = FALSE;
			precord->dpvt = it->second;
			status = 0;
		  }
		  break;

		default:
			precord->val = 0;
	}

	if (status != 0) 	{
		precord->udf = TRUE;
		precord->dpvt = NULL;
		return FF_S_db_badField;
	}
	return 2;
}

/**
 * Return the scanlist (saved in the LoopConfiguration) for the specified
 * record. The LongIn is used only as error counters for loops threads, so update
 * with loop _scanlist with all loop pvs at end of calculation.
 *
 * @param cmd (not used)
 * @param precord pointer to the record being set up
 * @param pvt_ps returning parameter, containing the pointer to the scanlist
 * @return 0 on success, -1 on failure
 * @author L.Piccoli
 */
static long devLonginFfConfig_get_ioint_info(int cmd, struct dbCommon *precord,
        IOSCANPVT *pvt_ps) {
    if (precord->dpvt == NULL) {
        return 0;
    }
    std::vector<PvDataLong *> *vector = reinterpret_cast<std::vector<PvDataLong *> *> (precord->dpvt);
    PvDataLong *pvData = NULL;
    try {
    	pvData = vector->at(0);
    } catch (std::out_of_range& e) {
    	std::cerr << "ERROR: " << e.what() << std::endl;
    	*pvt_ps = NULL;
    	return -1;
    }

    if (pvData == NULL) {
    	*pvt_ps = NULL;
    	return -1;
    }

    // The PV name used for all PvData instances is "$(LOOP) $(ATTRIBUTE NAME)
    // So, we can retrieve the loop slot name from the name of the PvData
    // The loop slot name starts on position 0 of the string and has size of 4
    // by convention
    std::string slotName = pvData->getPvName().substr(0, 4);
    std::cout << "INFO: Longin device support -> found loop slot name: " << slotName << std::endl;
    LoopConfiguration *loopConfig = ExecConfiguration::getInstance().getLoopConfiguration(slotName);
    if (loopConfig == NULL) {
    	std::cerr << "ERROR: Failed to setup callback for Longin PV " << precord->name << std::endl;
    	*pvt_ps = NULL;
    	return -1; }
    *pvt_ps = loopConfig->_scanlist;
    return 0;
}

/**
 * This function is called by the LoopThread at the end of a feedback cycle.
 * It is indirectly invoked by calling the function ''scanIoRequest'', which
 * signals the ChannelAccess threads to call this function to update the record.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devLonginFfConfig_read_longin(longinRecord *precord) {

	if (!precord->dpvt) {
		return 0;
	}
	try {
		std::vector<PvDataLong *> *vector = reinterpret_cast<std::vector<PvDataLong *> *> (precord->dpvt);
		vector->at(0)->read(reinterpret_cast<long int *> (&precord->val));
	} catch (std::out_of_range& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return -1;
	}
    return 0;
}

/** Longout Device Support ****************************************************/

/**
 * Struct defining functions for bo FfConfig support
 */
typedef struct {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write_longout;
    DEVSUPFUN special_linconv;
} devLongoutFfConfigFunctions;

static long devLongoutFfConfig_init_record(longoutRecord *precord);
static long devLongoutFfConfig_write_longout(longoutRecord *precord);

/**
 * Instantiation of data structure holding functions for handling bo FfConfig
 * support.
 */
devLongoutFfConfigFunctions devLongoutFfConfig = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN) devLongoutFfConfig_init_record,
    NULL,
    (DEVSUPFUN) devLongoutFfConfig_write_longout,
    NULL
};

epicsExportAddress(dset, devLongoutFfConfig);

/**
 * Initialize the Long Output (Longout) record. The precord->dpvt pointer
 * is set to the memory location of the controlled PvDataLong. The correct
 * PvDataLong is found based on the INST_IO string defined for the PV in the
 * database. The string (following the @ sign) must be the same string used
 * to initialize the PvDataLong (e.g. "TR01 TOTALPOI" -> for setting the
 * number of patterns used by the TR01 loop).
 *
 * @param precord pointer to the record being initialized
 * @author L.Piccoli
 */
static long devLongoutFfConfig_init_record(longoutRecord *precord) {
    long status = -1;

    std::string pvName;
    PvMapLong::iterator it;
    switch (precord->out.type) {
        case INST_IO:
            pvName = precord->out.value.instio.string;
            it = PvDataLong::getPvMap().find(pvName);
            if (it == PvDataLong::getPvMap().end()) {
                std::cout << "PvData: \"" << pvName;
                std::cout << "\" not found (longout)." << std::endl;
                std::cout << "There are " << PvDataLong::getPvMap().size()
                        << " PvDataLong registered PvData(s)" << std::endl;
              status = -1;
            } else {
                precord->udf = FALSE;
                precord->dpvt = it->second;
                status = 0;
            }
            break;
        default:
            precord->val = 0;
    }

    if (status != 0) {
        precord->udf = TRUE;
        precord->dpvt = NULL;
        return FF_S_db_badField;
    }

    return 2;
}

/**
 * This function is invoked when a new value is written to the PV. This new
 * value is copied over to the address location defined in the init_record
 * function.
 *
 * @param precord pointer to the record being processed
 * @author L.Piccoli
 */
static long devLongoutFfConfig_write_longout(longoutRecord *precord) {
    if (!precord->dpvt) {
        return 0;
    }

    try {
        std::vector<PvDataLong *> *vector =
                reinterpret_cast<std::vector<PvDataLong *> *> (precord->dpvt);
        for (int i = 0; i < (int) vector->size(); ++i) {
            vector->at(i)->write(reinterpret_cast<long *> (&precord->val));
        }
    } catch (std::out_of_range& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}


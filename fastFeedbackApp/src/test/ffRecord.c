#include <stdio.h>
#include <sys/types.h>
#include <recGbl.h>
#include <devSup.h>
#include <devLib.h>
#include <aiRecord.h>
#include <aoRecord.h>
#include <epicsExport.h>
/*#include <dbAccessDefs.h>*/
#include <dbCommon.h>
#include <dbFldTypes.h>
#include <dbScan.h>

static long devAoPowerSet_init_record(aoRecord *precord);
static long devAoPowerSet_write_ai(aoRecord *precord);

static double power_value = 0;
static aiRecord *power_read_record = 0;

static struct {
  long number; /* This is the number of pointers! */
  DEVSUPFUN report;
  DEVSUPFUN init;
  DEVSUPFUN init_record;
  DEVSUPFUN get_ioint_info; /* get I/O interrupt info */
  /* The rest of these functions are record dependent */
  DEVSUPFUN write_ai;
  DEVSUPFUN special_linconv;
} devAoPowerSet = {
  6,
  NULL,
  NULL,
  devAoPowerSet_init_record,
  NULL,
  devAoPowerSet_write_ai,
  NULL
};

epicsExportAddress(dset,devAoPowerSet);

long devAoPowerSet_init_record(aoRecord *precord){
  precord->udf = FALSE;
  precord->dpvt = (void *) &power_value;

  return 2;
}

long devAoPowerSet_write_ai(aoRecord *precord) {
  *(double *) precord->dpvt = (double) precord->val;
  scanOnce(power_read_record);
  return 2;
}

static long devAiPowerSet_init_record(aiRecord *precord);
static long devAiPowerSet_read_ai(aiRecord *precord);

static struct {
  long number; /* This is the number of pointers! */
  DEVSUPFUN report;
  DEVSUPFUN init;
  DEVSUPFUN init_record;
  DEVSUPFUN get_ioint_info; /* get I/O interrupt info */
  /* The rest of these functions are record dependent */
  DEVSUPFUN read_ai;
  DEVSUPFUN special_linconv;
} devAiPowerSet = {
  6,
  NULL,
  NULL,
  devAiPowerSet_init_record,
  NULL,
  devAiPowerSet_read_ai,
  NULL
};

epicsExportAddress(dset,devAiPowerSet);

long devAiPowerSet_init_record(aiRecord *precord){
  precord->dpvt = (void *) &power_value;

  power_read_record = precord;

  return 2;
}

long devAiPowerSet_read_ai(aiRecord *precord) {
  precord->val = ((double *) precord->dpvt)[0];
    
  return 2; /* no conversion */
}


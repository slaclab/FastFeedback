// AutoActRef.c
// November 2018
// Jeremy Mock
// Routine to automatically save fast feedback actuator refs when
// feedback loop has converged.

#include <stdbool.h>

#include <aSubRecord.h>
#include <dbCommon.h>
#include <dbDefs.h>
#include <dbEvent.h>
#include <epicsExport.h>
#include <recSup.h>
#include <registryFunction.h>

const int N_ACT_REFS         = 10;
const int N_POI              = 4;
const int N_TR_OFFSET_ACTS   = 0;
const int N_LONG_OFFSET_ACTS = 6;
const int N_LTU_OFFSET_ACTS  = 4;

/**
 * Exported Debug Variables
 *
 * If you want to use these issue the following
 * command in the IOC Shell:
 *
 *     `var <NAME> 1`
 *
 * And be sure to set to 0 again when you are done.
 */

// Flag for switching to alternative logic.
int AUTOACT_DEBUG = FALSE;
epicsExportAddress(int, AUTOACT_DEBUG);

// Simulate a converged status for testing.
int CONVERGED = FALSE;
epicsExportAddress(int, CONVERGED);

/**
 * End Exported Debug Variables
 */

/**
 * (rreno) As of today, 10/29/2020, the most actuators used by any one loop
 * is 6. aSub records support a maximum of 21 inputs but we would need 25
 * in order to have one input per POI per actuator + 1 for converged status.
 *
 * To work around this, we create a subroutine to collect the 4 POIs for
 * each actuator into an array. Then instead of needing 41 inputs for all 10
 * acts + 1 for converged, we only need 10 inputs of 4 elements each + 1 input
 * for converged status.
 */

int CollectActPOIs(aSubRecord *precord)
{
   ( (double *)precord->vala )[0] = *(double *)precord->a;
   ( (double *)precord->vala )[1] = *(double *)precord->b;
   ( (double *)precord->vala )[2] = *(double *)precord->c;
   ( (double *)precord->vala )[3] = *(double *)precord->d;

    return 0;
}

/**
 * Determine whether or not a loop is converged. To use, pass the pointer to
 * the input element where a value of zero is not converged and nonzero is 
 * converged.
 */
static bool Converged(const void * const input)
{
    if (AUTOACT_DEBUG)
        return CONVERGED;

    return *( (int *)input ) != 0;
}

/**
 * Get the correct INPut field for a given actuator number.
 * Returns a NULL pointer if you didn't pass a number from 0-9
 */
static double * GetInputActuator(const aSubRecord *precord, int actNum)
{
    switch(actNum) {
        case 0:
            return (double *)precord->b;
        case 1:
            return (double *)precord->c;
        case 2:
            return (double *)precord->d;
        case 3:
            return (double *)precord->e;
        case 4:
            return (double *)precord->f;
        case 5:
            return (double *)precord->g;
        case 6:
            return (double *)precord->h;
        case 7:
            return (double *)precord->i;
        case 8:
            return (double *)precord->j;
        case 9:
            return (double *)precord->k;
        default:
            return NULL;
    }
}

/**
 * Generic routine to autosave actuator refs or offsets to an array
 *
 */
static void AutoSave(aSubRecord *precord, int numActs, int numPOI)
{
    double *output = (double *)precord->vala;

    for (int i = 0; i < numActs; ++i) {
        double *input = GetInputActuator(precord, i);
        if (NULL == input)
            continue;

        for (int j = 0; j < numPOI; ++j) {
            output[numPOI * i + j] = input[j];
        }
    }
}

/**
 * All loop actuator reference autosave routine.
 */
int AutoActSave(aSubRecord *precord)
{
    if ( !Converged(precord->a) )
        return 0;

    AutoSave( precord, N_ACT_REFS, N_POI );

	return 0;
}

/**
 * Longitudinal loop actuator offset autosave routine.
 */
int AutoActOffsetLongitudinal(aSubRecord *precord)
{
    if ( !Converged(precord->a) )
        return 0;

    AutoSave( precord, N_LONG_OFFSET_ACTS, N_POI );

	return 0;
}

/**
 * Fast LTU actuator offset autosave routine.
 */
int AutoActOffsetLTU(aSubRecord *precord)
{
    if ( !Converged(precord->a) )
        return 0;

    AutoSave( precord, N_LTU_OFFSET_ACTS, N_POI );

	return 0;
}

/**
 * Transverse loop actuator offset autosave routine for those loops 
 * not covered above.
 * As of 10/29/2020 there are no offsets to autosave.
 */
int AutoActOffset(aSubRecord *precord)
{
    for (int i = 0; i < 40; ++i)
        ( (double *)precord->vala )[i] = 0.0;

    return 0;
}


epicsRegisterFunction(CollectActPOIs);
epicsRegisterFunction(AutoActSave);
epicsRegisterFunction(AutoActOffsetLongitudinal);
epicsRegisterFunction(AutoActOffsetLTU);
epicsRegisterFunction(AutoActOffset);

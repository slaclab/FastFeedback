// AutoActRef.c
// November 2018
// Jeremy Mock
// Routine to automatically save fast feedback actuator refs when
// feedback loop has converged.

#include <stdio.h>
#include <dbEvent.h>
#include <dbDefs.h>
#include <dbCommon.h>
#include <recSup.h>
#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

int AutoActSave(aSubRecord *precord)
{	
  int converged;
  double A1, A2, A3, A4, A5, A6, A7, A8, A9, A10;
  double* output = (double*)precord->vala;

	converged=*(int *)precord->a;
	A1 = *(double*)precord->b;
	A2 = *(double*)precord->c;
	A3 = *(double*)precord->d;
	A4 = *(double*)precord->e;
	A5 = *(double*)precord->f;
	A6 = *(double*)precord->g;
	A7 = *(double*)precord->h;
	A8 = *(double*)precord->i;
	A9 = *(double*)precord->j;
	A10 = *(double*)precord->k;

  if(converged) {
    output[0] = A1;
    output[1] = A2;
    output[2] = A3;
    output[3] = A4;
    output[4] = A5;
    output[5] = A6;
    output[6] = A7;
    output[7] = A8;
    output[8] = A9;
    output[9] = A10;
  }

	return 0;
}

int AutoActOff(aSubRecord *precord)
{
  double* output = (double*) precord->vala;
  int i;
  for (i = 0; i < 40; i++) {
    output[i] = 0;
  }
  return 0;
}

int AutoActOffLongSave(aSubRecord *precord)
{	
  int converged, i;
  double A1_1,A1_2,A1_3, A2_1,A2_2,A2_3, A3_1,A3_2,A3_3, A4_1,A4_2,A4_3, A5_1,A5_2,A5_3, A6_1,A6_2,A6_3;
  double* output = (double*)precord->vala;

	converged=*(int *)precord->a;
	A1_1 = *(double*)precord->b;
	A1_2 = *(double*)precord->c;
	A1_3 = *(double*)precord->d;
	A2_1 = *(double*)precord->e;
	A2_2 = *(double*)precord->f;
	A2_3 = *(double*)precord->g;
	A3_1 = *(double*)precord->h;
	A3_2 = *(double*)precord->i;
	A3_3 = *(double*)precord->j;
	A4_1 = *(double*)precord->k;
	A4_2 = *(double*)precord->l;
	A4_3 = *(double*)precord->m;
	A5_1 = *(double*)precord->n;
	A5_2 = *(double*)precord->o;
	A5_3 = *(double*)precord->p;
	A6_1 = *(double*)precord->q;
	A6_2 = *(double*)precord->r;
	A6_3 = *(double*)precord->s;


  if(converged) {
    output[0] = 0;
    output[1] = A1_1;
    output[2] = A1_2;
    output[3] = A1_3;
    output[4] = 0;
    output[5] = A2_1;
    output[6] = A2_2;
    output[7] = A2_3;
    output[8] = 0;
    output[9] = A3_1;
    output[10] = A3_2;
    output[11] = A3_3;
    output[12] = 0;
    output[13] = A4_1;
    output[14] = A4_2;
    output[15] = A4_3;
    output[16] = 0;
    output[17] = A5_1;
    output[18] = A5_2;
    output[19] = A5_3;
    output[20] = 0;
    output[21] = A6_1;
    output[22] = A6_2;
    output[23] = A6_3;
    for (i = 24; i < 40; i++){
      output[i] = 0.;
    }
  }

	return 0;
}

int AutoActOffSave(aSubRecord *precord)
{	
  int converged, i;
  double A1_0,A1_1,A1_2,A1_3, A2_0,A2_1,A2_2,A2_3, A3_0,A3_1,A3_2,A3_3, A4_0,A4_1,A4_2,A4_3;
  double* output = (double*)precord->vala;

	converged=*(int *)precord->a;
	A1_0 = *(double*)precord->b;
	A1_1 = *(double*)precord->c;
	A1_2 = *(double*)precord->d;
	A1_3 = *(double*)precord->e;
	A2_0 = *(double*)precord->f;
	A2_1 = *(double*)precord->g;
	A2_2 = *(double*)precord->h;
	A2_3 = *(double*)precord->i;
	A3_0 = *(double*)precord->j;
	A3_1 = *(double*)precord->k;
	A3_2 = *(double*)precord->l;
	A3_3 = *(double*)precord->m;
	A4_0 = *(double*)precord->n;
	A4_1 = *(double*)precord->o;
	A4_2 = *(double*)precord->p;
	A4_3 = *(double*)precord->q;




  if(converged) {
    output[0] = A1_0;
    output[1] = A1_1;
    output[2] = A1_2;
    output[3] = A1_3;
    output[4] = A2_0;
    output[5] = A2_1;
    output[6] = A2_2;
    output[7] = A2_3;
    output[8] = A3_0;
    output[9] = A3_1;
    output[10] = A3_2;
    output[11] = A3_3;
    output[12] = A4_0;
    output[13] = A4_1;
    output[14] = A4_2;
    output[15] = A4_3;
    for (i = 16; i < 40; i++){
      output[i] = 0.;
    }
  }

	return 0;
}


epicsRegisterFunction(AutoActSave);
epicsRegisterFunction(AutoActOffLongSave);
epicsRegisterFunction(AutoActOffSave);
epicsRegisterFunction(AutoActOff);

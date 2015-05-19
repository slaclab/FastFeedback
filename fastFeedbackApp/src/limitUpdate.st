/*
record(calcout, "IOC:SYS0:$(AREA):LEM_DL2ENLOLO") {
  field(PINI, "YES")
  field(INPA, "IOC:SYS0:$(AREA):L3AMP_NOFS")
  field(INPB, "IOC:SYS0:$(AREA):S29_ASUM")
  field(INPC, "IOC:SYS0:$(AREA):S30_ASUM")
  field(DTYP, "FfConfig")
  field(OUT, "@LEM_DL2ENLOLO")
  field(EGU, "MeV")
  field(PREC, "4")
  field(DESC, "DL2 Low limit from LEM")
  field(CALC, "A-(B+C)-300")
}

record(calcout, "IOC:SYS0:$(AREA):LEM_DL2ENHIHI") {
  field(PINI, "YES")
  field(INPA, "IOC:SYS0:$(AREA):L3AMP_NOFS")
  field(INPB, "IOC:SYS0:$(AREA):S29_ASUM")
  field(INPC, "IOC:SYS0:$(AREA):S30_ASUM")
  field(EGU, "MeV")
  field(DTYP, "FfConfig")
  field(OUT, "@LEM_DL2ENHIHI")
  field(PREC, "4")
  field(DESC, "DL2 High limit from LEM")
  field(CALC, "A+(B+C)+1000")
}
*/
program limitUpdate

double l3AmpNofs;
/*assign l3AmpNofs to "IOC:SYS0:{LOOP}:L3AMP_NOFS";*/
assign l3AmpNofs to "ACCL:LI25:1:A_NOFS_";
monitor l3AmpNofs;

double s29Asum;
/*assign s29Asum to "IOC:SYS0:{LOOP}:S29_ASUM";*/
assign s29Asum to "ACCL:LI29:0:A_SUM_";
monitor s29Asum;

double s30Asum;
/*assign s30Asum to "IOC:SYS0:{LOOP}:S30_ASUM";*/
assign s30Asum to "ACCL:LI30:0:A_SUM_";
monitor s30Asum;

double loloLimit;
assign loloLimit to "FBCK:{IOC}:{LOOP}:A6LOLO";

double hihiLimit;
assign hihiLimit to "FBCK:{IOC}:{LOOP}:A6HIHI";

ss limitUpdate {
  state INIT {
    when() {
      printf("*** INIT limitUpdate ***\n");
      pvGet(l3AmpNofs, ASYNC);
      pvGet(s29Asum, ASYNC);
      pvGet(s30Asum, ASYNC);
      /*      printf("Starting limitUpdate sequencer\n");*/
    } state LIMIT_UPDATE
  }

  state LIMIT_UPDATE {
    entry {
      pvGetComplete(l3AmpNofs);
      pvGetComplete(s29Asum);
      pvGetComplete(s30Asum);
    }

    when(delay(1.)) {
      /* LOLO = A-(B+C)-300 */
      /* HIHI = A+(B+C)+500 */

      loloLimit = l3AmpNofs - (s29Asum + s30Asum) - 500;
      hihiLimit = l3AmpNofs + (s29Asum + s30Asum) + 1000;

/* These limits are for testing on DEV only */
/*       loloLimit = l3AmpNofs - (s29Asum + s30Asum) - 50000; */
/*       hihiLimit = l3AmpNofs + (s29Asum + s30Asum) + 100000; */

      pvPut(loloLimit, SYNC);
      pvPut(hihiLimit, SYNC);

      /*      printf("LIMIT_UPDATE: %f ... %f\n", loloLimit, hihiLimit);*/
    } state LIMIT_UPDATE

    exit {
      pvGet(l3AmpNofs, SYNC);
      pvGet(s29Asum, SYNC);
      pvGet(s30Asum, SYNC);
    }
  }
}

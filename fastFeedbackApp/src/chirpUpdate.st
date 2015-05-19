program chirpUpdate

short chirpState;
assign chirpState to "FBCK:{IOC}:{LOOP}:CHIRPSTATE";
monitor chirpState;

double chirpValue;
assign chirpValue to "FBCK:LI22:1:CHIRP";

double chirpDesired;
assign chirpDesired to "FBCK:{IOC}:{LOOP}:CHIRPDES";

ss chirpUpdate {
  state INIT {
    when() {
      printf("*** INIT chirpUpdate ***\n");
      pvGet(chirpValue, SYNC);
    } state CHIRP_UPDATE
  }

  state CHIRP_UPDATE {
    entry {
      pvGet(chirpValue, SYNC);
      pvGet(chirpState, SYNC);
    }

    when(delay(1.)) {
      if (chirpState < 1) {
	pvGet(chirpValue, SYNC);
	chirpDesired = chirpValue;
	pvPut(chirpDesired, SYNC);
	/*	printf("CHIRP_UPDATE: %f\n", chirpDesired);*/
      }
    } state CHIRP_UPDATE

    exit {
      pvGet(chirpState, SYNC);
      pvGet(chirpValue, SYNC);
    }
  }
}

program chirpControl

short chirpState;
assign chirpState to "FBCK:{IOC}:{LOOP}:CHIRPSTATE";
monitor chirpState;

double chirpValue;
assign chirpValue to "FBCK:LI22:1:CHIRP";
/* This one is for testing on DEV only */
/* assign chirpValue to "FBCK:{IOC}:TR01:M1"; */

double chirpDesired;
assign chirpDesired to "FBCK:{IOC}:{LOOP}:CHIRPDES";

short chirpReady;
assign chirpReady to "FBCK:{IOC}:{LOOP}:CHIRPREADY";

ss chirpCheck {
  state INIT {
    when() {
      printf("*** INIT chirpCheck ***\n");
      pvGet(chirpValue, SYNC);
      printf("*** INITAL STATE *** %f ***\n", chirpValue);
    } state CHIRP_OFF
  }

  state CHIRP_OFF {
    /* Turning CHIRP ON */
    when(chirpState > 0) {
      printf("CHIRP went ON - getting current CHIRP\n");
      pvGet(chirpValue, ASYNC);
      chirpReady = 0;
      pvPut(chirpReady, ASYNC);
    } state WAIT_CHIRP
  }

  state WAIT_CHIRP {
    when (pvGetComplete(chirpValue) && pvPutComplete(chirpReady)) {
      printf("CHIRP went ON - setting CHIRPDES\n");
      chirpDesired = chirpValue;
      pvPut(chirpDesired, SYNC);
      chirpReady = 1;
      pvPut(chirpReady, ASYNC);
    } state WAIT_CHIRP_READY
  }

  state WAIT_CHIRP_READY {
    when(pvPutComplete(chirpReady)) {
      printf("CHIRPREADY has been set to YES\n");
    } state CHIRP_ON
  }

  state CHIRP_ON {
    /* Turning CHIRP OFF */
    when(chirpState <= 0) {
      printf("CHIRP went OFF\n");
    } state WAIT_CHIRP_NOT_READY
  }

  state WAIT_CHIRP_NOT_READY {
    when(pvPutComplete(chirpReady)) {
      chirpReady = 0;
      pvPut(chirpReady, ASYNC);
      printf("CHIRPREADY has been set to NO\n");
    } state CHIRP_OFF
  }
}

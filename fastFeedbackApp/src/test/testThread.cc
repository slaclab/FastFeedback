#include <iostream>
#include "testThread.h"

testThread::testThread(int arg, const char *name) :
  thread(*this, name,
	 epicsThreadGetStackSize(epicsThreadStackSmall), 50),
  counter(0) {

  thread.start();
}

testThread::~testThread() {
}

void testThread::run() {
  long status = dbNameToAddr("FBCK:FB02:TR01:POINUM", &pulseid);
  if (status != 0) {
    std::cout << "ERROR: dbNameToAddr(\"FBCK:FB02:TR01:POINUM\", ...) "
	      << "failed (status=" << status << ")" << std::endl;
  }
  else {
    while(1) {
      epicsThreadSleep(2);
      updatePV();
    }
  }
}

void testThread::updatePV() {
  ++counter;
  double counter_d = counter;
  long status = dbPutField(&pulseid, DBR_DOUBLE, &counter_d, 1);
  if (status != 0) {
    std::cout << "ERROR: dbPut() failed (status=" << status << ")" << std::endl;
  }
}


extern "C" {

void startTestThread() {
  printf("Starting MyThread...");
  testThread *myTestThread;
  myTestThread = new testThread(0, "MyThread");
  printf(" done.\n");
}

}

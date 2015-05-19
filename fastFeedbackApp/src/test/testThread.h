#ifndef __TESTTHREAD_H__
#define __TESTTHREAD_H__

#include<epicsThread.h>
#include <dbAccess.h>

class testThread : public epicsThreadRunable {
 public:
  testThread(int arg, const char *name);
  virtual ~testThread();

  virtual void run();
  epicsThread thread;

 private:
  int counter;
  DBADDR pulseid;

  void updatePV();
};

#endif

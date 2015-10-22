/* Author:  Marty Kraimer Date:    17MAR2000 */

//#include <stddef.h>
//#include <stdlib.h>
//#include <stddef.h>
//#include <string.h>
//#include <stdio.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"
#include "Commands.h"
//#include "Defs.h"USING_FF_NAMESPACE

/*
static const iocshArg epicsThreadSleepArg0 = {"seconds", iocshArgDouble};
static const iocshArg * const epicsThreadSleepArgs[1] = {&epicsThreadSleepArg0};
static const iocshFuncDef epicsThreadSleepFuncDef ={"mySleep", 1, epicsThreadSleepArgs};

static void epicsThreadSleepCallFunc(const iocshArgBuf *args) {
    epicsThreadSleep(args[0].dval);
    std::cout << "done." <<std::endl;
}

static const iocshArg ffStart_Arg0 = {"file", iocshArgString};
static const iocshArg * const ffStart_Args[1] = {&ffStart_Arg0};
static const iocshFuncDef ffStart_FuncDef ={"ffStart", 1, ffStart_Args};

static void ffStart(const iocshArgBuf *args) {
    //ExecThread::getInstance();
    std::cout << "Starting FastFeedback" << std::endl;
    std::cout << "-> config file: " << args[0].sval << std::endl;
}

FF_NAMESPACE_START

class IocshRegister {
public:
    IocshRegister() {
        iocshRegister(&epicsThreadSleepFuncDef, epicsThreadSleepCallFunc);
        iocshRegister(&ffStart_FuncDef, ffStart);
    }
};

FF_NAMESPACE_END


#include "test/testThread.h"
#include <iocsh.h>
#include <iostream>
#include <fstream>

//#include "framework/ExecThread.h"

#include "Commands.h"
*/
        
static FastFeedback::IocshRegister iocshRegisterObj;

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
    //testThread tt(0,"TEST"); // This works only on softIOC

    iocsh(NULL); // Puts iocsh in interactive mode
    epicsExit(0);
    return (0);
}

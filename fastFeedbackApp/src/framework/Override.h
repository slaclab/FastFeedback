#include <string>
#include <iostream>
#include "Defs.h"
#include "PvData.h"

FF_NAMESPACE_START

class Override {
    public:
        static Override &getInstance();
        
        void setOverrideState(bool overrideState);
        bool getOverrideState();

        void setBcastStates(bool bcastVal);
        void getBcastStates();

        void setStates();
        void getStates();
        
    protected:

//        TestRec(std::string slotName="TR01", std::string name="T1") :
//            _slotName(slotName),
//            _name(name),
//            _testPv(slotName + " " + name + "TESTREC"),
//            _test() {
//                _testPv.initScanList();
//                std::cout << "Slot Name: " + slotName << std::endl;
//                std::cout << "Name: " + name << std::endl;
//                std::cout << slotName + " " + name + "TESTREC" << std::endl;
//            }
        
        Override(bool overrideState=0) :
            _overrideState(overrideState) {
                std::cout << "Override State: " + overrideState << std::endl;
            }
        
        static Override _instance;        
        
        bool _overrideState;
        bool _bcastStates;

};
FF_NAMESPACE_END

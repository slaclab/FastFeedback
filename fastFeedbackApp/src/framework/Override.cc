#include "Override.h"


USING_FF_NAMESPACE

Override Override::_instance;

Override &Override::getInstance() {
    return _instance;
}

void Override::setOverrideState(bool overrideState) {
    _overrideState = overrideState;
}

bool Override::getOverrideState() {
    return _overrideState;
}

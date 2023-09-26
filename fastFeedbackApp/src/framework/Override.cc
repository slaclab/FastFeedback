#include "Override.h"


USING_FF_NAMESPACE

Override Override::_instance;

Override &Override::getInstance() {
    return _instance;
}


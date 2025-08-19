/* 
 * File:   Commands.h
 * Author: lpiccoli
 *
 * Created on June 15, 2010, 10:21 AM
 */

#ifndef _COMMANDS_H
#define	_COMMANDS_H

#include "Defs.h"


FF_NAMESPACE_START

class IocshRegister {
public:
    IocshRegister();
private:
    void registerHelp();
};

FF_NAMESPACE_END


#endif	/* _COMMANDS_H */


/* 
 * File:   FileChannel.h
 * Author: lpiccoli
 *
 * Created on May 28, 2010, 12:51 PM
 */

#ifndef _FILECHANNEL_H
#define	_FILECHANNEL_H

#include <string>
#include <fstream>

#include <epicsTime.h>

#include "Defs.h"
#include "CommunicationChannel.h"

FF_NAMESPACE_START

/**
 * Allow read or write access to a file.
 *
 * Measurements values are read, while actuator values are stored in the file
 *
 * Values appear on the file in ASCII format, one per line.
 *
 * TODO: Add timestamp after each value
 *
 * @author L.Piccoli
 */
class FileChannel : public CommunicationChannel {
public:
    FileChannel(CommunicationChannel::AccessType accessType,
                std::string fileName, bool wrapAround = false);
    virtual ~FileChannel();

    virtual int read(double &value, epicsTimeStamp &timestamp, double timeout = 3.0);
    virtual int write(double value);
    virtual int write(double value, epicsTimeStamp timestamp);

    std::string getFileName() {
        return _fileName;
    }
    
private:
    /** Name of the file containing measurements or to store settings */
    std::string _fileName;

    /** Used if channel is used for reading a file */
    std::ifstream _inFile;

    /** Used if channel is used for writing to a file */
    std::ofstream _outFile;

    /**
     * If file is used as input, start reading from the beginning after
     * reaching the end of file
     */
    bool _wrapAround;
};

FF_NAMESPACE_END

#endif	/* _FILECHANNEL_H */


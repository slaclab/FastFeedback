/* 
 * File:   FileChannel.cc
 * Author: lpiccoli
 * 
 * Created on May 28, 2010, 12:51 PM
 */

#include "FileChannel.h"
#include "Log.h"
#include <fstream>
#include <iostream>
#include <exception>
#include <sstream>

USING_FF_NAMESPACE

/**
 * FileChannel constructor, specifying the access type and file from which
 * the values will be read or written.
 *
 * TODO: should I really use exceptions here?!?
 *
 * @param accessType defines whether this channel is for read access
 * (CommunicationChannel::READ_ONLY) or write access
 * (CommunicationChannel::WRITE_ONLY)
 * @param fileName name of the file that contain value to be read or name
 * of the file that will have the values written.
 * @param wrapAround indicate whether the file when used as input should restart
 * from the beginning after reaching EOF.
 * @throw exception if the file for reading/writting cannot be open or created
 * @author L.Piccoli
 */
FileChannel::FileChannel(CommunicationChannel::AccessType accessType,
        std::string fileName, bool wrapAround) :
CommunicationChannel(accessType),
_fileName(fileName),
_wrapAround(wrapAround) {
	  Log::getInstance() << Log::flagConfig << Log::dpInfo
			     << "INFO: FileChannel::FileChannel("
			     << fileName.c_str() << ")"
			     << Log::dp;

    if (_accessType == CommunicationChannel::READ_ONLY) {
        _inFile.open(_fileName.c_str(), std::ifstream::in);
        if (!_inFile.is_open()) {
            std::stringstream s;
            s << "File \"" << fileName << "\" not opened (read)";
            throw Exception(s.str());
        }
        if (_inFile.fail()) {
            std::stringstream s;
            s << "Failed to open file \"" << fileName << "\" for reading";
            throw Exception(s.str());
        }
    } else {
        _outFile.open(_fileName.c_str(), std::ifstream::out);
        if (_outFile.fail()) {
            std::stringstream s;
            s << "Failed to open file \"" << fileName << "\" for writing";
            throw Exception(s.str());
        }
    }
}

/**
 * FileChannel destructor, closes the opened input/output files.\
 *
 * @author L.Piccoli
 */
FileChannel::~FileChannel() {
    if (_accessType == CommunicationChannel::READ_ONLY) {
        _inFile.close();
    } else {
        _outFile.close();
    }
}

/**
 * Read the next value from the input file.
 *
 * @param value its an output parameter that contains the read value. In
 * case of error (e.g. EOF or format error or FileChannel was created
 * as WRITE_ONLY) then the value is ZERO - and should *not* be used.
 * @return 0 in case of success reading the next value from the file, -1 if
 * there is an error.
 * @author L.Piccoli
 */
int FileChannel::read(double &value, epicsTimeStamp &timestamp, double timeout) {
    value = -1;
    try {
        _inFile >> value;
    } catch (...) {
        std::cerr << "ERROR: (FileChannel) Format error reading input file ("
                << _fileName << ")" << std::endl;
        return -3;
    }

    if (_inFile.eof() && _wrapAround) {
        _inFile.clear();
        _inFile.seekg(0, std::ios::beg);
        _inFile >> value;
    }

    if (_inFile.eof()) {
        value = 0;
        _readErrorCount++;
        std::cerr << "ERROR: (FileChannel) EOF reading input file." << std::endl;
        return -1;
    }

    if (_inFile.fail()) {
        value = -2;
        _readErrorCount++;
        std::cerr << "ERROR: (FileChannel) Failure reading input file." << std::endl;
        return -1;
    }

    timestamp.secPastEpoch = 0;
    timestamp.nsec = 0;

    _readCount++;
    return 0;
}

/**
 * Write the specified value to the output file.
 *
 * @param value new value to be added to the output file.
 * @return 0 if value was written successfully, -1 if there is an error
 * @author L.Piccoli
 */
int FileChannel::write(double value) {
    _outFile << value << std::endl;
    if (_outFile.fail()) {
        _writeErrorCount++;
        return -1;
    }
    _writeCount++;
    return 0;
}

int FileChannel::write(double value, epicsTimeStamp timestamp) {
    return write(value);
}

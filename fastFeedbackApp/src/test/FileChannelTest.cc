#include "FileChannelTest.h"
#include "FileChannel.h"
#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::FileChannelTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

#define TMP_TEST_DIR "/tmp/"
#ifdef RTEMS
#define TEST_DIR "/boot/g/lcls/vol8/epics/iocTop/FFController/Development/test/"
#else
#define TEST_DIR "./test/"
#endif
#define TEST_FILE_IN TEST_DIR"TestDevice.in"
#define TEST_FILE_OUT TEST_DIR"TestDevice.out"
#define TEST_FILE_IN_PERM TEST_DIR"TestDevice.in.nopermission"
#define TEST_FILE_OUT_PERM TEST_DIR"TestDevice.out.nopermission"
#define TMP_TEST_FILE_IN TMP_TEST_DIR"TestDevice.in"
#define TMP_TEST_FILE_OUT TMP_TEST_DIR"TestDevice.out"
#define TMP_TEST_FILE_IN_PERM TMP_TEST_DIR"TestDevice.in.nopermission"
#define TMP_TEST_FILE_OUT_PERM TMP_TEST_DIR"TestDevice.out.nopermission"

void FF::FileChannelTest::testConstructor() {
/** Do not run this test on RTEMS */
#ifndef RTEMS
    try {
        FileChannel fr(CommunicationChannel::READ_ONLY, "/tmp/non_existing_file");
    } catch (std::exception &e) {
        CPPUNIT_ASSERT_MESSAGE("Tried opening READ file that does not exist",
                true);
    }

    try {
        FileChannel fr(CommunicationChannel::READ_ONLY, TEST_FILE_IN);
    } catch (std::exception &e) {
        CPPUNIT_ASSERT_MESSAGE("Tried opening READ file that does not exist",
                false);
    }

    try {
        FileChannel fw(CommunicationChannel::WRITE_ONLY, "/tmp/non_existing_file");
    } catch (std::exception &e) {
        CPPUNIT_ASSERT_MESSAGE("Tried opening WRITE file that does not exist",
                true);
    }

    try {
        FileChannel fw(CommunicationChannel::WRITE_ONLY, TEST_FILE_OUT);
    } catch (std::exception &e) {
        CPPUNIT_ASSERT_MESSAGE("Tried opening WRITE file that does not exist",
                true);
    }

    std::ostringstream s;
    s << "chmod a-rw " << TEST_FILE_IN_PERM;
    ::system(s.str().c_str());

    try {
        FileChannel fr(CommunicationChannel::READ_ONLY, TEST_FILE_IN_PERM);
    } catch (std::exception &e) {
        CPPUNIT_ASSERT_MESSAGE("Tried opening READ file that has no read permission",
                true);
    }

    s.str(std::string());
    s.clear();
    s.seekp(0);
    s << "chmod u+rw,g+rw,o+r " << TEST_FILE_IN_PERM << "\0";
    system(s.str().c_str());

    s.str(std::string());
    s.clear();
    s.seekp(0);
    s << "chmod a-rw " << TEST_FILE_OUT_PERM << "\0";
    system(s.str().c_str());

    try {
        FileChannel fr(CommunicationChannel::WRITE_ONLY, TEST_FILE_OUT_PERM);
    } catch (std::exception &e) {
        CPPUNIT_ASSERT_MESSAGE("Tried opening WRITE file that has no read permission",
                true);
    }

    s.str(std::string());
    s.clear();
    s.seekp(0);
    s << "chmod u+rw,g+rw,o+r " << TEST_FILE_OUT_PERM << "\0";
    system(s.str().c_str());
#endif
}

void FF::FileChannelTest::testRead() {
    // Write known values to input file first
    int numMeas = 10;
    std::ofstream testFile;
    std::string testFileName;
#ifdef RTEMS
    testFileName = TMP_TEST_FILE_IN;
#else
    testFileName = TEST_FILE_IN;
#endif
    testFile.open(testFileName.c_str(), std::ifstream::trunc);

    for (int i = 0; i < numMeas; ++i) {
        testFile << i << std::endl;
    }
    testFile.close();

    FileChannel fr(CommunicationChannel::READ_ONLY, testFileName);
    double value;
    epicsTimeStamp timestamp;
    for (int i = 0; i < numMeas; ++i) {
        double expectedValue = i;
        CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }

    // Try reading one more should cause EOF
    CPPUNIT_ASSERT_EQUAL(-1, fr.read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(0.0, value);

    // Try once more!
    CPPUNIT_ASSERT_EQUAL(-1, fr.read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(0.0, value);
}

void FF::FileChannelTest::testWritingReadFile() {
    FileChannel fr(CommunicationChannel::READ_ONLY, TEST_FILE_IN);
    CPPUNIT_ASSERT_EQUAL(-1, fr.write(0.0));
}

void FF::FileChannelTest::testReadBadFile() {
    epicsTimeStamp timestamp;
    // Write known values to input file first

    std::ofstream testFile;
    std::string testFileName;
#ifdef RTEMS
    testFileName = TMP_TEST_FILE_IN;
#else
    testFileName = TEST_FILE_IN;
#endif

    try {
        testFile.open(testFileName.c_str(), std::ifstream::trunc);
        if (testFile.fail()) {
            CPPUNIT_ASSERT_MESSAGE("Failed to open input file", false);
        }
        testFile << "10e1" << std::endl;
        testFile << "-1e-10" << std::endl;
        testFile << "-.1e3" << std::endl;
        testFile << ".22" << std::endl;
        testFile << "a10" << std::endl; // This one should fails to be read in
        testFile.close();
    } catch (...) {
        CPPUNIT_ASSERT_MESSAGE("Failed to write test file", false);
    }
    
    FileChannel fr(CommunicationChannel::READ_ONLY, testFileName);
    double value;
    CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(-1, fr.read(value, timestamp));
}

void FF::FileChannelTest::testReadWrap() {
    // Write known values to input file first
    int numMeas = 10;
    std::ofstream testFile;
    std::string testFileName;
#ifdef RTEMS
    testFileName = TMP_TEST_FILE_IN;
#else
    testFileName = TEST_FILE_IN;
#endif

    testFile.open(testFileName.c_str(), std::ifstream::trunc);
    for (int i = 0; i < numMeas; ++i) {
        testFile << i << std::endl;
    }
    testFile.close();

    FileChannel fr(CommunicationChannel::READ_ONLY, testFileName, true);
    double value;
    epicsTimeStamp timestamp;
    for (int i = 0; i < numMeas; ++i) {
        double expectedValue = i;
        CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }

    // Read all over again, twice
    for (int i = 0; i < numMeas; ++i) {
        double expectedValue = i;
        CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }

    for (int i = 0; i < numMeas; ++i) {
        double expectedValue = i;
        CPPUNIT_ASSERT_EQUAL(0, fr.read(value, timestamp));
        CPPUNIT_ASSERT_EQUAL(expectedValue, value);
    }
}

void FF::FileChannelTest::testWrite() {
    int numMeas = 10;
    std::string testFileName;
#ifdef RTEMS
    testFileName = TMP_TEST_FILE_OUT;
#else
    testFileName = TEST_FILE_OUT;
#endif

    try {
        FileChannel fw(CommunicationChannel::WRITE_ONLY, testFileName);
        double value;
        for (int i = 0; i < numMeas; ++i) {
            value = i;
            CPPUNIT_ASSERT_EQUAL(0, fw.write(value));
        }

        // Check if file did get the correct values written
        // Write known values to input file first
        std::ifstream testFile;
        testFile.open(testFileName.c_str(), std::ifstream::in);
        for (int i = 0; i < numMeas; ++i) {
            double expectedValue = i;
            testFile >> value;
            CPPUNIT_ASSERT_EQUAL(expectedValue, value);
        }
        testFile.close();

    } catch (...) {
        CPPUNIT_ASSERT_MESSAGE("Failed test write", false);
    }
}

void FF::FileChannelTest::testReadingWriteFile() {
    double value = 0;
    FileChannel fw(CommunicationChannel::WRITE_ONLY, TEST_FILE_IN);
    CPPUNIT_ASSERT_EQUAL(-1, fw.write(value));
}

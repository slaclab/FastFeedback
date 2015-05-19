#include "FcomChannelTest.h"
#include <epicsTime.h>
#include <sstream>
#include <exception>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::FcomChannelTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void FcomChannelTest::setUp() {
#ifdef RTEMS
    std::string nameRead = "BPMS:LI22:301:X";
    fcomReadChannel = new FcomChannel(CommunicationChannel::READ_ONLY, nameRead, false);

    std::string nameWrite = "XCOR:LI26:202:BDES";
    fcomWriteChannel = new FcomChannel(CommunicationChannel::WRITE_ONLY, nameWrite, false);
#endif
}

void FcomChannelTest::tearDown() {
#ifdef RTEMS
    delete fcomReadChannel;
    delete fcomWriteChannel;
#endif
}

void FcomChannelTest::testInit() {
#ifdef RTEMS
    CPPUNIT_ASSERT_EQUAL(0, fcomReadChannel->initialize());
    CPPUNIT_ASSERT_EQUAL(0, fcomWriteChannel->initialize());
#endif
}

void FcomChannelTest::testRead() {
#ifdef RTEMS
    fcomReadChannel->initialize();

    sleep(1); // Wait for BPM data to arrive

    epicsTimeStamp timestamp;
    double value;

    CPPUNIT_ASSERT_EQUAL(0, fcomReadChannel->read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(0, fcomReadChannel->getReadErrorCount());
    CPPUNIT_ASSERT_EQUAL(1, fcomReadChannel->getReadCount());

    epicsTimeStamp timestamp2;
    sleep(1);

    CPPUNIT_ASSERT_EQUAL(0, fcomReadChannel->read(value, timestamp2));
    CPPUNIT_ASSERT_EQUAL(0, fcomReadChannel->getReadErrorCount());
    CPPUNIT_ASSERT_EQUAL(2, fcomReadChannel->getReadCount());
    CPPUNIT_ASSERT(timestamp.secPastEpoch < timestamp2.secPastEpoch);
#endif
}

void FcomChannelTest::testWrite() {
#ifdef RTEMS
    fcomWriteChannel->initialize();
    double value = 0.1;
    CPPUNIT_ASSERT_EQUAL(0, fcomWriteChannel->write(value));
    CPPUNIT_ASSERT_EQUAL(0, fcomWriteChannel->getWriteErrorCount());
    CPPUNIT_ASSERT_EQUAL(1, fcomWriteChannel->getWriteCount());
#endif
}

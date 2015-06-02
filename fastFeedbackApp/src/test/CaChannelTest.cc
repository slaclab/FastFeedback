#include "CaChannelTest.h"
#include "CaChannel.h"
#include <epicsTime.h>
#include <sstream>
#include <exception>

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::CaChannelTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

void CaChannelTest::setUp() {
    _channelAccess.attach();
}

void CaChannelTest::tearDown() {
    _channelAccess.detach();
}

/**
 * Test reading a double value using ChannelAccess
 */
void CaChannelTest::testRead() {
    std::string pvName = "SIOC:SYS0:ML00:AO500";

    CaChannel *caChannel;
    try {
        caChannel = new CaChannel(CommunicationChannel::READ_ONLY, pvName);
    } catch (...) {
        CPPUNIT_ASSERT_MESSAGE("Failed to create CaChannel", false);
        return;
    }

    double expectedValue = 3.1415;

    std::ostringstream cmd;
    cmd << "caput " << pvName << " " << expectedValue;
    ::system(cmd.str().c_str());

    epicsTimeStamp timestamp;
    double value;

    CPPUNIT_ASSERT_EQUAL(0, caChannel->read(value, timestamp));
    CPPUNIT_ASSERT_EQUAL(expectedValue, value);

    delete(caChannel);
}

void CaChannelTest::testWrite() {
    std::string pvName = "SIOC:SYS0:ML00:AO500";
    CaChannel *caChannel;
    try {
        caChannel = new CaChannel(CommunicationChannel::WRITE_ONLY, pvName);
    } catch (...) {
        CPPUNIT_ASSERT_MESSAGE("Failed to create CaChannel", false);
        return;
    }

    double value = 0.1;

    // Make sure ChannelAccess state is connected
    CPPUNIT_ASSERT_EQUAL(ECA_NORMAL, ca_pend_io(2.0));
    CPPUNIT_ASSERT_EQUAL(cs_conn, ca_state(caChannel->_channelId));

    CPPUNIT_ASSERT_EQUAL(0, caChannel->write(value));

    double readBack = 0;

    // read back value using "caget -t " + pvName
    std::string data;
    FILE *stream;
    char buffer[100];
    std::ostringstream cmd;
    cmd << "caget -t " << pvName;

    stream = ::popen(cmd.str().c_str(), "r");
    while (fgets(buffer, 100, stream) != NULL) {
        data.append(buffer);
    }
    ::pclose(stream);

    std::istringstream instring(data);
    instring >> readBack;


    CPPUNIT_ASSERT_EQUAL(value, readBack);

    delete(caChannel);
}

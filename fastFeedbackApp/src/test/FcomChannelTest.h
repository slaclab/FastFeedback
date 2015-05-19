/* 
 * File:   FcomChannelTest.h
 * Author: lpiccoli
 *
 * Created on July 2, 2010, 3:05 PM
 */

#ifndef _FCOMCHANNELTEST_H
#define	_FCOMCHANNELTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

#include "Defs.h"
#include "FcomChannel.h"

FF_NAMESPACE_START

class FcomChannelTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(FcomChannelTest);
    CPPUNIT_TEST(testInit);
    CPPUNIT_TEST(testRead);
    CPPUNIT_TEST(testWrite);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testInit();
    void testRead();
    void testWrite();

private:
    FcomChannel *fcomReadChannel;
    FcomChannel *fcomWriteChannel;
};

FF_NAMESPACE_END


#endif	/* _FCOMCHANNELTEST_H */


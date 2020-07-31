/* 
 * File:   CaChannelTest.h
 * Author: lpiccoli
 *
 * Created on June 29, 2010, 1:31 PM
 */

#ifndef _CACHANNELTEST_H
#define	_CACHANNELTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Defs.h"
#include "ChannelAccess.h"

FF_NAMESPACE_START

class CaChannelTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(CaChannelTest);
    CPPUNIT_TEST(testRead);
    CPPUNIT_TEST(testWrite);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testRead();
    void testWrite();

private:
    ChannelAccess _channelAccess;
};

FF_NAMESPACE_END

#endif	/* _CACHANNELTEST_H */


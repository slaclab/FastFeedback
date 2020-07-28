/* 
 * File:   PvDataTest.h
 * Author: lpiccoli
 *
 * Created on July 28, 2010, 10:11 AM
 */

#ifndef _PVDATATEST_H
#define	_PVDATATEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

FF_NAMESPACE_START

class PvDataTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PvDataTest);
    CPPUNIT_TEST(testAssign);
    CPPUNIT_TEST(testCheckMap);
    CPPUNIT_TEST(testDeviceSupport);
    CPPUNIT_TEST(testWrite);
    CPPUNIT_TEST(testReadExternal);
    CPPUNIT_TEST(testWriteExternal);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();

    void tearDown() {};

    void testAssign();
    void testCheckMap();
    void testDeviceSupport();
    void testWrite();
    void testReadExternal();
    void testWriteExternal();
};

FF_NAMESPACE_END

#endif	/* _PVDATATEST_H */

/* 
 * File:   ExecThreadTest.h
 * Author: lpiccoli
 *
 * Created on June 10, 2010, 2:16 PM
 */

#ifndef _EXECTHREADTEST_H
#define	_EXECTHREADTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include "Defs.h"

FF_NAMESPACE_START

class ExecThreadTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ExecThreadTest);
    CPPUNIT_TEST(testConfigure);
    //CPPUNIT_TEST(testRun);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    };

    void tearDown() {
    };

    void testConfigure();
    void testRun();
};

FF_NAMESPACE_END

#endif	/* _EXECTHREADTEST_H */


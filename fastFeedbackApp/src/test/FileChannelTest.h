/* 
 * File:   FileChannelTest.h
 * Author: lpiccoli
 *
 * Created on May 28, 2010, 1:05 PM
 */

#ifndef _FILECHANNELTEST_H
#define	_FILECHANNELTEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

class FileChannelTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FileChannelTest);
  CPPUNIT_TEST(testConstructor);
  CPPUNIT_TEST(testRead);
  CPPUNIT_TEST(testReadBadFile);
  CPPUNIT_TEST(testReadWrap);
  CPPUNIT_TEST(testWritingReadFile);
  CPPUNIT_TEST(testWrite);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {};
  void tearDown() {};

  void testConstructor();
  void testRead();
  void testWritingReadFile();
  void testReadBadFile();
  void testReadWrap();
  void testWrite();
  void testReadingWriteFile();
};


#endif	/* _FILECHANNELTEST_H */


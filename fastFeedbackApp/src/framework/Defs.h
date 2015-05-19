/* 
 * File:   Defs.h
 * Author: lpiccoli
 *
 * Created on May 24, 2010, 10:43 AM
 */

#ifndef _DEFS_H
#define	_DEFS_H

#define FF_NAMESPACE_START namespace FastFeedback {
#define FF_NAMESPACE_END }

#define USING_FF_NAMESPACE using namespace FastFeedback;

#define FF FastFeedback

/**
 * The following macros are temporary, while the CppUnit exception handling
 * problem is not resolved on RTEMS.
 *
 * If a test fails a CppUnit::Exception is thrown, and on RTEMS the try/catch
 * blocks are not handling them. The CPPUNIT_ASSERT macros are redefined here,
 * so CppUnit::Exceptions are not thrown on RTEMS.
 */

#ifdef RTEMS

#undef  CPPUNIT_ASSERT_EQUAL
#define CPPUNIT_ASSERT_EQUAL(expected, actual)  \
  if (expected != actual) {   \
    std::cout << "ERROR: " << __FILE__ << ":" << __LINE__ << std::endl;  \
    return;   \
  }

#undef CPPUNIT_ASSERT
#define CPPUNIT_ASSERT(condition) \
  if (!(condition)) { \
    std::cout << "ERROR: " << __FILE__ << ":" << __LINE__ << std::endl;  \
    return;   \
  }

#endif


#endif	/* _DEFS_H */


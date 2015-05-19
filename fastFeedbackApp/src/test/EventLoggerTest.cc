#include "EventLoggerTest.h"
#include "EventLogger.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::EventLoggerTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE

/**
 * @author L.Piccoli
 */
void FF::EventLoggerTest::testLog() {
  EventLogger::getInstance().add(MODE_EVENT, RECEIVED, "TR01", 0);
  CPPUNIT_ASSERT(EventLogger::getInstance()._head - EventLogger::getInstance()._tail == 1);

  EventLogger::getInstance().add(MODE_EVENT, PROCESSED, "TR01", 0);
  CPPUNIT_ASSERT(EventLogger::getInstance()._head - EventLogger::getInstance()._tail == 2);

  std::string loop("TR02");
  EventLogger::getInstance().add(MODE_EVENT, PROCESSED, loop, 0);
  CPPUNIT_ASSERT(EventLogger::getInstance()._head - EventLogger::getInstance()._tail == 3);

  //  EventLogger::getInstance().dump();
}

void FF::EventLoggerTest::testSetSamples() {
  EventLogger::getInstance().setSamples(2);
  EventLogger::getInstance().add(MODE_EVENT, RECEIVED, "TR01", 0);
  EventLogger::getInstance().add(MODE_EVENT, RECEIVED, "TR01", 1);
  EventLogger::getInstance().add(MODE_EVENT, RECEIVED, "TR01", 2);
  EventLogger::getInstance().add(MODE_EVENT, RECEIVED, "TR01", 3);
  EventLogger::getInstance().add(MODE_EVENT, RECEIVED, "TR01", 4);

  CPPUNIT_ASSERT(EventLogger::getInstance()._full == true);
  int pos = (EventLogger::getInstance()._head + 1) % EventLogger::getInstance()._size;
  CPPUNIT_ASSERT(pos == EventLogger::getInstance()._tail);

  EventLogger::getInstance().dump();
}

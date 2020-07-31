#include "PatternGeneratorTest.h"
#include "PatternGenerator.h"
#include "Pattern.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FF::PatternGeneratorTest, "FeedbackUnitTest");
CPPUNIT_REGISTRY_ADD_TO_DEFAULT("FeedbackUnitTest");

USING_FF_NAMESPACE


void FF::PatternGeneratorTest::setUp() {
    for (int i = 0; i < 6; ++i) {
        p1._inclusionMask[i] = i;
        p2._inclusionMask[i] = i + 10;
        p3._inclusionMask[i] = i + 50;
    }
}

/**
 * Make sure the patterns are generated in the correct order.
 *
 * @author L.Piccoli
 */
void FF::PatternGeneratorTest::testNextPattern() {
    PatternGenerator pg;

    // Add patterns to PatternGenerator
    pg.add(p1);
    pg.add(p2);
    pg.add(p3);
    pg.add(p2);

    // Sequence of patterns should be p1, p2, p3
    Pattern pp1(p1);
    Pattern pp2(p2);
    Pattern pp3(p3);
    CPPUNIT_ASSERT(pp1 == pg.next());
    CPPUNIT_ASSERT(pp2 == pg.next());
    CPPUNIT_ASSERT(pp3 == pg.next());

    CPPUNIT_ASSERT(pp1 == pg.next());
    CPPUNIT_ASSERT(pp2 == pg.next());
    CPPUNIT_ASSERT(pp3 == pg.next());

    CPPUNIT_ASSERT(pp1 == pg.next());
    CPPUNIT_ASSERT(pp2 == pg.next());
    CPPUNIT_ASSERT(pp3 == pg.next());
}

/**
 * Test if the correct number of patterns is generated.
 * We need to use PatternGenerator::getInstance() here because the timer
 * callback invokes the singleton to generate a pattern.
 *
 * @author L.Piccoli
 */
void FF::PatternGeneratorTest::testGenerate() {
    PatternGenerator::getInstance()._delay = 1;

    // Add some patterns
    PatternGenerator::getInstance().add(p1);
    PatternGenerator::getInstance().add(p2);
    PatternGenerator::getInstance().add(p2);
    PatternGenerator::getInstance().add(p3);

    // Clear the pattern count
    PatternGenerator::getInstance()._generatedPatternCount = 0;

    PatternGenerator::getInstance().start();
    epicsThreadSleep(6);
    PatternGenerator::getInstance().stop();

    CPPUNIT_ASSERT(PatternGenerator::getInstance()._generatedPatternCount > 4);
    CPPUNIT_ASSERT(PatternGenerator::getInstance()._generatedPatternCount < 7);
}

/**
 * Test if PatternGenerator can be started/stopped multiple times.
 *
 * @author L.Piccoli
 */
void FF::PatternGeneratorTest::testStartStop() {
    PatternGenerator::getInstance()._delay = 1;

    // Add some patterns
    PatternGenerator::getInstance().add(p1);
    PatternGenerator::getInstance().add(p2);
    PatternGenerator::getInstance().add(p2);
    PatternGenerator::getInstance().add(p3);

    PatternGenerator::getInstance().start();
    CPPUNIT_ASSERT(PatternGenerator::getInstance()._enabled);
    epicsThreadSleep(2);

    PatternGenerator::getInstance().stop();
    CPPUNIT_ASSERT(!PatternGenerator::getInstance()._enabled);

    PatternGenerator::getInstance().start();
    CPPUNIT_ASSERT(PatternGenerator::getInstance()._enabled);
    epicsThreadSleep(1);
    PatternGenerator::getInstance().stop();
    CPPUNIT_ASSERT(!PatternGenerator::getInstance()._enabled);

    PatternGenerator::getInstance().start();
    CPPUNIT_ASSERT(PatternGenerator::getInstance()._enabled);
    epicsThreadSleep(3);
    PatternGenerator::getInstance().stop();
    CPPUNIT_ASSERT(!PatternGenerator::getInstance()._enabled);
}

/**
 * Check whether the pattern mask removal is working.
 * @author L.Piccoli
 */
void FF::PatternGeneratorTest::testRemovePattern() {
    PatternGenerator pg;

    pg.add(p1);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, pg._patternMasks.size());

    pg.remove(p1);
    CPPUNIT_ASSERT_EQUAL((size_t) 0, pg._patternMasks.size());
}

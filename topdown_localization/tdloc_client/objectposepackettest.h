/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   10/2/2007
 *
 */

#include <cppunit/extensions/HelperMacros.h>
#include "./objectposepacket.h"

using namespace rlab;

class ObjectPosePacketTest : public CppUnit::TestFixture
{
public:
  CPPUNIT_TEST_SUITE(ObjectPosePacketTest);
  CPPUNIT_TEST(testParse);
  CPPUNIT_TEST(testGenerate);
  CPPUNIT_TEST_SUITE_END();

  void testParse()
  {
  }

  void testGenerate()
  {
  }
};


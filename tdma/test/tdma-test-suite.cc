#include "ns3/test.h"


using namespace ns3;

class TdmaTestCase1 : public TestCase
{
public:
  TdmaTestCase1 ();
  virtual ~TdmaTestCase1 ();

private:
  virtual void DoRun (void);
};

TdmaTestCase1::TdmaTestCase1 ()
  : TestCase ("Tdma test case (does nothing)") {}
TdmaTestCase1::~TdmaTestCase1 () {}

void TdmaTestCase1::DoRun (void) {
  // A wide variety of test macros are available in src/core/test.h
  NS_TEST_ASSERT_MSG_EQ (true, true, "true doesn't equal true for some reason");
  // Use this one for floating point comparisons
  NS_TEST_ASSERT_MSG_EQ_TOL (0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

class TdmaTestSuite : public TestSuite {
public:
  TdmaTestSuite ();
};

TdmaTestSuite::TdmaTestSuite ()
  : TestSuite ("tdma", UNIT) {
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new TdmaTestCase1, TestCase::QUICK);
}
// Do not forget to allocate an instance of this TestSuite
static TdmaTestSuite tdmaTestSuite;


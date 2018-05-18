/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/test.h"
#include "ns3/mesh-helper.h"
#include "ns3/simulator.h"
#include "ns3/mobility-helper.h"
#include "ns3/uav-routing-helper.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/v4ping-helper.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/pcap-file.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/uav-packet.h"
#include "ns3/uav-rtable.h"
// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

// This is an example TestCase.
class UAVHeaderTestCase : public TestCase
{
public:
  UAVHeaderTestCase ();
  ~UAVHeaderTestCase ();
  virtual void
  DoRun (void);
};
UAVHeaderTestCase::UAVHeaderTestCase ()
  : TestCase ("Verifying the UAV header")
{
}
UAVHeaderTestCase::~UAVHeaderTestCase ()
{
}
void
UAVHeaderTestCase::DoRun ()
{
  Ptr<Packet> packet = Create<Packet> ();

  {
    UAVRouting::UAVHeader hdr1;
    hdr1.SetAddress (Ipv4Address ("10.1.1.2"));
    packet->AddHeader (hdr1);
    UAVRouting::UAVHeader hdr2;
    hdr2.SetAddress (Ipv4Address ("10.1.1.3"));
    packet->AddHeader (hdr2);
    NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), 24, "001");
  }

  {
    UAVRouting::UAVHeader hdr2;
    packet->RemoveHeader (hdr2);
    NS_TEST_ASSERT_MSG_EQ (hdr2.GetSerializedSize (),72,"002");
    NS_TEST_ASSERT_MSG_EQ (hdr2.GetAddress (),Ipv4Address ("10.1.1.3"),"003");
    UAVRouting::UAVHeader hdr1;
    packet->RemoveHeader (hdr1);
    NS_TEST_ASSERT_MSG_EQ (hdr1.GetSerializedSize (),72,"006");
    NS_TEST_ASSERT_MSG_EQ (hdr1.GetAddress (),Ipv4Address ("10.1.1.2"),"008");
  }
}

/**
 * \ingroup uav-routing-test
 * \ingroup tests
 *
 * \brief UAV routing table tests (adding and looking up routes)
 */
class UAVTableTestCase : public TestCase
{
public:
  UAVTableTestCase ();
  ~UAVTableTestCase ();
  virtual void
  DoRun (void);
};

UAVTableTestCase::UAVTableTestCase ()
  : TestCase ("UAV Routing Table test case")
{
}
UAVTableTestCase::~UAVTableTestCase ()
{
}
void
UAVTableTestCase::DoRun ()
{
  UAVRouting::DistanceTable disTable;
  Ptr<NetDevice> dev;
  {

  }
  {

  }
  Simulator::Destroy ();
}

/**
 * \ingroup uav-routing-test
 * \ingroup tests
 *
 * \brief UAVRouting test suite
 */
class UAVRoutingTestSuite : public TestSuite
{
public:
  UAVRoutingTestSuite () : TestSuite ("uav-routing", UNIT)
  {
    AddTestCase (new UAVHeaderTestCase (), TestCase::QUICK);
    AddTestCase (new UAVTableTestCase (), TestCase::QUICK);
  }
} g_UAVRoutingTestSuite; ///< the test suite

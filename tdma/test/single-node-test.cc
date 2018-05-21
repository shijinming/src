#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/tdma-module.h"
#include "ns3/mobility-module.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/packet-socket-address.h"
#include "ns3/on-off-helper.h"
#include "ns3/llc-snap-header.h"
#include "single-node-test.h"

using namespace ns3;

namespace tdma {
    TdmaSingleNodeTest::TdmaSingleNodeTest ()
            : ns3::TestCase ("TdmaSingleNodeTest") {}
    
    void TdmaSingleNodeTest::DoRun (void) {
        ns3::SeedManager::SetSeed (1);

        tdma::TdmaHelper tdma;
        tdma.SetStandard(ns3::WIFI_PHY_STANDARD_80211n_5GHZ);
        tdma::TdmaMacHelper TdmaMac = tdma::TdmaMacHelper::Default();

        ns3::Config::SetDefault ("tdma::TdmaMac::FrameDuration", ns3::TimeValue(ns3::Seconds(1.0)));
        ns3::Config::SetDefault ("tdma::TdmaMac::MaximumPacketSize", ns3::UintegerValue(400));
        ns3::Config::SetDefault ("tdma::TdmaMac::ReportRate", ns3::UintegerValue(10));
        ns3::Config::SetDefault ("tdma::TdmaMac::Timeout",
                                 ns3::UintegerValue (ns3::CreateObject<ns3::UniformRandomVariable>()->GetInteger(8, 8)));;

        // Create network nodes
        ns3::NodeContainer m_nodes;
        m_nodes.Create(1);

        // Configure the wireless channel characteristics
        ns3::YansWifiChannelHelper wifiChannel;
        wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
        ns3::Config::SetDefault ("ns3::LogDistancePropagationLossModel::Exponent", ns3::DoubleValue(1.85));
        ns3::Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", ns3::DoubleValue(59.7));
        wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

        ns3::YansWifiPhyHelper wifiPhy = ns3::YansWifiPhyHelper::Default();
        wifiPhy.SetChannel(wifiChannel.Create());

        // Install wifiPhy and link it with tdma medium access control layer implementation
        tdma.Install(wifiPhy, TdmaMac, m_nodes);

        // Define positions of the two nodes
        ns3::MobilityHelper mobility;
        ns3::Ptr<ns3::ListPositionAllocator> positionAlloc = ns3::CreateObject<ns3::ListPositionAllocator>();
        positionAlloc->Add(ns3::Vector(0.0, 0.0, 0.0));
        mobility.SetPositionAllocator(positionAlloc);
        mobility.Install(m_nodes);

        // Add packet socket handlers
        ns3::PacketSocketHelper packetSocket;
        packetSocket.Install(m_nodes);

        // Configure packet socket: use broadcasts
        ns3::PacketSocketAddress socket;
        socket.SetAllDevices();
        socket.SetPhysicalAddress(ns3::Mac48Address::GetBroadcast());
        socket.SetProtocol(1);

        ns3::OnOffHelper onOff ("ns3::PacketSocketFactory", ns3::Address (socket));
        onOff.SetAttribute ("PacketSize", ns3::UintegerValue (400 - GetProtocolOverheads()));
        onOff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
        onOff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
        onOff.SetAttribute ("DataRate", ns3::DataRateValue (ns3::DataRate ("40kb/s")));

        ns3::ApplicationContainer app;
        app = onOff.Install (m_nodes.Get (0));
        app.Start(ns3::Seconds (0.0));
        app.Stop(ns3::Seconds (4.1));

        ns3::Config::Connect("/NodeList/*/DeviceList/*/$Tdma::TdmaNetDevice/Mac/Startup", 
                             ns3::MakeCallback (&tdma::TdmaSingleNodeTest::TdmaStartupTrace, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$Tdma::TdmaNetDevice/Mac/Tx",
                             ns3::MakeCallback (&tdma::TdmaSingleNodeTest::TdmaTxTrace, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$Tdma::TdmaNetDevice/Mac/NetworkEntry",
                             ns3::MakeCallback (&tdma::TdmaSingleNodeTest::TdmaNetworkEntry, this) );

        ns3::Simulator::Stop(ns3::Seconds(4.1));

        m_count = 0;
        ns3::Simulator::Run ();
        ns3::Simulator::Destroy ();
        NS_TEST_EXPECT_MSG_EQ (m_count, 30, "The station should have transmitted exactly 30 packets");
    }
    void TdmaSingleNodeTest::TdmaTxTrace (std::string context, ns3::Ptr<const ns3::Packet> p, 
                                            uint32_t no, uint8_t timeout, uint32_t offset) {
        NS_TEST_EXPECT_MSG_EQ (m_nextTx.GetNanoSeconds(), ns3::Simulator::Now().GetNanoSeconds(), 
                               "The expected next transmission time should match the current time stamp.");
        NS_TEST_EXPECT_MSG_EQ (p->GetSize(), 400, "The packets should all be 400bytes long.");
        NS_TEST_EXPECT_MSG_EQ (no, m_count % 10, "The packet number should match the expected sequence.");
        NS_TEST_EXPECT_MSG_EQ (timeout, 8 - ceil((m_count + 1) / 10.0), "The timeout value should be exactly as expected.");
        m_count++;
        m_nextTx = ns3::Simulator::Now() + ns3::NanoSeconds(offset * m_slotDuration.GetNanoSeconds());
    }
    void TdmaSingleNodeTest::TdmaStartupTrace (std::string context, ns3::Time when, ns3::Time frameDuration, 
                                                 ns3::Time slotDuration) {
        NS_TEST_EXPECT_MSG_EQ (frameDuration, ns3::NanoSeconds(999556000.0), "The frame duration should be +999556000.0ns");
        NS_TEST_EXPECT_MSG_EQ (slotDuration, ns3::NanoSeconds(566000.0), "The frame duration should be +566000.0ns");
        m_slotDuration = slotDuration;
    }
    void TdmaSingleNodeTest::TdmaNetworkEntry(std::string context, ns3::Ptr<const ns3::Packet> p, 
                                                ns3::Time offset, bool isTaken) {
        m_nextTx = ns3::Simulator::Now() + offset;
    }

    uint32_t TdmaSingleNodeTest::GetProtocolOverheads() {
        ns3::WifiMacHeader hdr;
        hdr.SetTypeData();
        hdr.SetDsNotFrom();
        hdr.SetDsNotTo();
        tdma::TdmaHeader tdmaHdr;
        ns3::WifiMacTrailer fcs;
        return hdr.GetSerializedSize() + tdmaHdr.GetSerializedSize() + fcs.GetSerializedSize() + ns3::LLC_SNAP_HEADER_LENGTH;
    }

} // namespace tdma

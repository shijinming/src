#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/node-list.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/network-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/tdma-module.h"

#include <string.h>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <numeric>

using namespace ns3;

class Experiment {

public:
    void Run () {
        ns3::SeedManager::SetSeed (1);

        tdma::TdmaHelper tdma1;
        tdma1.SetStandard(ns3::WIFI_PHY_STANDARD_80211n_5GHZ);
        tdma::TdmaMacHelper tdmaMac = tdma::TdmaMacHelper::Default();

        ns3::Config::SetDefault ("tdma::TdmaMac::FrameDuration", ns3::TimeValue(ns3::Seconds(1.0)));
        ns3::Config::SetDefault ("tdma::TdmaMac::MaximumPacketSize", ns3::UintegerValue(400));
        ns3::Config::SetDefault ("tdma::TdmaMac::ReportRate", ns3::UintegerValue(10));
       ns3::Config::SetDefault ("tdma::TdmaMac::Timeout",
                                ns3::PointerValue(ns3::CreateObject<ns3::UniformRandomVariable>()));
        // Create network nodes
        ns3::NodeContainer m_nodes;
        m_nodes.Create(10);
        // Configure the wireless channel characteristics
        ns3::YansWifiChannelHelper wifiChannel;
        wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
        ns3::Config::SetDefault ("ns3::LogDistancePropagationLossModel::Exponent", ns3::DoubleValue(1.85));
        ns3::Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", ns3::DoubleValue(59.7));
        wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

        ns3::YansWifiPhyHelper wifiPhy = ns3::YansWifiPhyHelper::Default();
        wifiPhy.SetChannel(wifiChannel.Create());
        // Install wifiPhy and link it with TDMA medium access control layer implementation
        tdma1.Install(wifiPhy, tdmaMac, m_nodes);
        // Define positions of the two nodes
        ns3::MobilityHelper mobility;
        ns3::Ptr<ns3::ListPositionAllocator> positionAlloc = ns3::CreateObject<ns3::ListPositionAllocator>();
        for (uint32_t i = 0; i < 10; i++) {
            positionAlloc->Add(ns3::Vector(1.0 * i, 0.0, 0.0));
        }
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
        app = onOff.Install (m_nodes);
        app.Start(ns3::Seconds (0.0));
        app.Stop(ns3::Seconds (20.0));

        ns3::Config::Connect("/NodeList/*/DeviceList/*/$tdma::TdmaNetDevice/Mac/Startup",
                             ns3::MakeCallback (&Experiment::TdmaStartupTrace, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$tdma::TdmaNetDevice/Mac/NetworkEntry",
                             ns3::MakeCallback (&Experiment::TdmaNetworkEntry, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$tdma::TdmaNetDevice/Mac/Tx",
                             ns3::MakeCallback (&Experiment::TdmaTxTrace, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$tdma::TdmaNetDevice/Mac/Rx",
                             ns3::MakeCallback (&Experiment::TdmaRxTrace, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$tdma::TdmaNetDevice/Mac/SlotManager/NominalSlotSelection",
                             MakeCallback(&Experiment::TdmaNominalSlotsTrace, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$tdma::TdmaNetDevice/Mac/SlotManager/SlotReservation",
                             MakeCallback(&Experiment::TdmaSlotReservation, this) );
        ns3::Config::Connect("/NodeList/*/DeviceList/*/$tdma::TdmaNetDevice/Mac/SlotManager/SlotReReservation",
                             MakeCallback(&Experiment::TdmaSlotReReservation, this) );

        ns3::Simulator::Stop(ns3::Seconds(20.0));

        ns3::Simulator::Run ();
        ns3::Simulator::Destroy ();
    }

private:
    uint32_t GetProtocolOverheads() {
        ns3::WifiMacHeader hdr;
        hdr.SetTypeData();
        hdr.SetDsNotFrom();
        hdr.SetDsNotTo();
        tdma::TdmaHeader tdmaHdr;
        ns3::WifiMacTrailer fcs;
        return hdr.GetSerializedSize() + tdmaHdr.GetSerializedSize() + fcs.GetSerializedSize() + ns3::LLC_SNAP_HEADER_LENGTH;
    }
    void TdmaStartupTrace (std::string context, ns3::Time when, ns3::Time frameDuration, ns3::Time slotDuration)
    {
        Ptr<Node> node = NodeList::GetNode(Simulator::GetContext());
        std::cout << "Node " << node->GetId() << " started up!" << std::endl;
    }
    void TdmaNetworkEntry(std::string context, ns3::Ptr<const ns3::Packet> p , ns3::Time when, bool isTaken) {
        Ptr<Node> node = NodeList::GetNode(Simulator::GetContext());
        std::cout << "Node " << node->GetId() << " enters the network and announces its presence!" << std::endl;
    }
    void TdmaTxTrace (std::string context, ns3::Ptr<const ns3::Packet> p, uint32_t no, 
                       uint8_t timeout, uint32_t offset) {
        Ptr<Node> node = NodeList::GetNode(Simulator::GetContext());
        std::cout << "Node " << node->GetId() << " transmits packet " << p->GetUid()
                  << " (offset = " << offset << ", timeout = " << (uint32_t) timeout << ")" << std::endl;
    }
    void TdmaRxTrace (std::string context, ns3::Ptr<const ns3::Packet> p, uint8_t timeout, uint32_t offset) {
        Ptr<Node> node = NodeList::GetNode(Simulator::GetContext());
        std::cout << "Node " << node->GetId() << " receives packet " << p->GetUid()
                  << " (offset = " << offset << ", timeout = " << (uint32_t) timeout << ")" << std::endl;
    }
    void TdmaNominalSlotsTrace(std::string context, std::vector<uint32_t> slots) {
        Ptr<Node> node = NodeList::GetNode(Simulator::GetContext());
    }
    void TdmaSlotReservation(std::string context, uint32_t numCandidates, uint32_t numFree, bool wasFree) {
        Ptr<Node> node = NodeList::GetNode(Simulator::GetContext());
        std::cout << "Node " << node->GetId() << " performs initial slot reservation (numCandidates = " << numCandidates
                  << ", numFree = " << numFree << ", wasFree = " << wasFree << ")" << std::endl;
    }
    void TdmaSlotReReservation(std::string context, uint32_t numCandidates, uint32_t numFree, bool wasFree, bool isSame) {
        Ptr<Node> node = NodeList::GetNode(Simulator::GetContext());
        std::cout << "Node " << node->GetId() << " performs slot re-reservation (numCandidates = " << numCandidates
                  << ", numFree = " << numFree << ", wasFree = " << wasFree << ", isSame = " << isSame << ")" << std::endl;
    }
};

int main (int argc, char *argv[]) {
    Experiment e;
    e.Run();
    return 0;
}

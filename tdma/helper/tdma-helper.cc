/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "tdma-helper.h"
#include "tdma-mac-helper.h"
#include "ns3/tdma-mac.h"
#include "ns3/tdma-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/mobility-model.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/names.h"

NS_LOG_COMPONENT_DEFINE ("TdmaHelper");

namespace tdma {

    TdmaHelper::TdmaHelper ()
            : m_standard (ns3::WIFI_PHY_STANDARD_80211n_5GHZ) {}
    TdmaHelper TdmaHelper::Default (void) {
        TdmaHelper helper;
        return helper;
    }
    void TdmaHelper::SetStandard (enum ns3::WifiPhyStandard standard) {
        m_standard = standard;
    }
    ns3::NetDeviceContainer TdmaHelper::Install (const ns3::WifiPhyHelper &phyHelper,
                          const TdmaMacHelper &macHelper, ns3::NodeContainer c, std::vector<ns3::Time> startups) const {
        NS_ASSERT(c.GetN() <= startups.size());
        ns3::NetDeviceContainer devices;
        uint32_t index = 0;
        for (ns3::NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
        {
            ns3::Ptr<ns3::Node> node = *i;
            ns3::Ptr<TdmaNetDevice> device = ns3::CreateObject<TdmaNetDevice> ();
            ns3::Ptr<TdmaMac> mac = macHelper.Create ();
            ns3::Ptr<ns3::WifiPhy> phy = phyHelper.Create (node, device);
            mac->SetAddress (ns3::Mac48Address::Allocate ());
            mac->ConfigureStandard (m_standard);
            phy->ConfigureStandard (m_standard);
            device->SetMac (mac);
            device->SetPhy (phy);
            node->AddDevice (device);
            devices.Add (device);
            ns3::Simulator::ScheduleWithContext(node->GetId(), startups[index], &TdmaMac::StartInitializationPhase, mac);
            index++;
        }
        return devices;
    }
    ns3::NetDeviceContainer TdmaHelper::Install (const ns3::WifiPhyHelper &phyHelper,
                          const TdmaMacHelper &macHelper, ns3::NodeContainer c) const {
        ns3::NetDeviceContainer devices;
        for (ns3::NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i) {
            ns3::Ptr<ns3::Node> node = *i;
            ns3::Ptr<TdmaNetDevice> device = ns3::CreateObject<TdmaNetDevice> ();
            ns3::Ptr<TdmaMac> mac = macHelper.Create ();
            ns3::Ptr<ns3::WifiPhy> phy = phyHelper.Create (node, device);
            mac->SetAddress (ns3::Mac48Address::Allocate ());
            mac->ConfigureStandard (m_standard);
            phy->ConfigureStandard (m_standard);
            device->SetMac (mac);
            device->SetPhy (phy);
            node->AddDevice (device);
            devices.Add (device);
            ns3::Simulator::ScheduleWithContext(node->GetId(), ns3::Seconds(0), &TdmaMac::StartInitializationPhase, mac);
        }
        return devices;
    }
    ns3::NetDeviceContainer TdmaHelper::Install (const ns3::WifiPhyHelper &phy,
                          const TdmaMacHelper &mac, ns3::Ptr<ns3::Node> node) const {
        return Install (phy, mac, ns3::NodeContainer (node));
    }
    ns3::NetDeviceContainer TdmaHelper::Install (const ns3::WifiPhyHelper &phy,
                          const TdmaMacHelper &mac, std::string nodename) const {
        ns3::Ptr<ns3::Node> node = ns3::Names::Find<ns3::Node> (nodename);
        return Install (phy, mac, ns3::NodeContainer (node));
    }

} // namespace tdma


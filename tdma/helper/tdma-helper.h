/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_HELPER_H
#define TDMA_HELPER_H

#include <string>
#include <vector>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/wifi-phy-standard.h"
#include "ns3/wifi-helper.h"
#include "ns3/trace-helper.h"
#include "ns3/wifi-phy.h"
#include "tdma-mac-helper.h"

namespace tdma {

class TdmaHelper{
public:
    TdmaHelper();
    static TdmaHelper Default(void);
    ns3::NetDeviceContainer Install(const ns3::WifiPhyHelper &phy, const TdmaMacHelper &mac,
                                    ns3::NodeContainer c, std::vector<ns3::Time> startups) const;
    ns3::NetDeviceContainer Install(const ns3::WifiPhyHelper &phy, const TdmaMacHelper &mac,
                                    ns3::NodeContainer c) const;
    ns3::NetDeviceContainer Install(const ns3::WifiPhyHelper &phy, const TdmaMacHelper &mac,
                                    ns3::Ptr<ns3::Node> node) const;
    ns3::NetDeviceContainer Install(const ns3::WifiPhyHelper &phy, const TdmaMacHelper &mac,
                                    std::string nodename) const;
    void SetStandard(enum ns3::WifiPhyStandard standard);


private:
    enum ns3::WifiPhyStandard m_standard;

};


}

#endif /* TDMA_HELPER_H */


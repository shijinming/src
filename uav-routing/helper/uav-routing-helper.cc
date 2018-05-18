/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "uav-routing-helper.h"
#include "ns3/uav-routing-protocol.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include "ns3/ipv4-list-routing.h"

namespace ns3 {
UAVRoutingHelper::~UAVRoutingHelper ()
{
}

UAVRoutingHelper::UAVRoutingHelper () : Ipv4RoutingHelper ()
{
  m_agentFactory.SetTypeId ("ns3::UAVRouting::RoutingProtocol");
}

UAVRoutingHelper*
UAVRoutingHelper::Copy (void) const
{
  return new UAVRoutingHelper (*this);
}

Ptr<Ipv4RoutingProtocol>
UAVRoutingHelper::Create (Ptr<Node> node) const
{
  Ptr<UAVRouting::RoutingProtocol> agent = m_agentFactory.Create<UAVRouting::RoutingProtocol> ();
  node->AggregateObject (agent);
  return agent;
}

void
UAVRoutingHelper::Set (std::string name, const AttributeValue &value)
{
  m_agentFactory.Set (name, value);
}

}


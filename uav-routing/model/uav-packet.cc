/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "uav-packet.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"

namespace ns3 {
namespace UAVRouting {

NS_OBJECT_ENSURE_REGISTERED (UAVHeader);

//UAVHeader::UAVHeader (Ipv4Address dst, uint32_t hopCount, uint32_t dstSeqNo)
//  : m_dst (dst),
//    m_hopCount (hopCount),
//    m_dstSeqNo (dstSeqNo)
//{
//}
UAVHeader::UAVHeader (Ipv4Address addr)
    :m_address(addr)
{

}

UAVHeader::~UAVHeader ()
{
}

TypeId
UAVHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UAVRouting::UAVHeader")
    .SetParent<Header> ()
    .SetGroupName ("UAVRouting")
    .AddConstructor<UAVHeader> ();
  return tid;
}

TypeId
UAVHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
UAVHeader::GetSerializedSize () const
{
  // return 12;
  return 72;
}

void
UAVHeader::Serialize (Buffer::Iterator i) const
{
  WriteTo (i, m_address);
  i.WriteHtonU64(m_timestamp);
  i.WriteHtonU64(m_position.x*100);
  i.WriteHtonU64(m_position.y*100);
  i.WriteHtonU64(m_position.z*100);
  i.WriteHtonU64(m_velocity.x*100);
  i.WriteHtonU64(m_velocity.y*100);
  i.WriteHtonU64(m_velocity.z*100);
  i.WriteHtonU32(m_queueLen);
  i.WriteHtonU64(m_energy*10000);

}

uint32_t
UAVHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  ReadFrom (i, m_address);
  m_timestamp = i.ReadNtohU64();
  m_position = Vector (int64_t(i.ReadNtohU64())/100.0, int64_t(i.ReadNtohU64())/100.0, int64_t(i.ReadNtohU64())/100.0);
  m_velocity = Vector (int64_t(i.ReadNtohU64())/100.0, int64_t(i.ReadNtohU64())/100.0, int64_t(i.ReadNtohU64())/100.0);
  m_queueLen = i.ReadNtohU32();
  m_energy = int64_t(i.ReadNtohU64())/10000.0;

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
UAVHeader::Print (std::ostream &os) const
{
  os << "DestinationIpv4: " << m_address
     << "\tPosition: " << m_position
     << "\tVelocity: " << m_velocity
     << "\tQueue length: " << m_queueLen
     << "\tEnergy: " << m_energy
     << "\tTimestamp: " << m_timestamp;
}
}
}

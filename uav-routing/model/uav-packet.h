/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef UAV_PACKET_H
#define UAV_PACKET_H

#include <iostream>
#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/vector.h"

namespace ns3 {
namespace UAVRouting {
/**
 * \ingroup UAVRouting
 * \brief UAV Update Packet Format
 * \verbatim
 |      0        |      1        |      2        |       3       |
  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                      Destination Address                      |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                            HopCount                           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                       Sequence Number                         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * \endverbatim
 */

class UAVHeader : public Header
{
public:
  /**
   * Constructor
   *
   * \param dst destination IP address
   * \param hopcount hop count
   * \param dstSeqNo destination sequence number
   */
  // UAVHeader (Ipv4Address dst = Ipv4Address (), uint32_t hopcount = 0, uint32_t dstSeqNo = 0);
  UAVHeader (Ipv4Address addr = Ipv4Address ());
  virtual ~UAVHeader ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  /**
   * Set destination address
   * \param destination the destination IPv4 address
   */
//  void
//  SetDst (Ipv4Address destination)
//  {
//    m_dst = destination;
//  }
//  /**
//   * Get destination address
//   * \returns the destination IPv4 address
//   */
//  Ipv4Address
//  GetDst () const
//  {
//    return m_dst;
//  }
//  /**
//   * Set hop count
//   * \param hopCount the hop count
//   */
//  void
//  SetHopCount (uint32_t hopCount)
//  {
//    m_hopCount = hopCount;
//  }
//  /**
//   * Get hop count
//   * \returns the hop count
//   */
//  uint32_t
//  GetHopCount () const
//  {
//    return m_hopCount;
//  }
//  /**
//   * Set destination sequence number
//   * \param sequenceNumber The sequence number
//   */
//  void
//  SetDstSeqno (uint32_t sequenceNumber)
//  {
//    m_dstSeqNo = sequenceNumber;
//  }
//  /**
//   * Get destination sequence number
//   * \returns the destination sequence number
//   */
//  uint32_t
//  GetDstSeqno () const
//  {
//    return m_dstSeqNo;
//  }

  void
  SetAddress(Ipv4Address addr)
  {
    m_address = addr;
  }

  Ipv4Address
  GetAddress() const
  {
    return m_address;
  }

  void
  SetTimeStamp(uint64_t timestamp)
  {
    m_timestamp = timestamp;
  }

  uint64_t
  GetTimeStamp() const
  {
    return m_timestamp;
  }

  void
  SetPosition(Vector pos)

  {
    m_position = pos;
  }

  Vector
  GetPosition() const
  {
    return m_position;
  }

  void
  SetVelocity(Vector v)
  {
      m_velocity = v;
  }

  Vector
  GetVelocity() const
  {
      return m_velocity;
  }

  void
  SetQueueLen(uint32_t len)
  {
      m_queueLen = len;
  }

  uint32_t
  GetQueueLen() const
  {
      return m_queueLen;
  }

  void
  SetEnergy(double e)
  {
      m_energy = e;
  }

  double
  GetEnergy() const
  {
      return m_energy;
  }

private:
  Ipv4Address m_address; ///< Destination IP Address
  // uint32_t m_hopCount; ///< Number of Hops
  // uint32_t m_dstSeqNo; ///< Destination Sequence Number
  uint64_t m_timestamp;
  Vector m_position;
  Vector m_velocity;
  uint32_t m_queueLen;
  double m_energy;
};
static inline std::ostream & operator<< (std::ostream& os, const UAVHeader & packet)
{
  packet.Print (os);
  return os;
}
}
}

#endif /* UAV_PACKET_H */

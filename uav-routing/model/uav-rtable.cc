/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "uav-rtable.h"
#include "ns3/simulator.h"
#include <iomanip>
#include "ns3/log.h"
#include <cstring>

#define INF 99999999

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UAVRoutingTable");

namespace UAVRouting {
RoutingTableEntry::RoutingTableEntry (Ptr<NetDevice> dev,
                                      Ipv4Address dst,
                                      uint32_t seqNo,
                                      Ipv4InterfaceAddress iface,
                                      uint32_t hops,
                                      Ipv4Address nextHop,
                                      Time lifetime,
                                      Time SettlingTime,
                                      bool areChanged)
  : m_seqNo (seqNo),
    m_hops (hops),
    m_lifeTime (lifetime),
    m_iface (iface),
    m_flag (VALID),
    m_settlingTime (SettlingTime),
    m_entriesChanged (areChanged)
{
  m_ipv4Route = Create<Ipv4Route> ();
  m_ipv4Route->SetDestination (dst);
  m_ipv4Route->SetGateway (nextHop);
  m_ipv4Route->SetSource (m_iface.GetLocal ());
  m_ipv4Route->SetOutputDevice (dev);
}
RoutingTableEntry::~RoutingTableEntry ()
{
}
RoutingTable::RoutingTable ()
{
}

bool
RoutingTable::LookupRoute (Ipv4Address id,
                           RoutingTableEntry & rt)
{
  if (m_ipv4AddressEntry.empty ())
    {
      return false;
    }
  std::map<Ipv4Address, RoutingTableEntry>::const_iterator i = m_ipv4AddressEntry.find (id);
  if (i == m_ipv4AddressEntry.end ())
    {
      return false;
    }
  rt = i->second;
  return true;
}

bool
RoutingTable::LookupRoute (Ipv4Address id,
                           RoutingTableEntry & rt,
                           bool forRouteInput)
{
  if (m_ipv4AddressEntry.empty ())
    {
      return false;
    }
  std::map<Ipv4Address, RoutingTableEntry>::const_iterator i = m_ipv4AddressEntry.find (id);
  if (i == m_ipv4AddressEntry.end ())
    {
      return false;
    }
  if (forRouteInput == true && id == i->second.GetInterface ().GetBroadcast ())
    {
      return false;
    }
  rt = i->second;
  return true;
}

bool
RoutingTable::DeleteRoute (Ipv4Address dst)
{
  if (m_ipv4AddressEntry.erase (dst) != 0)
    {
      // NS_LOG_DEBUG("Route erased");
      return true;
    }
  return false;
}

uint32_t
RoutingTable::RoutingTableSize ()
{
  return m_ipv4AddressEntry.size ();
}

bool
RoutingTable::AddRoute (RoutingTableEntry & rt)
{
  std::pair<std::map<Ipv4Address, RoutingTableEntry>::iterator, bool> result = m_ipv4AddressEntry.insert (std::make_pair (
                                                                                                            rt.GetDestination (),rt));
  return result.second;
}

bool
RoutingTable::Update (RoutingTableEntry & rt)
{
  std::map<Ipv4Address, RoutingTableEntry>::iterator i = m_ipv4AddressEntry.find (rt.GetDestination ());
  if (i == m_ipv4AddressEntry.end ())
    {
      return false;
    }
  i->second = rt;
  return true;
}

void
RoutingTable::DeleteAllRoutesFromInterface (Ipv4InterfaceAddress iface)
{
  if (m_ipv4AddressEntry.empty ())
    {
      return;
    }
  for (std::map<Ipv4Address, RoutingTableEntry>::iterator i = m_ipv4AddressEntry.begin (); i != m_ipv4AddressEntry.end (); )
    {
      if (i->second.GetInterface () == iface)
        {
          std::map<Ipv4Address, RoutingTableEntry>::iterator tmp = i;
          ++i;
          m_ipv4AddressEntry.erase (tmp);
        }
      else
        {
          ++i;
        }
    }
}

void
RoutingTable::GetListOfAllRoutes (std::map<Ipv4Address, RoutingTableEntry> & allRoutes)
{
  for (std::map<Ipv4Address, RoutingTableEntry>::iterator i = m_ipv4AddressEntry.begin (); i != m_ipv4AddressEntry.end (); ++i)
    {
      if (i->second.GetDestination () != Ipv4Address ("127.0.0.1") && i->second.GetFlag () == VALID)
        {
          allRoutes.insert (
            std::make_pair (i->first,i->second));
        }
    }
}

void
RoutingTable::GetListOfDestinationWithNextHop (Ipv4Address nextHop,
                                               std::map<Ipv4Address, RoutingTableEntry> & unreachable)
{
  unreachable.clear ();
  for (std::map<Ipv4Address, RoutingTableEntry>::const_iterator i = m_ipv4AddressEntry.begin (); i
       != m_ipv4AddressEntry.end (); ++i)
    {
      if (i->second.GetNextHop () == nextHop)
        {
          unreachable.insert (std::make_pair (i->first,i->second));
        }
    }
}

void
RoutingTableEntry::Print (Ptr<OutputStreamWrapper> stream) const
{
  *stream->GetStream () << std::setiosflags (std::ios::fixed) << m_ipv4Route->GetDestination () << "\t\t" << m_ipv4Route->GetGateway () << "\t\t"
                        << m_iface.GetLocal () << "\t\t" << std::setiosflags (std::ios::left)
                        << std::setw (10) << m_hops << "\t" << std::setw (10) << m_seqNo << "\t"
                        << std::setprecision (3) << (Simulator::Now () - m_lifeTime).GetSeconds ()
                        << "s\t\t" << m_settlingTime.GetSeconds () << "s\n";
}

void
RoutingTable::Purge (std::map<Ipv4Address, RoutingTableEntry> & removedAddresses)
{
  if (m_ipv4AddressEntry.empty ())
    {
      return;
    }
  for (std::map<Ipv4Address, RoutingTableEntry>::iterator i = m_ipv4AddressEntry.begin (); i != m_ipv4AddressEntry.end (); )
    {
      std::map<Ipv4Address, RoutingTableEntry>::iterator itmp = i;
      if (i->second.GetLifeTime () > m_holddownTime && (i->second.GetHop () > 0))
        {
          for (std::map<Ipv4Address, RoutingTableEntry>::iterator j = m_ipv4AddressEntry.begin (); j != m_ipv4AddressEntry.end (); )
            {
              if ((j->second.GetNextHop () == i->second.GetDestination ()) && (i->second.GetHop () != j->second.GetHop ()))
                {
                  std::map<Ipv4Address, RoutingTableEntry>::iterator jtmp = j;
                  removedAddresses.insert (std::make_pair (j->first,j->second));
                  ++j;
                  m_ipv4AddressEntry.erase (jtmp);
                }
              else
                {
                  ++j;
                }
            }
          removedAddresses.insert (std::make_pair (i->first,i->second));
          ++i;
          m_ipv4AddressEntry.erase (itmp);
        }
      /** \todo Need to decide when to invalidate a route */
      /*          else if (i->second.GetLifeTime() > m_holddownTime)
       {
       ++i;
       itmp->second.SetFlag(INVALID);
       }*/
      else
        {
          ++i;
        }
    }
  return;
}

void
RoutingTable::Print (Ptr<OutputStreamWrapper> stream) const
{
  *stream->GetStream () << "\nUAV Routing table\n" << "Destination\t\tGateway\t\tInterface\t\tHopCount\t\tSeqNum\t\tLifeTime\t\tSettlingTime\n";
  for (std::map<Ipv4Address, RoutingTableEntry>::const_iterator i = m_ipv4AddressEntry.begin (); i
       != m_ipv4AddressEntry.end (); ++i)
    {
      i->second.Print (stream);
    }
  *stream->GetStream () << "\n";
}

bool
RoutingTable::AddIpv4Event (Ipv4Address address,
                            EventId id)
{
  std::pair<std::map<Ipv4Address, EventId>::iterator, bool> result = m_ipv4Events.insert (std::make_pair (address,id));
  return result.second;
}

bool
RoutingTable::AnyRunningEvent (Ipv4Address address)
{
  EventId event;
  std::map<Ipv4Address, EventId>::const_iterator i = m_ipv4Events.find (address);
  if (m_ipv4Events.empty ())
    {
      return false;
    }
  if (i == m_ipv4Events.end ())
    {
      return false;
    }
  event = i->second;
  if (event.IsRunning ())
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool
RoutingTable::ForceDeleteIpv4Event (Ipv4Address address)
{
  EventId event;
  std::map<Ipv4Address, EventId>::const_iterator i = m_ipv4Events.find (address);
  if (m_ipv4Events.empty () || i == m_ipv4Events.end ())
    {
      return false;
    }
  event = i->second;
  Simulator::Cancel (event);
  m_ipv4Events.erase (address);
  return true;
}

bool
RoutingTable::DeleteIpv4Event (Ipv4Address address)
{
  EventId event;
  std::map<Ipv4Address, EventId>::const_iterator i = m_ipv4Events.find (address);
  if (m_ipv4Events.empty () || i == m_ipv4Events.end ())
    {
      return false;
    }
  event = i->second;
  if (event.IsRunning ())
    {
      return false;
    }
  if (event.IsExpired ())
    {
      event.Cancel ();
      m_ipv4Events.erase (address);
      return true;
    }
  else
    {
      m_ipv4Events.erase (address);
      return true;
    }
}

EventId
RoutingTable::GetEventId (Ipv4Address address)
{
  std::map <Ipv4Address, EventId>::const_iterator i = m_ipv4Events.find (address);
  if (m_ipv4Events.empty () || i == m_ipv4Events.end ())
    {
      return EventId ();
    }
  else
    {
      return i->second;
    }
}

DistanceTable::DistanceTable ()
{
  m_nodeNum = 0;
  m_distMatrix = NULL;
  m_validTime = 150;
  m_dis = NULL;
  m_maxDis = 300;
}

void
DistanceTable::UpdateInfo(Ipv4Address ipv4, NodeInfo info)
{
  std::map<Ipv4Address, NodeInfo>::iterator i;
  i = m_nodeInfo.find(ipv4);
  if(i != m_nodeInfo.end())
  {
      double dt = (info.timeStamp - i->second.timeStamp)/1000.0;
      if(dt > 0) {
          i->second.position = info.position;
          if (i->second.velocity.GetLength() != 0 && dt < m_validTime) {
              Vector dv = info.velocity - i->second.velocity;
              i->second.acceleration = Vector(dv.x / dt, dv.y / dt, dv.z / dt);
          }
          i->second.velocity = info.velocity;
          i->second.queueLen = info.queueLen;
          i->second.energy = info.energy;
          i->second.timeStamp = info.timeStamp;
          i->second.isNeighbour = false;
      }
      else
          return;
  }
  else
      m_nodeInfo.insert(std::make_pair(ipv4, info));
  UpdateMatrix();
  UpdateNeighbour();
}

void
DistanceTable::PrintMatrix()
{
    std::cout<<m_localAdress<<" disMatrix: "<<std::endl;
    for(uint32_t i=0;i<m_nodeNum;i++){
        for(uint32_t j=0;j<m_nodeNum;j++)
            std::cout<<m_distMatrix[i*m_nodeNum+j]<<' ';
        std::cout<<std::endl;
    }
}

void
DistanceTable::UpdateMatrix()
{
  uint32_t len = m_nodeInfo.size();
  if(len > m_nodeNum){
      delete [] m_distMatrix;
      m_distMatrix = new double [len*len];
      memset(m_distMatrix, INF, len*len);
      m_nodeNum = len;
  }
  int m=0, n=0;
  double dis, dt;
  Vector t1, t2;
  for(std::map<Ipv4Address, NodeInfo>::iterator i = m_nodeInfo.begin(); i != m_nodeInfo.end(); i++)
  {
      n=m;
      for(std::map<Ipv4Address, NodeInfo>::iterator j = i; j != m_nodeInfo.end(); j++)
      {
          dt = (Simulator::Now ().GetMilliSeconds () - i->second.timeStamp) / 1000.0;
          if(dt > m_validTime)
          {
              dis = INF;
          }
          else
          {
            t1 = i->second.position + i->second.velocity*dt;// + i->second.acceleration*dt*dt*0.1;
            t2 = j->second.position + j->second.velocity*dt;// + j->second.acceleration*dt*dt*0.1;
            dis = ComputeDistance(t1, t2);
            if(dis > m_maxDis)
                dis = INF;
            else
                dis = 1;
          }
          m_distMatrix[m*m_nodeNum + n] = dis;
          m_distMatrix[n*m_nodeNum + m] = dis;
          n++;
      }
      m++;
  }
  delete [] m_dis;
  m_dis = new Dis [len];
  int begin = 0;
  for(std::map<Ipv4Address, NodeInfo>::iterator i = m_nodeInfo.begin(); i != m_nodeInfo.end(); i++)
  {
      if(m_localAdress == i->first)
          break;
      begin++;
  }
  Dijkstra(begin);
}

void
DistanceTable::UpdateNeighbour()
{
    std::map<Ipv4Address, NodeInfo>::iterator j;
    j = m_nodeInfo.find(m_localAdress);
    if(j != m_nodeInfo.end())
    {
        double dt;
        for (std::map<Ipv4Address, NodeInfo>::iterator i = m_nodeInfo.begin(); i != m_nodeInfo.end(); i++)
        {
            dt = (Simulator::Now ().GetMilliSeconds () - i->second.timeStamp) / 1000.0;
            if(dt < m_validTime)
            {
                if (i->first != m_localAdress && ComputeDistance(j->second.position + j->second.velocity * dt,
                                                                 i->second.position + i->second.velocity * dt) <
                                                 m_maxDis)
                    i->second.isNeighbour = true;
                else
                    i->second.isNeighbour = false;
            }
        }
    }
}

void
DistanceTable::Dijkstra(uint32_t begin){
    //首先初始化我们的dis数组
    uint32_t i;
    for (i=0; i<m_nodeNum; i++) {
      m_dis[i].path.push_back(begin);
      m_dis[i].value = m_distMatrix[m_nodeNum*begin + i];
    }
    //设置起点的到起点的路径为0
    m_dis[begin].value = 0;
    m_dis[begin].visit = true;

    uint32_t count = 1;
    //计算剩余的顶点的最短路径（剩余this->vexnum-1个顶点）
    while (count != m_nodeNum) {
        //temp用于保存当前dis数组中最小的那个下标
        //min记录的当前的最小值
        int temp=0;
        int min = INF;
        for (i = 0; i < m_nodeNum; i++) {
            if (!m_dis[i].visit && m_dis[i].value<min) {
                min = m_dis[i].value;
                temp = i;
            }
        }
        //cout << temp + 1 << "  "<<min << endl;
        //把temp对应的顶点加入到已经找到的最短路径的集合中
        m_dis[temp].visit = true;
        ++count;
        for (i = 0; i < m_nodeNum; i++) {
            //注意这里的条件arc[temp][i]!=INT_MAX必须加，不然会出现溢出，从而造成程序异常
            if (!m_dis[i].visit && m_distMatrix[m_nodeNum*temp + i]!=INF &&
                (m_dis[temp].value + m_distMatrix[m_nodeNum*temp + i]) < m_dis[i].value) {
                //如果新得到的边可以影响其他为访问的顶点，那就就更新它的最短路径和长度
                m_dis[i].value = m_dis[temp].value + m_distMatrix[m_nodeNum*temp + i];
                m_dis[temp].path.push_back(i);

            }
        }
    }

}

void
DistanceTable::GetListOfNodeInfo(std::map<Ipv4Address, NodeInfo> & nodeInfo)
{
    nodeInfo = m_nodeInfo;
}

void
DistanceTable::AddNetDevice(Ipv4Address addr, Ptr<NetDevice> dev)
{
    std::map<Ipv4Address, Ptr<NetDevice> >::const_iterator i = m_netDevice.find(addr);
    if(i == m_netDevice.end())
        m_netDevice.insert(std::make_pair(addr, dev));
    else
        m_netDevice[addr] = dev;
}
void
DistanceTable::Print(Ptr<OutputStreamWrapper> stream) const
{
    NodeInfo info;
    for(std::map<Ipv4Address, NodeInfo>::const_iterator i=m_nodeInfo.begin(); i!=m_nodeInfo.end(); i++)
    {
        info = i->second;
        *stream->GetStream () << i->first << ":\t" << "Position: " << info.position
                              << "\tVelocity: " << info.velocity << "\tAcceleration: " << info.acceleration
                              << "\tTimestamp: " << info.timeStamp << "\tQueue length: " << info.queueLen
                              << "\tEnergy:" << info.energy << "\tNeighbour: " << info.isNeighbour << '\n';
    }
}

bool
DistanceTable::LookupRoute(Ipv4Address addr, Ptr<Ipv4Route> & route)
{
    int32_t j = 0;
    int32_t begin, end = -1;
//    Ipv4Address *a = new Ipv4Address [m_nodeInfo.size()];
    for(std::map<Ipv4Address, NodeInfo>::const_iterator i=m_nodeInfo.begin(); i!=m_nodeInfo.end(); i++)
    {
        if(i->first == m_localAdress)
            begin = j;
        if(i->first == addr)
            end = j;
//        a[j] = i->first;
        j++;

    }
    if(begin == end || end < 0)
        return false;
    int32_t index = -1;
    for(uint32_t i=0; i<m_nodeNum ;i++)
    {
        if(m_dis[i].path.back() == (uint32_t)end)
        {
            index=i;
            break;
        }
    }
    if(index < 0)
        return false;
//    std::cout<<m_localAdress<<'>'<<addr<<" : ";
//    for(uint32_t i=0;i<m_dis[index].path.size();i++)
//        std::cout<<a[m_dis[index].path[i]]<<'\t';
//    std::cout<<std::endl;
    std::map<Ipv4Address, NodeInfo>::const_iterator i=m_nodeInfo.begin();
    for(uint32_t j=0;j<m_dis[index].path[1];j++)
        i++;
    if(i->first == m_localAdress)
        return false;
    route = Create<Ipv4Route> ();
    route->SetDestination (addr);
    std::map<Ipv4Address, Ptr<NetDevice> >::const_iterator tmp = m_netDevice.find(addr);
    route->SetOutputDevice(tmp->second);
    route->SetGateway (i->first);
//    delete [] a;
    return true;
}
}
}

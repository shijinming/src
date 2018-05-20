#ifndef TDMA_NET_DEVICE_H
#define TDMA_NET_DEVICE_H

#include "ns3/wifi-net-device.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/mac48-address.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/wifi-phy.h"
#include <string>

namespace tdma {

    class TdmaMac;

    class TdmaNetDevice : public ns3::WifiNetDevice
    {
    public:
        static ns3::TypeId GetTypeId (void);

        TdmaNetDevice ();
        virtual ~TdmaNetDevice ();

        void SetMac (ns3::Ptr<TdmaMac> mac);
        void SetPhy (ns3::Ptr<ns3::WifiPhy> phy);
        void SetRemoteStationManager (ns3::Ptr<ns3::WifiRemoteStationManager> manager);
        ns3::Ptr<TdmaMac> GetMac (void) const;
        ns3::Ptr<ns3::WifiPhy> GetPhy (void) const;
        ns3::Ptr<ns3::WifiRemoteStationManager> GetRemoteStationManager (void) const;
        // inherited from NetDevice base class.
        virtual void SetIfIndex (const uint32_t index);
        virtual uint32_t GetIfIndex (void) const;
        virtual ns3::Ptr<ns3::Channel> GetChannel (void) const;
        virtual void SetAddress (ns3::Address address);
        virtual ns3::Address GetAddress (void) const;
        virtual bool SetMtu (const uint16_t mtu);
        virtual uint16_t GetMtu (void) const;
        virtual bool IsLinkUp (void) const;
        virtual void AddLinkChangeCallback (ns3::Callback<void> callback);
        virtual bool IsBroadcast (void) const;
        virtual ns3::Address GetBroadcast (void) const;
        virtual bool IsMulticast (void) const;
        virtual ns3::Address GetMulticast (ns3::Ipv4Address multicastGroup) const;
        virtual bool IsPointToPoint (void) const;
        virtual bool IsBridge (void) const;
        virtual bool Send (ns3::Ptr<ns3::Packet> packet, const ns3::Address& dest, uint16_t protocolNumber);
        virtual ns3::Ptr<ns3::Node> GetNode (void) const;
        virtual void SetNode (ns3::Ptr<ns3::Node> node);
        virtual bool NeedsArp (void) const;
        virtual void SetReceiveCallback (ns3::NetDevice::ReceiveCallback cb);

        virtual ns3::Address GetMulticast (ns3::Ipv6Address addr) const;

        virtual bool SendFrom (ns3::Ptr<ns3::Packet> packet, const ns3::Address& source, const ns3::Address& dest, uint16_t protocolNumber);
        virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
        virtual bool SupportsSendFrom (void) const;

    private:
        // This value conforms to the 802.11 specification
        static const uint16_t MAX_MSDU_SIZE = 2304;

        virtual void DoDispose (void);
        virtual void DoInitialize (void);
        void ForwardUp (ns3::Ptr<ns3::Packet> packet, ns3::Mac48Address from, ns3::Mac48Address to);
        void LinkUp (void);
        void LinkDown (void);
        void Setup (void);
        ns3::Ptr<ns3::Channel> DoGetChannel (void) const;
        void CompleteConfig (void);

        ns3::Ptr<ns3::Node> m_node;
        ns3::Ptr<ns3::WifiPhy> m_phy;
        ns3::Ptr<TdmaMac> m_mac;
        ns3::Ptr<ns3::WifiRemoteStationManager> m_stationManager;
        ns3::NetDevice::ReceiveCallback m_forwardUp;
        ns3::NetDevice::PromiscReceiveCallback m_promiscRx;

        ns3::TracedCallback<ns3::Ptr<const ns3::Packet>, ns3::Mac48Address> m_rxLogger;
        ns3::TracedCallback<ns3::Ptr<const ns3::Packet>, ns3::Mac48Address> m_txLogger;

        bool m_configComplete;
        mutable uint16_t m_mtu;
        bool m_linkUp;
        uint32_t m_ifIndex;
        ns3::TracedCallback<> m_linkChanges;
    };

} // namespace tdma

#endif /* TDMA_NET_DEVICE_H */

#include "tdma-net-device.h"
#include "tdma-mac.h"
#include "ns3/wifi-phy.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/wifi-remote-station-manager.h"
#include "ns3/llc-snap-header.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/log.h"

namespace tdma {
    NS_LOG_COMPONENT_DEFINE ("TdmaNetDevice");
    NS_OBJECT_ENSURE_REGISTERED (TdmaNetDevice);

    ns3::TypeId TdmaNetDevice::GetTypeId (void) {
        static ns3::TypeId tid = ns3::TypeId ("tdma::TdmaNetDevice")
                .SetParent<ns3::NetDevice> ()
                .AddConstructor<TdmaNetDevice> ()
                .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                               ns3::UintegerValue (MAX_MSDU_SIZE - ns3::LLC_SNAP_HEADER_LENGTH),
                               ns3::MakeUintegerAccessor (&TdmaNetDevice::SetMtu,
                                                          &TdmaNetDevice::GetMtu),
                               ns3::MakeUintegerChecker<uint16_t> (1, MAX_MSDU_SIZE - ns3::LLC_SNAP_HEADER_LENGTH))
                .AddAttribute ("Channel", "The channel attached to this device",
                               ns3::PointerValue (),
                               ns3::MakePointerAccessor (&TdmaNetDevice::DoGetChannel),
                               ns3::MakePointerChecker<ns3::YansWifiChannel> ())
                .AddAttribute ("Phy", "The PHY layer attached to this device.",
                               ns3::PointerValue (),
                               ns3::MakePointerAccessor (&TdmaNetDevice::GetPhy,
                                                         &TdmaNetDevice::SetPhy),
                               ns3::MakePointerChecker<ns3::WifiPhy> ())
                .AddAttribute ("Mac", "The MAC layer attached to this device.",
                               ns3::PointerValue (),
                               ns3::MakePointerAccessor (&TdmaNetDevice::GetMac,
                                                         &TdmaNetDevice::SetMac),
                               ns3::MakePointerChecker<TdmaMac> ());
        return tid;
    }
    TdmaNetDevice::TdmaNetDevice ()
            : m_configComplete (false),
              m_mtu(MAX_MSDU_SIZE - ns3::LLC_SNAP_HEADER_LENGTH),
              m_linkUp(false),
              m_ifIndex(0) {
        NS_LOG_FUNCTION_NOARGS ();
    }
    TdmaNetDevice::~TdmaNetDevice () {
        NS_LOG_FUNCTION_NOARGS ();
    }
    void TdmaNetDevice::DoDispose (void) {
        NS_LOG_FUNCTION_NOARGS ();
        m_node = 0;
        m_mac->Dispose ();
        m_phy->Dispose ();
        m_mac = 0;
        m_phy = 0;
        // chain up.
        NetDevice::DoDispose ();
    }
    void TdmaNetDevice::DoInitialize (void) {
        m_phy->Initialize ();
        m_mac->Initialize ();
        NetDevice::DoInitialize ();
    }
    void TdmaNetDevice::CompleteConfig (void) {
        if (m_mac == 0
            || m_phy == 0
            || m_node == 0
            || m_configComplete) {
            NS_LOG_WARN("Complete config called with some of the entities not set");
            return;
        }
        m_mac->SetWifiPhy (m_phy);
        m_mac->SetForwardUpCallback (MakeCallback (&TdmaNetDevice::ForwardUp, this));
        m_mac->SetLinkUpCallback (MakeCallback (&TdmaNetDevice::LinkUp, this));
        m_mac->SetLinkDownCallback (MakeCallback (&TdmaNetDevice::LinkDown, this));
        m_configComplete = true;
        NS_LOG_DEBUG("Complete config called and all entities were set");
    }
    void TdmaNetDevice::SetMac (ns3::Ptr<TdmaMac> mac) {
        m_mac = mac;
        CompleteConfig ();
    }
    void TdmaNetDevice::SetPhy (ns3::Ptr<ns3::WifiPhy> phy) {
        m_phy = phy;
        CompleteConfig ();
    }
    ns3::Ptr<TdmaMac> TdmaNetDevice::GetMac (void) const {
        return m_mac;
    }
    ns3::Ptr<ns3::WifiPhy> TdmaNetDevice::GetPhy (void) const {
        return m_phy;
    }
    void TdmaNetDevice::SetIfIndex (const uint32_t index) {
        m_ifIndex = index;
    }
    uint32_t TdmaNetDevice::GetIfIndex (void) const {
        return m_ifIndex;
    }
    ns3::Ptr<ns3::Channel> TdmaNetDevice::GetChannel (void) const {
        return m_phy->GetChannel ();
    }
    ns3::Ptr<ns3::Channel> TdmaNetDevice::DoGetChannel (void) const {
        return m_phy->GetChannel ();
    }
    void TdmaNetDevice::SetAddress (ns3::Address address) {
        m_mac->SetAddress (ns3::Mac48Address::ConvertFrom (address));
    }
    ns3::Address TdmaNetDevice::GetAddress (void) const {
        return m_mac->GetAddress ();
    }
    bool TdmaNetDevice::SetMtu (const uint16_t mtu) {
        if (mtu > MAX_MSDU_SIZE - ns3::LLC_SNAP_HEADER_LENGTH) {
            return false;
        }
        m_mtu = mtu;
        return true;
    }
    uint16_t TdmaNetDevice::GetMtu (void) const {
        return m_mtu;
    }
    bool TdmaNetDevice::IsLinkUp (void) const {
        return m_phy != 0 && m_linkUp;
    }
    void TdmaNetDevice::AddLinkChangeCallback (ns3::Callback<void> callback) {
        m_linkChanges.ConnectWithoutContext (callback);
    }
    bool TdmaNetDevice::IsBroadcast (void) const {
        return true;
    }
    ns3::Address TdmaNetDevice::GetBroadcast (void) const {
        return ns3::Mac48Address::GetBroadcast ();
    }
    bool TdmaNetDevice::IsMulticast (void) const {
        return true;
    }
    ns3::Address TdmaNetDevice::GetMulticast (ns3::Ipv4Address multicastGroup) const {
        return ns3::Mac48Address::GetMulticast (multicastGroup);
    }
    ns3::Address TdmaNetDevice::GetMulticast (ns3::Ipv6Address addr) const {
        return ns3::Mac48Address::GetMulticast (addr);
    }
    bool TdmaNetDevice::IsPointToPoint (void) const {
        return false;
    }
    bool TdmaNetDevice::IsBridge (void) const {
        return false;
    }
    bool TdmaNetDevice::Send (ns3::Ptr<ns3::Packet> packet, const ns3::Address& dest, uint16_t protocolNumber) {
        NS_ASSERT (ns3::Mac48Address::IsMatchingType (dest));
        ns3::Mac48Address realTo = ns3::Mac48Address::ConvertFrom (dest);
        ns3::LlcSnapHeader llc;
        llc.SetType (protocolNumber);
        packet->AddHeader (llc);
        m_mac->Enqueue (packet, realTo);
        return true;
    }
    ns3::Ptr<ns3::Node> TdmaNetDevice::GetNode (void) const {
        return m_node;
    }
    void TdmaNetDevice::SetNode (ns3::Ptr<ns3::Node> node) {
        m_node = node;
        CompleteConfig ();
    }
    bool TdmaNetDevice::NeedsArp (void) const {
        return true;
    }
    void TdmaNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb) {
        m_forwardUp = cb;
    }
    void TdmaNetDevice::ForwardUp (ns3::Ptr<ns3::Packet> packet, ns3::Mac48Address from, ns3::Mac48Address to) {
        ns3::LlcSnapHeader llc;
        packet->RemoveHeader (llc);
        enum NetDevice::PacketType type;
        if (to.IsBroadcast ()) {
            type = NetDevice::PACKET_BROADCAST;
        }
        else if (to.IsGroup ()) {
            type = NetDevice::PACKET_MULTICAST;
        }
        else if (to == m_mac->GetAddress ()) {
            type = NetDevice::PACKET_HOST;
        }
        else {
            type = NetDevice::PACKET_OTHERHOST;
        }
        if (type != NetDevice::PACKET_OTHERHOST) {
            m_forwardUp (this, packet, llc.GetType (), from);
        }
        if (!m_promiscRx.IsNull ()) {
            m_promiscRx (this, packet, llc.GetType (), from, to, type);
        }
    }
    void TdmaNetDevice::LinkUp (void) {
        m_linkUp = true;
        m_linkChanges ();
    }
    void TdmaNetDevice::LinkDown (void) {
        m_linkUp = false;
        m_linkChanges ();
    }
    bool TdmaNetDevice::SendFrom (ns3::Ptr<ns3::Packet> packet, const ns3::Address& source,
                             const ns3::Address& dest, uint16_t protocolNumber) {
        NS_ASSERT (ns3::Mac48Address::IsMatchingType (dest));
        NS_ASSERT (ns3::Mac48Address::IsMatchingType (source));
        ns3::Mac48Address realTo = ns3::Mac48Address::ConvertFrom (dest);
        ns3::Mac48Address realFrom = ns3::Mac48Address::ConvertFrom (source);
        ns3::LlcSnapHeader llc;
        llc.SetType (protocolNumber);
        packet->AddHeader (llc);
        m_mac->Enqueue (packet, realTo, realFrom);
        return true;
    }
    void TdmaNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb) {
        m_promiscRx = cb;
    }
    bool TdmaNetDevice::SupportsSendFrom (void) const {
        return m_mac->SupportsSendFrom ();
    }

} // namespace stdma

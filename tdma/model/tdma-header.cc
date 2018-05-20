#include "tdma-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("TdmaHeader");

namespace tdma {

    ns3::TypeId TdmaHeader::GetTypeId (void)
    {
        static ns3::TypeId tid = ns3::TypeId("tdma::TdmaHeader")
                .SetParent<ns3::Header>()
                .AddConstructor<TdmaHeader>();
        return tid;
    }
    TdmaHeader::TdmaHeader ()
            : m_latitude(0),
              m_longitude(0),
              m_offset(0),
              m_timeout(0),
              m_entry(0) {}

    TdmaHeader::~TdmaHeader () {}
    ns3::TypeId TdmaHeader::GetInstanceTypeId (void) const {
        return GetTypeId();
    }
    uint32_t TdmaHeader::GetSerializedSize (void) const {
        return sizeof(m_latitude) + sizeof(m_longitude) + sizeof(m_offset) + sizeof(m_timeout) + sizeof(m_entry);
    }
    void TdmaHeader::Serialize (ns3::Buffer::Iterator start) const {
        ns3::Buffer::Iterator i = start;
        i.WriteU32(m_latitude);
        i.WriteU32(m_longitude);
        i.WriteU16(m_offset);
        i.WriteU8(m_timeout);
        i.WriteU8(m_entry);
    }
    uint32_t TdmaHeader::Deserialize (ns3::Buffer::Iterator start) {
        m_latitude = start.ReadU32();
        m_longitude = start.ReadU32();
        m_offset = start.ReadU16();
        m_timeout = start.ReadU8();
        m_entry = start.ReadU8();
        return sizeof(m_latitude) + sizeof(m_longitude) + sizeof(m_offset) + sizeof(m_timeout) + sizeof(m_entry);
    }
    void TdmaHeader::Print (std::ostream &os) const {
        std::string type;
        os << "TDMA Header: (Lat: " << m_latitude
           << ", Lon: " << m_longitude
           << ", Offset: " << m_offset
           << ", Timeout: " << m_timeout << ")";
    }
    void TdmaHeader::SetLatitude(double lat) {
        m_latitude = lat;
    }
    double TdmaHeader::GetLatitude() {
        return m_latitude;
    }
    void TdmaHeader::SetLongitude(double lon) {
        m_longitude = lon;
    }
    double TdmaHeader::GetLongitude() {
        return m_longitude;
    }
    void TdmaHeader::SetOffset(uint16_t offset) {
        m_offset = offset;
    }
    uint16_t TdmaHeader::GetOffset() {
        return m_offset;
    }
    void TdmaHeader::SetTimeout(uint8_t timeout) {
        m_timeout = timeout;
    }
    uint8_t TdmaHeader::GetTimeout() {
        return m_timeout;
    }
    void TdmaHeader::SetNetworkEntry() {
        m_entry = 1;
    }
    bool TdmaHeader::GetNetworkEntry() {
        return (m_entry > 0);
    }

} // namespace tdma

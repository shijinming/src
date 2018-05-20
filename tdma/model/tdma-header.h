#ifndef TDMA_HEADER_H_
#define TDMA_HEADER_H_

#include "ns3/header.h"
#include "ns3/buffer.h"

namespace tdma {

    class TdmaHeader : public ns3::Header
    {

    public:

        static ns3::TypeId GetTypeId (void);
        TdmaHeader ();
        virtual ~TdmaHeader ();
        virtual ns3::TypeId GetInstanceTypeId (void) const;
        virtual uint32_t GetSerializedSize (void) const;
        virtual void Serialize (ns3::Buffer::Iterator start) const;
        virtual uint32_t Deserialize (ns3::Buffer::Iterator start);
        virtual void Print (std::ostream &os) const;

        void SetLatitude(double lat);
        double GetLatitude();

        void SetLongitude(double lon);
        double GetLongitude();

        void SetOffset(uint16_t offset);
        uint16_t GetOffset();

        void SetTimeout(uint8_t timeout);
        uint8_t GetTimeout();

        void SetNetworkEntry();
        bool GetNetworkEntry();

    private:

        double m_latitude;
        double m_longitude;

        uint16_t m_offset;
        uint8_t m_timeout;  // Describes the slot timeout and defines the number of future frames

        uint8_t m_entry;    // Used as a flag to indicate that this is a network entry packet

    };

} // namespace stdma

#endif /* STDMA_HEADER_H_ */

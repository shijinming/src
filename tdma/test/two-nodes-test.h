#ifndef TWO_NODES_TEST_H_
#define TWO_NODES_TEST_H_

#include "ns3/log.h"
#include "ns3/test.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"

namespace tdma {

    class TdmaTwoNodesTest : public ns3::TestCase
    {
    public:
        TdmaTwoNodesTest ();

        virtual void DoRun (void);
        void TdmaTxTrace (std::string context, ns3::Ptr<const ns3::Packet> p, uint32_t no, uint8_t timeout, uint32_t offset);
        void TdmaRxTrace (std::string context, ns3::Ptr<const ns3::Packet> p, uint8_t timeout, uint32_t offset);
        void TdmaStartupTrace (std::string context, ns3::Time when, ns3::Time frameDuration, ns3::Time slotDuration);
        void TdmaNetworkEntry (std::string context, ns3::Ptr<const ns3::Packet> p, ns3::Time offset, bool isTaken);

    private:
        uint32_t GetProtocolOverheads();
        uint32_t GetGlobalSlotId(ns3::Time t);

        uint32_t m_count;
        uint32_t m_nextTxNodeOne;
        uint32_t m_nextTxNodeTwo;
        uint32_t m_nextRxFromNodeOne;
        uint32_t m_nextRxFromNodeTwo;
        ns3::Time m_slotDuration;

    };

} // namespace tdma

#endif /* TWO_NODES_TEST_H_ */

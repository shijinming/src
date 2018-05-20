#ifndef SINGLE_NODE_TEST_H_
#define SINGLE_NODE_TEST_H_

#include "ns3/log.h"
#include "ns3/test.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"

namespace tdma {

    class TdmaSingleNodeTest : public ns3::TestCase
    {
    public:
        TdmaSingleNodeTest ();

        virtual void DoRun (void);
        void TdmaTxTrace (std::string context, ns3::Ptr<const ns3::Packet> p, uint32_t no, uint8_t timeout, uint32_t offset);
        void TdmaStartupTrace (std::string context, ns3::Time when, ns3::Time frameDuration, ns3::Time slotDuration);
        void TdmaNetworkEntry (std::string context, ns3::Ptr<const ns3::Packet> p, ns3::Time offset, bool isTaken);

    private:
        uint32_t GetProtocolOverheads();

        uint32_t m_count;
        ns3::Time m_nextTx;
        ns3::Time m_slotDuration;

    };

} // namespace tdma

#endif /* SINGLE_NODE_TEST_H_ */

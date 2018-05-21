#ifndef SLOT_MANAGER_TEST_H_
#define SLOT_MANAGER_TEST_H_

#include "ns3/log.h"
#include "ns3/test.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"

namespace tdma {

    class TdmaSlotManagerTest : public ns3::TestCase
    {
    public:
        TdmaSlotManagerTest ();

        virtual void DoRun (void);

    private:

    };

} // namespace tdma

#endif /* SLOT_MANAGER_TEST_H_ */
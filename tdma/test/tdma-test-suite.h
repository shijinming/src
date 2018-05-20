#ifndef STDMA_TEST_SUITE_H_
#define STDMA_TEST_SUITE_H_

#include "ns3/log.h"
#include "ns3/test.h"

using namespace ns3;

namespace tdma {

    class TdmaSingleNodeTestSuite : public TestSuite
    {
    public:
        TdmaSingleNodeTestSuite ();
    };

}

#endif /* TDMA_TEST_SUITE_H_ */
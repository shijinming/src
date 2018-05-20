#include "tdma-mac-helper.h"
#include "ns3/tdma-mac.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/object-base.h"

namespace tdma {

    TdmaMacHelper::TdmaMacHelper () {}
    TdmaMacHelper::~TdmaMacHelper () {}

    TdmaMacHelper TdmaMacHelper::Default (void) {
        TdmaMacHelper helper;
        helper.SetType("tdma::TdmaMac");
        return helper;
    }
    void TdmaMacHelper::SetType (std::string type,
                             std::string n0, const ns3::AttributeValue &v0,
                             std::string n1, const ns3::AttributeValue &v1,
                             std::string n2, const ns3::AttributeValue &v2,
                             std::string n3, const ns3::AttributeValue &v3,
                             std::string n4, const ns3::AttributeValue &v4,
                             std::string n5, const ns3::AttributeValue &v5,
                             std::string n6, const ns3::AttributeValue &v6,
                             std::string n7, const ns3::AttributeValue &v7) {
        m_mac.SetTypeId (type);
        m_mac.Set (n0, v0);
        m_mac.Set (n1, v1);
        m_mac.Set (n2, v2);
        m_mac.Set (n3, v3);
        m_mac.Set (n4, v4);
        m_mac.Set (n5, v5);
        m_mac.Set (n6, v6);
        m_mac.Set (n7, v7);
    }
    ns3::Ptr<TdmaMac> TdmaMacHelper::Create (void) const {
        ns3::Ptr<TdmaMac> mac = m_mac.Create<TdmaMac> ();
        return mac;
    }

} //namespace tdma

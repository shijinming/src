#ifndef TDMA_MAC_HELPER_H
#define TDMA_MAC_HELPER_H

#include "ns3/object-factory.h"

namespace tdma{

class TdmaMac;

class TdmaMacHelper{

public:
    TdmaMacHelper();
    virtual ~TdmaMacHelper();
    static TdmaMacHelper Default(void);
    void SetType(std::string type, std::string n0="", const ns3::AttributeValue &v0=
     ns3::EmptyAttributeValue(), std::string n1 = "", const ns3::AttributeValue &v1 =
     ns3::EmptyAttributeValue(), std::string n2 = "", const ns3::AttributeValue &v2 =
     ns3::EmptyAttributeValue(), std::string n3 = "", const ns3::AttributeValue &v3 =
     ns3::EmptyAttributeValue(), std::string n4 = "", const ns3::AttributeValue &v4 =
     ns3::EmptyAttributeValue(), std::string n5 = "", const ns3::AttributeValue &v5 =
     ns3::EmptyAttributeValue(), std::string n6 = "", const ns3::AttributeValue &v6 =
     ns3::EmptyAttributeValue(), std::string n7 = "", const ns3::AttributeValue &v7 =
     ns3::EmptyAttributeValue());
    virtual ns3::Ptr<TdmaMac> Create(void) const;

private:
    ns3::ObjectFactory m_mac;
};

}

#endif
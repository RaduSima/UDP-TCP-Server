#ifndef TEMA2_UDP_MESSAGEFLOAT_H
#define TEMA2_UDP_MESSAGEFLOAT_H

#include "UDP_MessageBase.h"

class UDP_MessageFLOAT : public UDP_MessageBase{
private:
    double value;
public:
    UDP_MessageFLOAT(const string &topic, const string &typeName,
                     unsigned int type, double value,
                     sockaddr_in udp_client_address);

    double getValue() const;

    void setValue(double value);
};


#endif

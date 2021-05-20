#ifndef TEMA2_UDP_MESSAGEINT_H
#define TEMA2_UDP_MESSAGEINT_H

#include "UDP_MessageBase.h"

class UDP_MessageINT : public UDP_MessageBase {
private:
    int value;
public:
    UDP_MessageINT(const string &topic, const string &typeName,
                   unsigned int type, int value,
                   sockaddr_in udp_client_address);

    int getValue() const;

    void setValue(int value);
};


#endif

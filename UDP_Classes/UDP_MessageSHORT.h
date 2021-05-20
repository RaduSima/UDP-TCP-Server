#ifndef TEMA2_UDP_MESSAGESHORT_H
#define TEMA2_UDP_MESSAGESHORT_H

#include "UDP_MessageBase.h"

class UDP_MessageSHORT : public UDP_MessageBase{
private:
    float value;
public:
    UDP_MessageSHORT(const string &topic, const string &typeName,
                     unsigned int type, float value,
                     sockaddr_in udp_client_address);

    float getValue() const;

    void setValue(float value);
};


#endif

#ifndef TEMA2_UDP_MESSAGESTRING_H
#define TEMA2_UDP_MESSAGESTRING_H

#include <string>

#include "UDP_MessageBase.h"

using namespace std;

class UDP_MessageSTRING : public UDP_MessageBase{
private:
    string value;
public:
    UDP_MessageSTRING(const string &topic, const string &typeName,
                      unsigned int type, const string &value,
                      sockaddr_in udp_client_address);

    const string &getValue() const;

    void setValue(const string &value);
};


#endif

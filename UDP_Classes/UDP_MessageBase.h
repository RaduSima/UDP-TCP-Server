#ifndef TEMA2_UDP_MESSAGEBASE_H
#define TEMA2_UDP_MESSAGEBASE_H

#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>

#include <netinet/in.h>

using namespace std;

class UDP_MessageBase {
protected:
    string topic;
    string type_name;
    unsigned int type;
    struct sockaddr_in udp_client_address;
public:
    UDP_MessageBase(const string &topic, const string &typeName,
                    unsigned int type, struct sockaddr_in udp_client_address);

    const string &getTopic() const;

    void setTopic(const string &topic);

    unsigned int getType() const;

    const string &getTypeName() const;

    void setTypeName(const string &typeName);

    void setType(unsigned int type);

    const sockaddr_in &getUdpClientAddress() const;

    void setUdpClientAddress(const sockaddr_in &udpClientAddress);
};


#endif

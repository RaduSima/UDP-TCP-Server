#include "UDP_MessageSHORT.h"

float UDP_MessageSHORT::getValue() const {
    return value;
}

void UDP_MessageSHORT::setValue(float _value) {
    UDP_MessageSHORT::value = _value;
}

UDP_MessageSHORT::UDP_MessageSHORT(const string &topic, const string &typeName,
                                   unsigned int type, float value,
                                   struct sockaddr_in udp_client_address)
        : UDP_MessageBase(topic, typeName, type, udp_client_address), value(value) {
}

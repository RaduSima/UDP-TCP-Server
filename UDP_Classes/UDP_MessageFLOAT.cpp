#include "UDP_MessageFLOAT.h"

double UDP_MessageFLOAT::getValue() const {
    return value;
}

void UDP_MessageFLOAT::setValue(double _value) {
    UDP_MessageFLOAT::value = _value;
}

UDP_MessageFLOAT::UDP_MessageFLOAT(const string &topic, const string &typeName,
                                   unsigned int type, double value,
                                   struct sockaddr_in udp_client_address)
        : UDP_MessageBase(topic, typeName, type, udp_client_address), value(value) {}

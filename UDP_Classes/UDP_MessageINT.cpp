#include "UDP_MessageINT.h"

int UDP_MessageINT::getValue() const {
    return value;
}

void UDP_MessageINT::setValue(int _value) {
    UDP_MessageINT::value = _value;
}

UDP_MessageINT::UDP_MessageINT(const string &topic, const string &typeName,
                               unsigned int type, int value,
                               struct sockaddr_in udp_client_address) :
                               UDP_MessageBase(
        topic, typeName, type, udp_client_address), value(value) {}

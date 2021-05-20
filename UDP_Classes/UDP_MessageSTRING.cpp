#include "UDP_MessageSTRING.h"

const string &UDP_MessageSTRING::getValue() const {
    return value;
}

void UDP_MessageSTRING::setValue(const string &_value) {
    UDP_MessageSTRING::value = _value;
}

UDP_MessageSTRING::UDP_MessageSTRING(const string &topic,
                                     const string &typeName, unsigned int type,
                                     const string &value,
                                     struct sockaddr_in udp_client_address) :
                                     UDP_MessageBase(
        topic, typeName, type, udp_client_address), value(value) {
}

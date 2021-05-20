#include "UDP_MessageBase.h"

UDP_MessageBase::UDP_MessageBase(const string &topic, const string &typeName,
                                 unsigned int type,
                                 struct sockaddr_in udp_client_address) : topic(topic),
                                                      type_name(typeName),
                                                      type(type), udp_client_address(udp_client_address) {}

const string &UDP_MessageBase::getTopic() const {
    return topic;
}

void UDP_MessageBase::setTopic(const string &_topic) {
    UDP_MessageBase::topic = _topic;
}

unsigned int UDP_MessageBase::getType() const {
    return type;
}

void UDP_MessageBase::setType(unsigned int _type) {
    UDP_MessageBase::type = _type;
}

const string &UDP_MessageBase::getTypeName() const {
    return type_name;
}

void UDP_MessageBase::setTypeName(const string &typeName) {
    type_name = typeName;
}

const sockaddr_in &UDP_MessageBase::getUdpClientAddress() const {
    return udp_client_address;
}

void UDP_MessageBase::setUdpClientAddress(const sockaddr_in &udpClientAddress) {
    udp_client_address = udpClientAddress;
}

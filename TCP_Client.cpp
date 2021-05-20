#include "TCP_Client.h"

TCP_Client::TCP_Client(const sockaddr_in &clientAddress, int tcpSocketId,
                       const string &clientId) : client_address(clientAddress),
                                         tcp_socket_id(tcpSocketId),
                                         client_id(clientId) {}

const sockaddr_in &TCP_Client::getClientAddress() const {
    return client_address;
}

void TCP_Client::setClientAddress(const sockaddr_in &clientAddress) {
    client_address = clientAddress;
}

const string &TCP_Client::getClientId() const {
    return client_id;
}

void TCP_Client::setClientId(const string &clientId) {
    client_id = clientId;
}

int TCP_Client::getTcpSocketId() const {
    return tcp_socket_id;
}

void TCP_Client::setTcpSocketId(int tcpSocketId) {
    tcp_socket_id = tcpSocketId;
}

bool TCP_Client::isConnected() const {
    return connected;
}

void TCP_Client::setConnected(bool connected) {
    TCP_Client::connected = connected;
}

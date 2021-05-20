#ifndef TEMA2_TCP_CLIENT_H
#define TEMA2_TCP_CLIENT_H

#include <string>
#include <vector>

#include <netinet/in.h>

using namespace std;

class TCP_Client {
private:
    struct sockaddr_in client_address;
    int tcp_socket_id;
    string client_id;
    bool connected = true;
public:

    TCP_Client(const sockaddr_in &clientAddress, int tcpSocketId,
               const string &clientId);

    const sockaddr_in &getClientAddress() const;

    void setClientAddress(const sockaddr_in &clientAddress);

    const string &getClientId() const;

    void setClientId(const string &clientId);

    int getTcpSocketId() const;

    void setTcpSocketId(int tcpSocketId);

    bool isConnected() const;

    void setConnected(bool connected);
};


#endif

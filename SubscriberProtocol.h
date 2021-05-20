#ifndef TEMA2_SUBSCRIBERPROTOCOL_H
#define TEMA2_SUBSCRIBERPROTOCOL_H

#include <cstdio>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <regex>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "utils.h"

using namespace std;

class SubscriberProtocol {
private:
    bool running_flag = false;
    string client_id;
    long server_port;

    struct sockaddr_in serv_addr{};
    int tcp_socket_id{};
    fd_set read_fds{};
    int fdmax{};
public:
    SubscriberProtocol(const string &clientId, long serverPort);

    virtual ~SubscriberProtocol();

    void init(struct in_addr server_ip);

    void run();
private:
    void handleDataOnSTDIN();
    void handleDataFromServer();

    char* receiveServerData() const;
};


#endif

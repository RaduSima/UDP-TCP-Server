#ifndef TEMA2_SERVERPROTOCOL_H
#define TEMA2_SERVERPROTOCOL_H

#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <cmath>
#include <unordered_map>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "UDP_Classes/UDP_Classes.h"
#include "TCP_Client.h"
#include "Subscription.h"
#include "utils.h"

class ServerProtocol {
private:
    long server_port;
    vector<TCP_Client *> clients;
    unordered_map<string, vector<Subscription *>> subscriptions_map;
    bool running_flag = false;

    int udp_socket_id{};
    int tcp_socket_id{};
    struct sockaddr_in server_address{}, client_address{}, udp_address{};
    unsigned int udp_address_size{};

    fd_set read_fds{};
    int fdmax{};
public:
    explicit ServerProtocol(long port);

    virtual ~ServerProtocol();

    void init();
    void run();

private:
    // data
    void handleDataOnSTDIN();
    void handleDataOnUDP();
    void handleNewTCPConnection();
    void handleDataOnTCPClient(int current_fd);

    // messages
    static UDP_MessageBase *
    parseUDPMessage(char *buf, struct sockaddr_in udp_client_address);
    static void sendMessage(UDP_MessageBase* message, int
    tcp_client_socket_id);
    void sendStoredMessages(TCP_Client* client);

    // client utility
    TCP_Client* findClient(int socket_id);
    void disconnectClient(int client_fd);
    void addNewClient(int new_tcp_socket, char* new_client_id);
    void reconnectClient(TCP_Client* client, int new_tcp_socket);
    void subscribeClient(vector<string> client_request_params, int client_fd);
    void unsubscribeClient(vector<string> client_request_params, int client_fd);
};

#endif

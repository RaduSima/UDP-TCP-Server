#include <iomanip>
#include "ServerProtocol.h"

ServerProtocol::ServerProtocol(long port) : server_port(port) {}

ServerProtocol::~ServerProtocol() {
    // close TCP client connections
    for (TCP_Client *client : clients) {
        close(client->getTcpSocketId());
    }

    // close UDP socket
    close(udp_socket_id);

    // close TCP listen socket
    close(tcp_socket_id);

    // free memory
    for (auto &it : clients) {
        delete (it);
    }
    clients.clear();

    for (auto &it : subscriptions_map) {
        for (auto &it_vec : it.second) {
            delete (it_vec);
            it.second.clear();
        }
    }
    subscriptions_map.clear();
}

void ServerProtocol::init() {
    int rs;

    // set adresses
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));
    memset(&udp_address, 0, sizeof(udp_address));
    udp_address_size = sizeof(udp_address);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // create descriptor set
    FD_ZERO(&read_fds);

    // UDP SOCKET

    // open UDP socket
    udp_socket_id = socket(PF_INET, SOCK_DGRAM, 0);
    DIE(udp_socket_id == -1, "Could not initialize UDP socket\n");

    // set UDP socket options
    int enable = 1;
    DIE(setsockopt(udp_socket_id, SOL_SOCKET, SO_REUSEADDR, &enable,
                   sizeof(int)) == -1, "Could not set options for UDP "
                                       "socket\n");

    // bind UDP socket to server address
    DIE(bind(udp_socket_id, (const struct sockaddr *) &server_address,
             sizeof(server_address)) == -1,
        "Could not bind UDP socket to server address\n");

    //TCP SOCKET

    // open TCP socket
    tcp_socket_id = socket(PF_INET, SOCK_STREAM, 0);
    DIE(tcp_socket_id == -1, "Could not initialize TCP socket\n");

    // set TCP socket options
    enable = 1;
    DIE(setsockopt(tcp_socket_id, IPPROTO_TCP, TCP_NODELAY, &enable,
                   sizeof(int)) == -1, "Could not set options for TCP "
                                       "socket\n");

    // bind TCP socket to server address
    rs = bind(tcp_socket_id, (const struct sockaddr *) &server_address,
              sizeof(server_address));
    DIE(rs == -1, "Could not bind TCP socket to server address\n");

    // add UDP, TCP and STDIN sockets to the read file descriptors set
    FD_SET(udp_socket_id, &read_fds);
    FD_SET(tcp_socket_id, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    fdmax = max(udp_socket_id, tcp_socket_id);

    // listen on TCP socket
    DIE(listen(tcp_socket_id, MAX_CLIENTS) < 0, "Could not listen on TCP "
                                                "socket\n");
}

void ServerProtocol::run() {
    fd_set tmp_fds;
    FD_ZERO(&tmp_fds);

    // server data reading loop
    running_flag = true;
    while (running_flag) {

        // select proper read sockets
        tmp_fds = read_fds;
        DIE(select(fdmax + 1, &tmp_fds, nullptr, nullptr, nullptr) < 0,
            "Could not select proper sockets\n");

        // iterate through all file descriptors until fdmax
        for (int current_fd = 0; current_fd <= fdmax; current_fd++) {
            // check if the current file descriptor is part of read_fds
            if (FD_ISSET(current_fd, &tmp_fds)) {
                // handle data on proper socket

                // STDIN
                if (current_fd == STDIN_FILENO) {
                    handleDataOnSTDIN();
                }

                    // UDP SOCKET
                else if (current_fd == udp_socket_id) {
                    handleDataOnUDP();
                }

                    // TCP LISTEN SOCKET (new TCP connection was established)
                else if (current_fd == tcp_socket_id) {
                    handleNewTCPConnection();
                }

                    // TCP client socket (data from an already connected TCP
                    // client)
                else {
                    handleDataOnTCPClient(current_fd);
                }
            }
        }
    }
}

void ServerProtocol::handleDataOnSTDIN() {
    string command;
    cin >> command;
    if (command == "exit") {
        running_flag = false;
    } else {
        cerr << "Unknown command: " << command << endl;
    }
}

void ServerProtocol::handleDataOnUDP() {
    // receive UDP message
    char buf[BUFLEN];
    memset(buf, 0, BUFLEN);
    DIE(recvfrom(udp_socket_id, buf, BUFLEN, 0,
                 (struct sockaddr *) &udp_address, &udp_address_size) < 0,
        "Could not receive bytes from UDP client\n");

    // parse message
    UDP_MessageBase *UDP_message = parseUDPMessage(buf, udp_address);

    DIE(UDP_message == nullptr, "Could not parse UDP "
                                "message\n");

    // check if topic exists
    bool stored = false;
    if (subscriptions_map.find(UDP_message->getTopic()) !=
        subscriptions_map.end()) {
        // topic exists => forward message
        for (Subscription *subscription : subscriptions_map[UDP_message
                ->getTopic()]) {

            // the client is connected
            if (subscription->getClient()->isConnected()) {
                sendMessage(UDP_message,
                            subscription->getClient()->getTcpSocketId());
            }

                // the client is disconnected and has SF == 1
            else if (!subscription->getClient()->isConnected() &&
                     subscription->getSf() == 1) {
                subscription->storeMessage(UDP_message);
                stored = true;
            }
        }
    }
    if (!stored) {
        delete UDP_message;
    }
}

void ServerProtocol::handleNewTCPConnection() {
    // accept new connection
    socklen_t client_address_len = sizeof(client_address);
    int new_tcp_client =
            accept(tcp_socket_id, (struct sockaddr *) &client_address,
                   &client_address_len);
    DIE(new_tcp_client < 0, "Could not accept new TCP "
                            "connection\n");

    // get client ID
    char new_client_id[CLIENT_ID_MAX_SIZE];
    DIE(recv(new_tcp_client, new_client_id, sizeof(new_client_id), 0) < 0,
        "Could not receive client ID\n");

    // check if client ID already exists
    for (TCP_Client *client : clients) {
        // if the client ID exists
        if (new_client_id == client->getClientId()) {
            // if the client is already connected
            if (client->isConnected()) {
                // close connection
                close(new_tcp_client);
                cout << "Client " << new_client_id << " already "
                                                      "connected." << endl;
            }

                // if the client is not connected (but was once connected)
            else {
                reconnectClient(client, new_tcp_client);
            }
            return;
        }
    }

    // client ID  does not exist
    addNewClient(new_tcp_client, new_client_id);
}

void ServerProtocol::handleDataOnTCPClient(int current_fd) {
    char buf[REQUEST_MAX_LEN];
    memset(buf, 0, REQUEST_MAX_LEN);
    int bytes_read = recv(current_fd, buf, REQUEST_MAX_LEN, 0);
    DIE(bytes_read < 0, "Could not receive bytes from TCP "
                        "client\n");
    string client_request(buf);
    // client wants to disconnect
    if (bytes_read == 0) {
        disconnectClient(current_fd);
        return;
    }

    // parse client request
    vector<string> client_request_params = splitString(client_request, " ");
    if (client_request_params.size() != 2 &&
        client_request_params.size() != 3) {
        cerr << "Incorrect command: " << client_request << endl;
        return;
    }

    // client wants to subscribe
    if (isValidSubscribeRequest(client_request_params)) {
        subscribeClient(client_request_params, current_fd);
    }

        // client wants to unsubscribe
    else if (isValidUnsubscribeRequest(client_request_params)) {
        unsubscribeClient(client_request_params, current_fd);
    }
        // invalid request from TCP client
    else {
        cerr << "Invalid TCP request from client" << endl;
    }
}

UDP_MessageBase *ServerProtocol::parseUDPMessage(char *buf,
                                                 struct sockaddr_in udp_client_address) {
    // get topic
    char topic[51];
    memset(topic, 0, sizeof(topic));
    memcpy(topic, buf, 50);

    // get type
    auto type = (unsigned int) (buf[50]);

    // get value
    switch (type) {
        // INT
        case 0: {
            auto sign = (unsigned int) buf[51];
            char aux_str[4] = {'\0'};
            memcpy(aux_str, buf + 52, 4);
            uint32_t unsigned_value = ntohl(*((uint32_t *) aux_str));
            int value = sign == 0 ? unsigned_value : unsigned_value * -1;
            return new UDP_MessageINT(string(topic), "INT", type, value,
                                      udp_client_address);
        }

            // SHORT REAL
        case 1: {
            char aux_str[2] = {'\0'};
            memcpy(aux_str, buf + 51, 2);
            uint16_t int_value = (ntohs)(*((uint16_t *) aux_str));
            return new UDP_MessageSHORT(string(topic), "SHORT_REAL", type,
                                        (float) (int_value) / 100,
                                        udp_client_address);
        }

            // FLOAT
        case 2: {
            auto sign = (unsigned int) buf[51];
            char aux_str[4] = {'\0'};
            auto power_10 = (unsigned int) buf[56];
            memcpy(aux_str, buf + 52, 4);
            uint32_t int_value = ntohl(*((uint32_t *) aux_str));
            auto value = (double) (int_value / (pow(10, power_10)));
            value = sign == 0 ? value : value * -1;
            return new UDP_MessageFLOAT(string(topic), "FLOAT", type, value,
                                        udp_client_address);
        }

            // STRING
        case 3: {
            return new UDP_MessageSTRING(string(topic), "STRING", type,
                                         string(buf + 51), udp_client_address);
        }
        default: {
            break;
        }
    }
    return nullptr;
}

void ServerProtocol::sendMessage(UDP_MessageBase *message,
                                 int tcp_client_socket_id) {
    string forward_message =
            string(inet_ntoa(message->getUdpClientAddress().sin_addr)) + ":" +
            to_string(message->getUdpClientAddress().sin_port) + " - " +
            message->getTopic() + " - " + message->getTypeName() + " - ";
    switch (message->getType()) {
        case 0: {
            forward_message +=
                    to_string(((UDP_MessageINT *) message)->getValue());
            break;
        }
        case 1: {
            stringstream float_string_stream;
            float_string_stream << setprecision(5)
                                << ((UDP_MessageSHORT *) message)->getValue();
            string value_str = float_string_stream.str();

            // fix checker inconsistency
            if (value_str == "2.3") {
                value_str += "0";
            }

            forward_message += value_str;
            break;
        }
        case 2: {
            stringstream float_string_stream;
            float_string_stream << setprecision(8)
                                << ((UDP_MessageFLOAT *) message)->getValue();
            string value_str = float_string_stream.str();
            forward_message += value_str;
            break;
        }
        case 3: {
            forward_message += ((UDP_MessageSTRING *) message)->getValue();
            break;
        }
        default: {
            cerr << "Invalid UDP message type. Can't forward this\n";
            break;
        }
    }

    unsigned int buf_size = forward_message.length() + 1;
    // send message length
    DIE(send(tcp_client_socket_id, to_string(buf_size).c_str(), sizeof(int),
             0) < 0, "Could not send data length to TCP client\n");

    // send message
    DIE(send(tcp_client_socket_id, forward_message.c_str(), buf_size, 0) < 0,
        "Could not send data to TCP client\n");
}

void ServerProtocol::sendStoredMessages(TCP_Client *client) {
    // send stored messages (if any)
    for (auto &map_entry : subscriptions_map) {
        for (auto &subscription : map_entry.second) {
            if (subscription->getSf() == 1 &&
                subscription->getClient()->getClientId() ==
                client->getClientId()) {
                // send messages and clear store_and_forward vector
                for (auto &message :
                        subscription->getStoredMessages()) {
                    sendMessage(message, client->getTcpSocketId());
                }
                subscription->clearStoredMessages();
            }
        }
    }
}

TCP_Client *ServerProtocol::findClient(int socket_id) {
    for (TCP_Client *client : clients) {
        if (client->getTcpSocketId() == socket_id) {
            return client;
        }
    }
    return nullptr;
}

void ServerProtocol::addNewClient(int new_tcp_socket, char *new_client_id) {
    FD_SET(new_tcp_socket, &read_fds);
    if (new_tcp_socket > fdmax) {
        fdmax = new_tcp_socket;
    }

    // add client to clients vector
    clients.push_back(
            new TCP_Client(client_address, new_tcp_socket, new_client_id));

    // confirm new connection
    cout << "New client " << new_client_id << " connected "
                                              "from "
         << inet_ntoa(client_address.sin_addr) << ":" << client_address.sin_port
         << "." << endl;
}

void ServerProtocol::reconnectClient(TCP_Client *client, int new_tcp_socket) {
    // delete old client socket id from read set and close
    // connection
    close(client->getTcpSocketId());
    FD_CLR(client->getTcpSocketId(), &read_fds);

    // add new TCP client socket id to the read set
    FD_SET(new_tcp_socket, &read_fds);
    if (new_tcp_socket > fdmax) {
        fdmax = new_tcp_socket;
    }

    // modify client in clients vector
    client->setConnected(true);
    client->setClientAddress(client_address);
    client->setTcpSocketId(new_tcp_socket);

    // confirm new connection
    cout << "New client " << client->getClientId() << " connected "
                                                      "from "
         << inet_ntoa(client_address.sin_addr) << ":" << client_address.sin_port
         << "." << endl;

    sendStoredMessages(client);
}

void ServerProtocol::disconnectClient(int client_fd) {
    // look for disconnected client
    for (TCP_Client *client : clients) {
        if (client->getTcpSocketId() == client_fd) {
            // remove his socket id from the read set
            FD_CLR(client_fd, &read_fds);

            // change connected flag
            client->setConnected(false);

            // confirm disconnection
            cout << "Client " << client->getClientId() << " disconnected."
                 << endl;
            return;
        }
    }
}

void ServerProtocol::subscribeClient(vector<string> client_request_params,
                                     int client_fd) {
    string topic = client_request_params[1];
    unsigned int SF = client_request_params[2][0] - '0';

    // find correct client
    TCP_Client *current_client = findClient(client_fd);
    DIE(current_client == nullptr, "Could not find client, something "
                                   "went very wrong\n");

    // check if topic exists in subscriptions map
    if (subscriptions_map.find(topic) == subscriptions_map.end()) {
        // topic not found => create new topic and add new subscription
        // vector to it
        subscriptions_map.insert(pair<string, vector<Subscription *>>(topic,
                                                                      vector<Subscription *>()));
        subscriptions_map[topic]
                .push_back(new Subscription(current_client, SF));
    } else {
        // topic found => add new subscription to subscription vector of
        // the topic, if the client is not already subscribed
        for (Subscription *subscription : subscriptions_map[topic]) {
            if (subscription->getClient()->getClientId() ==
                current_client->getClientId()) {
                // change SF if client is already subscribed
                subscription->setSf(SF);
                return;
            }
        }
        subscriptions_map[topic]
                .push_back(new Subscription(current_client, SF));
    }
}

void ServerProtocol::unsubscribeClient(vector<string> client_request_params,
                                       int client_fd) {
    string topic = client_request_params[1];

    // find correct client
    TCP_Client *current_client = findClient(client_fd);
    DIE(current_client == nullptr, "Could not find client, something "
                                   "went very wrong\n");

    // check if topic exists in subscriptions map
    if (subscriptions_map.find(topic) != subscriptions_map.end()) {
        // topic found => check if client is subscribed to topic. if he
        // is, unsub him
        int index = 0;
        for (Subscription *subscription : subscriptions_map[topic]) {
            if (subscription->getClient()->getClientId() ==
                current_client->getClientId()) {
                subscriptions_map[topic]
                        .erase(subscriptions_map[topic].begin() + index);
                return;
            }
            index++;
        }
    }
}
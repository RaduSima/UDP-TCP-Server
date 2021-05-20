#include "SubscriberProtocol.h"

SubscriberProtocol::SubscriberProtocol(const string &clientId, long serverPort)
        : client_id(clientId), server_port(serverPort) {}

SubscriberProtocol::~SubscriberProtocol() {
    close(tcp_socket_id);
}

void SubscriberProtocol::init(struct in_addr server_ip) {
    // set server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    serv_addr.sin_addr = server_ip;

    // initialise descriptor set
    FD_ZERO(&read_fds);

    // open TCP socket
    tcp_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    DIE(tcp_socket_id == -1, "Could not initialize TCP socket\n");

    // set TCP socket options
    int enable = 1;
    DIE(setsockopt(tcp_socket_id, IPPROTO_TCP, TCP_NODELAY, &enable,
                   sizeof(int)) == -1, "Could not set options for TCP "
                                       "socket\n");

    // add TCP and STDIN socket to the read file descriptors set
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(tcp_socket_id, &read_fds);
    fdmax = tcp_socket_id;

    // connect to server
    DIE(connect(tcp_socket_id, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0, "Could not connect to server\n");
    DIE(send(tcp_socket_id, client_id.c_str(), CLIENT_ID_MAX_SIZE, 0) < 0,
        "Could not send client ID to server\n");
}

void SubscriberProtocol::run() {
    fd_set tmp_fds;
    FD_ZERO(&tmp_fds);

    // actual client loop
    running_flag = true;
    while (running_flag) {
        // select proper read sockets
        tmp_fds = read_fds;
        DIE(select(fdmax + 1, &tmp_fds, nullptr, nullptr, nullptr) < 0,
            "Could not select proper sockets\n");

        // iterate through all file descriptors until fdmax
        for (int current_fd = 0; current_fd <= fdmax; current_fd += fdmax) {
            // check if the current file descriptor is part of read_fds
            if (FD_ISSET(current_fd, &tmp_fds)) {
                // handle data on proper socket

                // STDIN
                if (current_fd == STDIN_FILENO) {
                    handleDataOnSTDIN();
                }

                // SERVER
                if (current_fd == tcp_socket_id) {
                    handleDataFromServer();
                }
            }
        }
    }
}

void SubscriberProtocol::handleDataOnSTDIN() {
    string command_str;
    getline(cin, command_str);
    if (command_str == "exit") {
        running_flag = false;
        return;
    }

    // parse command
    vector<string> command_params = splitString(command_str, " ");
    if (command_params.size() != 2 && command_params.size() != 3) {
        cerr << "Incorrect command: " << command_str << endl;
        return;
    }

    if (isValidSubscribeRequest(command_params)) {
        DIE(send(tcp_socket_id, command_str.c_str(), REQUEST_MAX_LEN, 0) < 0,
            "Could not send subscribe request to server\n");
        cout << "Subscribed to topic." << endl;
    } else if (isValidUnsubscribeRequest(command_params)) {
        DIE(send(tcp_socket_id, command_str.c_str(), REQUEST_MAX_LEN, 0) < 0,
            "Could not send unsubscribe request to server\n");
        cout << "Unsubscribed from topic." << endl;
    } else {
        cerr << "Incorrect command: " << command_str << endl;
    }
}

void SubscriberProtocol::handleDataFromServer() {
    // print data
    char *buf = receiveServerData();
    if (buf) {
        cout << buf << endl;
        free(buf);
    } else {
        running_flag = false;
    }

}

char *SubscriberProtocol::receiveServerData() const {
    // receive message length from server
    char messsage_length_str[5] = {'\0'};
    int bytes_read = recv(tcp_socket_id, messsage_length_str, sizeof(int), 0);
    DIE(bytes_read < 0, "Could not receive message length from server\n");

    // server closed connection
    if (bytes_read == 0) {
        return nullptr;
    }

    errno = 0;
    char *end;
    unsigned int message_length = strtol(messsage_length_str, &end, 10);
    DIE(errno == ERANGE, "Could not parse server TCP message length\n");

    char *buf = static_cast<char *>(calloc(message_length, sizeof(char)));
    bytes_read = recv(tcp_socket_id, buf, message_length, 0);
    DIE(bytes_read < 0, "Could not receive bytes from server\n");

    return buf;
}

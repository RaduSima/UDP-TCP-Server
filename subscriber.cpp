#include "SubscriberProtocol.h"

using namespace std;

int main(int argc, char **argv) {
    DIE(argc != 4, "Usage: ./subscriber <ID_CLIENT> <IP_SERVER> "
                   "<PORT_SERVER>\n");
    setvbuf(stdout, nullptr, _IONBF, BUFSIZ);

    // get client id
    string client_id = argv[1];
        DIE(client_id.length() > 10, "Invalid client ID (too big)\n");

    // get server IP address
    struct in_addr server_ip{};
    DIE(!inet_aton(argv[2], &server_ip), "Could not get server "
                                                  "address\n");

    // get server port
    errno = 0;
    char *end;
    long server_port = strtol(argv[3], &end, 10);
    DIE(errno == ERANGE, "Could not parse server port into integer\n");

    auto* subscriber = new SubscriberProtocol(client_id, server_port);
    subscriber->init(server_ip);
    subscriber->run();

    delete subscriber;

    return 0;
}


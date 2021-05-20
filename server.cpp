#include <cstdio>
#include <string>
#include <cmath>
#include "utils.h"
#include "ServerProtocol.h"

using namespace std;

int main(int argc, char **argv) {
    DIE(argc != 2, "Usage: ./server <SERVER_PORT>\n");
    setvbuf(stdout, nullptr, _IONBF, BUFSIZ);

    // get server port
    errno = 0;
    long server_port = strtol(argv[1], nullptr, 10);
    DIE(errno == ERANGE, "Could not parse server port into integer\n");

    auto* server = new ServerProtocol(server_port);
    server->init();
    server->run();

    delete server;

    return 0;
}
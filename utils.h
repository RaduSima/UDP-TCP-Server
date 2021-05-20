#ifndef TEMA2_UTILS_H
#define TEMA2_UTILS_H

#include <vector>
#include <regex>

using namespace std;

#define BUFLEN 1560
#define REQUEST_MAX_LEN 100
#define MAX_CLIENTS 256
#define CLIENT_ID_MAX_SIZE 11

#define DIE(assertion, call_description)    \
    do {                                    \
        if (assertion) {                    \
            fprintf(stderr, "(%s, %d): ",    \
                    __FILE__, __LINE__);    \
            perror(call_description);        \
            exit(EXIT_FAILURE);                \
        }                                    \
    } while(0)

bool isValidSubscribeRequest(vector<string> client_request_params);

bool isValidUnsubscribeRequest(vector<string> client_request_params);

vector<string> splitString(const string &str, const string &delim);

#endif

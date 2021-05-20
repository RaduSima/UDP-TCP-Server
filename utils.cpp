#include <iostream>
# include "utils.h"

vector<string> splitString(const string &str, const string &delim) {
    regex reg(delim);
    vector<string> tokens
        (sregex_token_iterator(str.begin(), str.end(), reg, -1),
                sregex_token_iterator());
    return tokens;
}

bool isValidSubscribeRequest(vector<string> client_request_params) {
    return client_request_params.size() == 3 &&
           client_request_params[0] == "subscribe" &&
           client_request_params[1].length() <= 52 &&
           (client_request_params[2] == "0" || client_request_params[2] == "1");
}

bool isValidUnsubscribeRequest(vector<string> client_request_params) {
    return client_request_params.size() == 2 &&
           client_request_params[0] == "unsubscribe" &&
           client_request_params[1].length() <= 52;
}
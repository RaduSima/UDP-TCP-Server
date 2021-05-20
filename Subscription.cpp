#include "Subscription.h"


Subscription::Subscription(TCP_Client *client, unsigned int sf) : client(client),
                                                                  SF(sf) {}

Subscription::~Subscription() {
    for(auto& it : stored_messages) {
        delete(it);
    }
    stored_messages.clear();
}

unsigned int Subscription::getSf() const {
    return SF;
}

void Subscription::setSf(unsigned int sf) {
    SF = sf;
}

TCP_Client *Subscription::getClient() const {
    return client;
}

void Subscription::setClient(TCP_Client *_client) {
    Subscription::client = _client;
}

const vector<UDP_MessageBase *> &Subscription::getStoredMessages() const {
    return stored_messages;
}

void Subscription::setStoredMessages(
        const vector<UDP_MessageBase *> &storedMessages) {
    stored_messages = storedMessages;
}

void Subscription::storeMessage(UDP_MessageBase *message) {
    stored_messages.push_back(message);
}

void Subscription::clearStoredMessages() {
    for (auto& it : stored_messages) {
        delete(it);
    }
    stored_messages.clear();
}

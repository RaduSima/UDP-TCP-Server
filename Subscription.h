#ifndef TEMA2_SUBSCRIPTION_H
#define TEMA2_SUBSCRIPTION_H

#include <string>

#include "TCP_Client.h"
#include "UDP_Classes/UDP_MessageBase.h"

using namespace std;

class Subscription {
private:
    TCP_Client* client;
    unsigned int SF;
    vector<UDP_MessageBase*> stored_messages;
public:
    Subscription(TCP_Client *client, unsigned int sf);

    virtual ~Subscription();

    unsigned int getSf() const;

    void setSf(unsigned int sf);

    TCP_Client *getClient() const;

    void setClient(TCP_Client *client);

    const vector<UDP_MessageBase *> &getStoredMessages() const;

    void setStoredMessages(const vector<UDP_MessageBase *> &storedMessages);

    void storeMessage(UDP_MessageBase *message);

    void clearStoredMessages();
};


#endif

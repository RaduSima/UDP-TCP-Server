server: server.cpp UDP_Classes/UDP_MessageBase.cpp UDP_Classes/UDP_MessageINT.cpp UDP_Classes/UDP_MessageSHORT.cpp UDP_Classes/UDP_MessageFLOAT.cpp UDP_Classes/UDP_MessageSTRING.cpp TCP_Client.cpp Subscription.cpp ServerProtocol.cpp utils.cpp
	g++ -Wall -o server server.cpp UDP_Classes/*.cpp TCP_Client.cpp Subscription.cpp ServerProtocol.cpp utils.cpp

subscriber: subscriber.cpp SubscriberProtocol.cpp utils.cpp
	g++ -Wall -o subscriber subscriber.cpp SubscriberProtocol.cpp utils.cpp

clean:
	rm -f server subscriber
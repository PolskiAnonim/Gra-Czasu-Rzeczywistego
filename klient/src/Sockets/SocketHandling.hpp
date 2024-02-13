#ifndef SOCKETHANDLING
#define SOCKETHANDLING

#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <csignal>
#include <netinet/sctp.h>
//C libraries
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <csignal>
//C libraries
#include <cstdio>
#include <cstdlib>
#include <cstring>
//C++ libraries
#include <vector>
#include <string>
#include <iostream>

#include "../../../networking/src/Packets.h"
#define CONNECTIONPORT 1100
#define MAX_CONNECTIONS 5
#define MAX_CLIENTS 5

using namespace std;

struct client_t {
	int socket;
	string address;
	int port;
	int id;
};

class SocketHandling {
public:
	SocketHandling() = default;
    //Connect to server
    static int connectToServer(string);


    //Debug info/Additional information

    static NetMessageType getNetMessageType(int& descriptor);
    //Send message to server by TCP
    static int sendTCPMessage(int&, NetMessage msg);
    //Receive message
    static int getTCPMessage(int&,  NetMessage& msg);
    //Player packages

    static int sendPlayerUpdate(int& descriptor,NetPlayerUpdatePacket self);
    static int recvPlayerUpdate(int& descriptor, std::vector<NetPlayerUpdatePacket>& data);
    //Player package create

    static int recvPlayerCreate(int& descriptor, std::vector<NetCreatePlayerPacket>& data);

    static int recvPlayerDelete(int & descriptor, NetMessage& msg);

    static int getQueue(int & descriptor,std::vector<NetQueue>& queue);

    static int sendQueue(int &descriptor, NetQueue& queue);
};

#endif // SOCKETHANDLING
#ifndef SERVERSOCKETHANDLING
#define SERVERSOCKETHANDLING

#include "../Libraries.hpp"
#include "../../../networking/src/Packets.h"
#include "../Structures.hpp"
#define CONNECTIONPORT 1100
#define MAX_CONNECTIONS 5

class ServerSocketHandling {
public:
	ServerSocketHandling() = default;
	//Create socket for accepting connections
	static int CreateConnectionSocket();
	//Accepting connections
	static client_t AcceptConnection(int& descriptor);

    //Debug info/Additional information

    static NetMessageType GetPacketHeader(int& descriptor);
    static int SendNetMessage(int&, NetMessage);
    static int RecvNetMessage(int&, NetMessage&);

    //Player packages

    static int SendPlayerUpdate(int& descriptor,std::vector<NetPlayerUpdatePacket> & otherplayers);
    static int RecvPlayerUpdate(int& descriptor, NetPlayerUpdatePacket& data);
    int static SendPlayerCreate(int,std::map<int,Player>,int);

    //Queue/lobby
    static int SendQueue(int descriptor, std::vector<client_t> &clients);

    static int RecvQueue(int & descriptor, NetQueue& queue);
};

#endif // SERVERSOCKETHANDLING
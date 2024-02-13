#include "SocketHandling.hpp"
#include "SocketInfo.h"
#include "../GameManager/GameManager.h"


int SocketHandling::connectToServer(std::string address) {
    int client_socket = socket(PF_INET, SOCK_STREAM , IPPROTO_SCTP);
    if (client_socket == -1) {
        GameManager::msgManager.addMsg("Initialisastion of socket: ERROR",5.0f,0);
        return -1;
    }
    else
        GameManager::msgManager.addMsg("Initialisastion of socket: PASSED\n",5.0f,0);

    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(address.data());
    sa.sin_port = htons(1100);

    if (connect(client_socket, (sockaddr *)&sa, sizeof(sa)) == -1) {
        GameManager::msgManager.addMsg("Establishing connection: ERROR\n CANNOT REACH MAIN SERVER!!!",5.0f,0);
        close(client_socket);
        return -1;
    }
    else
    GameManager::msgManager.addMsg("Establishing connection: CONNECTED",5.0f,0);


    fcntl(client_socket, F_SETFL, fcntl(client_socket,F_GETFL,0) | O_NONBLOCK);
    SocketInfo::setSocket(client_socket);
    return 1;
}


NetMessageType SocketHandling::getNetMessageType(int& descriptor) {
    string t;
    {
        char type[3] = {};
        memset(type, 0, sizeof(type));
        recv(descriptor, type, 3,0);
        if(type[0] == '\0' && type[1] == '\0' && type[2] == '\0')
            return NONE;
        t=std::string(type, 3);

    }
    if (t=="MSG")
        return MESSAGE;
    else if (t=="NUP")
        return NETUPDATEPLAYER;
    else if (t=="NCP")
        return NETCREATEPLAYER;
    else if (t=="RMV")
        return NETDELETEPLAYER;
    else if (t=="QUE")
        return NETQUEUEPLAYER;
    else
        return NONE;
}

int SocketHandling::sendTCPMessage(int& descriptor,NetMessage msg) {
    return sendData<NetMessage>(descriptor, msg, 0);
}

int SocketHandling::getTCPMessage(int& descriptor, NetMessage& msg) {
    return getData<NetMessage>(descriptor, msg);
}

int SocketHandling::recvPlayerUpdate(int &descriptor, std::vector<NetPlayerUpdatePacket>& data) {
    return getData<NetPlayerUpdatePacket>(descriptor,data);
}


int SocketHandling::sendPlayerUpdate(int &descriptor,NetPlayerUpdatePacket self) {
     return sendData<NetPlayerUpdatePacket>(descriptor, self, MSG_NOSIGNAL);
}



int SocketHandling::recvPlayerCreate(int& descriptor,  std::vector<NetCreatePlayerPacket>& data) {
    fcntl(SocketInfo::getSocket(), F_SETFL, fcntl(SocketInfo::getSocket(),F_GETFL,0) & ~O_NONBLOCK);
    getData<NetCreatePlayerPacket>(descriptor,data);
    fcntl(SocketInfo::getSocket(), F_SETFL, fcntl(SocketInfo::getSocket(),F_GETFL,0) | O_NONBLOCK);
    return 1;
}

int SocketHandling::recvPlayerDelete(int &descriptor, NetMessage& msg) {
    return getData<NetMessage>(descriptor,msg);
}


int SocketHandling::getQueue(int &descriptor, std::vector<NetQueue>& queue) {
    return getData<NetQueue>(descriptor, queue);
}

int SocketHandling::sendQueue(int & descriptor,NetQueue& queue) {
    return sendData<NetQueue>(descriptor,queue,0);
}
#include "ServerSocketHandling.hpp"


client_t ServerSocketHandling::AcceptConnection(int &serversocket)
{
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    client_t client;    //new client
    client.queue=nullptr;//new NetQueue();
    if ((client.socket = accept(serversocket, (sockaddr*)&client_addr, &client_addr_len)) <= 0) {
        cout << ("Error accepting client connection") << endl;
    }
    //Change to non-blocking socket
    fcntl(client.socket, F_SETFL, fcntl(client.socket,F_GETFL,0) | O_NONBLOCK);
    return client;
}


int ServerSocketHandling::CreateConnectionSocket() {
    //variables
    int serversocket;
    sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(CONNECTIONPORT),
        .sin_addr = {.s_addr = inet_addr("0.0.0.0")}
    };
    //create socket
    serversocket = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_SCTP);
    if (serversocket == -1) {
        cout << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Socket created" << endl;
    int optval = 1;
    setsockopt(serversocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    //bind to address and port
    if (bind(serversocket, (sockaddr*)&addr, sizeof(addr)) == -1) {
        cout << "Error binding server socket" << endl;
        close(serversocket);
        exit(EXIT_FAILURE);
    }
    cout << "Socket binded to port "  << ":" << CONNECTIONPORT << endl;
    //change port to listening
    if (listen(serversocket, MAX_CONNECTIONS) == -1) {
        cout << "Error listening on server socket" << endl;
        close(serversocket);
        exit(EXIT_FAILURE);
    }
    cout << "Started listening" << endl;

    return serversocket;
}



NetMessageType ServerSocketHandling::GetPacketHeader(int& descriptor) {
    string t;
    {
        char type[3];
        read(descriptor, type, 3);
        t=type;
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
    else return NONE;
}

int ServerSocketHandling::SendNetMessage(int& descriptor,NetMessage message) {
    return sendData<NetMessage>(descriptor,message, MSG_NOSIGNAL);
}

int ServerSocketHandling::RecvNetMessage(int & descriptor, NetMessage& message) {
    return getData<NetMessage>(descriptor, message);
}

int ServerSocketHandling::RecvPlayerUpdate(int &descriptor, NetPlayerUpdatePacket& data) {
    return getData<NetPlayerUpdatePacket>(descriptor,data);
}


int ServerSocketHandling::SendPlayerUpdate(int &descriptor,vector<NetPlayerUpdatePacket>& otherplayers) {
    return sendData<NetPlayerUpdatePacket>(descriptor,otherplayers, MSG_NOSIGNAL);
}

//Sends all players positions and colors at the start of the game
int ServerSocketHandling::SendPlayerCreate(int descriptor, map<int,Player> players,int id) {
    vector<NetCreatePlayerPacket> otherplayers;
    otherplayers.push_back(players[id].convertToNetPlayer(id));
    for (auto & player:players) {
        if (player.first!=id) {
            NetCreatePlayerPacket otherPlayer = players[player.first].convertToNetPlayer(player.first);
            otherplayers.push_back(otherPlayer);
        }
    }
    return sendData<NetCreatePlayerPacket>(descriptor, otherplayers, MSG_NOSIGNAL);
}


int ServerSocketHandling::SendQueue(int descriptor,vector<client_t>& clients) {
    std::vector<std::reference_wrapper<NetQueue>> queue;
    for (auto& qu : clients)     if (qu.queue != nullptr)
        queue.emplace_back(*qu.queue);
    return sendData<NetQueue>(descriptor, queue, MSG_NOSIGNAL);
}

int ServerSocketHandling::RecvQueue(int &descriptor, NetQueue& queue) {
    return getData<NetQueue>(descriptor, queue);
}


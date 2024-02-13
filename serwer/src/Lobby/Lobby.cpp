#include "Lobby.hpp"


Lobby &Lobby::GetInstance() {
    static Lobby instance;
    return instance;
}

//Add new game thread
void Lobby::AddGame(vector<client_t> &clients) {
    games.push_back(jthread(GameInfo::Game, clients));
}


void Lobby::WaitAndAcceptConnection(int connectionsocket) {
        pollfd pfd[1] = {connectionsocket, POLLIN | POLLHUP};
        int res = poll(pfd, 1, 500);
        if (res == 0)
            return; //timeout
        //if there is new client connected
        if (pfd[0].revents & POLLIN) {
            client_t client = ServerSocketHandling::AcceptConnection(connectionsocket);
            Queue::GetInstance().AddClient(client);
            cout << "Connection accepted" << std::endl;
        }
}

[[noreturn]] void Lobby::Server() {
    //Socket used for accepting new connections
    int connectionsocket = ServerSocketHandling::CreateConnectionSocket();

    while (true) {
        //Wait (in poll) and accept new connections if there are any
        Lobby::GetInstance().WaitAndAcceptConnection(connectionsocket);
        //Queue functions - can be done once per about 1 second
        Queue::RunQueue();
        //Clear vector from old games
        for (int i = 0; i < Lobby::GetInstance().games.size(); i++) {
            if (!Lobby::GetInstance().games[i].joinable()) {
                Lobby::GetInstance().games.erase(Lobby::GetInstance().games.begin() + i--);
            }
        }

    }


}
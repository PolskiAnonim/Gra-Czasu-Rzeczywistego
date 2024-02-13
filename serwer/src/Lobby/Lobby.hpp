#pragma once
#include "../Libraries.hpp"
#include "Queue.hpp"
#include "../Game/GameMain.hpp"

class Lobby
{
private:
    vector<jthread> games;
//Singleton
    Lobby()=default;
    ~Lobby()=default;
public:
    Lobby(const Lobby&) = delete;
    Lobby(Lobby&&) = delete;
    Lobby& operator= (const Lobby&) = delete;
    Lobby& operator= (Lobby&&) = delete;

    static Lobby& GetInstance();
    //Lobby
    void WaitAndAcceptConnection(int socket);
    void AddGame(vector<client_t> &clients);

    [[noreturn]] static void Server();
};
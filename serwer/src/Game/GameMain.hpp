#ifndef GAMEMAIN
#define GAMEMAIN
#include "../Libraries.hpp"
#include "../Structures.hpp"
#include "Player/Player.hpp"
#include <chrono> //- no nwm

class GameInfo {
    vector<pair<short,client_t>> clients;
    map <int,jthread> clients_threads;

public:
    GameInfo(vector<client_t> cli);
    //Synchronization of threads
    latch* sendlatch;
    latch* validationlatch;
    latch* recvlatch;
    latch* cleanlatch;
    //(Dead) players
    map <int,Player> players;
    vector<pair<int,int>> invalid;

    //Jak w PKP
    void ValidatePositionsAndKillPlayers();
    void CheckForExitGame();
    void ClientSynchronize(short phase);
    void MainWaitAndSynchronize(short phase);

    static void Game(vector<client_t> clients);
};

#include "ClientThread.hpp"
#endif //GAMEMAIN

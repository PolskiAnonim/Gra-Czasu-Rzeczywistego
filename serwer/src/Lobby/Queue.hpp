#ifndef QUEUE
#define QUEUE
#include "../../networking/src/Packets.h"
#include "../Libraries.hpp"
#include "../Structures.hpp"
#include "../Game/GameMain.hpp"
#include "Lobby.hpp"

class Queue {
    //Variables
    //Clients
    vector<vector<client_t>> clients;
    //Timer
    vector<long long> timer;
    //Singleton
    Queue()=default;
    ~Queue()=default;
public:
    Queue(const Queue&) = delete;
    Queue(Queue&&) = delete;
    Queue& operator= (const Queue&) = delete;
    Queue& operator= (Queue&&) = delete;

    static Queue& GetInstance();
    //Clients for Lobby class
    void AddClient(client_t&);
    //Timer
    void TryResetTimer(int&);
    //Loop
    static void RunQueue();
    //Clients information about players
    void ColorAndName(int&);
    //Check for conditions to start new game
    void CheckStartNewGame(int&);
    //For clients waiting in lobby
    void SendQueueState(int&);
};

#endif //QUEUE

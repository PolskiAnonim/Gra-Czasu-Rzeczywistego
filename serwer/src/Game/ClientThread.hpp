#pragma once
#include "../Libraries.hpp"
#include "GameMain.hpp"
#include "Player/Player.hpp"

class ClientThread {
    static inline thread_local bool killed;
    static inline thread_local long long timer;
public:
    static void Thread(GameInfo* gameinfo,pair<short,client_t> client);
    static void PhaseGetPlayerUpdate(GameInfo * gameinfo,int &socket, short &id);
    static void PhaseValidatePlayer(GameInfo * gameinfo,int &socket, short &id);
    static void PhaseSendPlayerUpdate(GameInfo * gameinfo,int &socket, short &id);
};


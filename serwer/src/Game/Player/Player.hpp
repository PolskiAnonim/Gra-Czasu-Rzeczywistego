#ifndef PLAYER
#define PLAYER
#include "../../Libraries.hpp"
#include "../../Sockets/ServerSocketHandling.hpp"
#include "../../../../networking/src/Packets.h"
#include <cmath>



class PlayerHandling {
public:
    static Player createPlayer(pair<short,client_t> &client);

    static void updatePlayer(Player &, NetPlayerUpdatePacket&);

    static pair<bool,int> validatePlayer(map<int, Player> &players, int id);

    static NetPlayerUpdatePacket createUpdatePacket(Player &player,int id);
};

#endif //PLAYER
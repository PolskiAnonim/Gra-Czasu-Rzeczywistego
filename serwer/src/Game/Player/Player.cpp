#include "Player.hpp"

using namespace std;

Player PlayerHandling::createPlayer(pair<short,client_t> &client)
{
    Player player;
    player.dzialoRotation=0;
    player.x=client.first%3*500+500;
    player.y=client.first%2*500+500;
    player.color=client.second.queue->color;
    player.playerName=client.second.queue->name.data(); //czy to ma sens?
    player.speed = NetSpeed{0,0,0.2,2};
    player.shootCd=0.05;
    player.size=10;
    delete client.second.queue;
    return player;
}

void PlayerHandling::updatePlayer(Player &player,NetPlayerUpdatePacket &packet)
{
    player.dzialoRotation=packet.dzialoRotation;
    player.x=packet.x;
    player.y=packet.y;
    player.projectiles=packet.projectilki;
    player.speed=packet.speed;
}

NetPlayerUpdatePacket PlayerHandling::createUpdatePacket(Player &player,int id)
{
    NetPlayerUpdatePacket packet;
    packet.projectilki=player.projectiles;
    packet.dzialoRotation=player.dzialoRotation;
    packet.playerId=id;
    packet.x=player.x;
    packet.y=player.y;
    packet.size=player.size;
    packet.speed=player.speed;
    packet.color=player.color;
    return packet;
}


pair<bool,int> PlayerHandling::validatePlayer(map<int, Player> &players, int id) {
    Player self=players[id];
    pair <bool,int> ret;
    ret.first=true;
    ret.second=-1;
    for (auto & player: players)
    {
        if (player.first!=id)
        {
            for (auto & projectile:player.second.projectiles) {
                if (projectile->x==0 && projectile->y==0)
                    continue;
                float dx = projectile->x - self.x;      // X distance between centers
                float dy = projectile->y - self.y;      // Y distance between centers

                float distance = sqrtf(dx * dx + dy * dy); // Distance between centers

                if (distance <= (5.0f + self.size))
                {
                    ret.first=false;
                    ret.second=player.first;
                    return ret;
                }
            }

        }
    }
    return ret;
}
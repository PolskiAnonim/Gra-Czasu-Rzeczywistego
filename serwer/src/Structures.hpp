#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP
#include "../../networking/src/Packets.h"

struct client_t {
    int socket;
    NetQueue *queue; //pointer/player color and name
};

//Player used by server
struct Player {
    //Name and color
    std::string playerName;
    NetColor color;
    //Position and size
    float x;
    float y;
    float size;
    float shootCd;
    //speed
    NetSpeed speed;
    float dzialoRotation;
    std::array<std::optional<NetProjectile>,16> projectiles;

    inline NetCreatePlayerPacket convertToNetPlayer(int id){
        return {
                id, this->shootCd, this->playerName,
                NetMoveAble(
                        this->size, this->color, this->speed,
                        NetRectangle(this->x,this->y))};
    }
};

#endif

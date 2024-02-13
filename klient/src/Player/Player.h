#pragma once
#ifndef SIECI_PLAYER_H
#define SIECI_PLAYER_H

#include "raylib/raylib-cpp.hpp"
#include "MoveAble.h"
#include "Projectile.h"
#include <memory>
#include <string>
#include "../Board/WindowManager.h"
#include <optional>


class Player : public MoveAble {
protected:
    Rectangle dzialo;

    std::array<std::optional<Projectile>, 16> projectiles;

    float lastTimeShoot = 0;
    float shootCd = 0.05;
    int it = 0;
    void setPos(float x, float y) override;
    Rectangle dzialoObwodka;
    float dzialoRotation;
public:
    bool shouldShown = true;
    bool shootingDisabled = false;
    Player();
    Player(float x, float y, float size);
    explicit Player(NetCreatePlayerPacket pckg);
    void Draw() override;

    void ApplyUpdate(const NetPlayerUpdatePacket& update);

    void setColor(raylib::Color color){
        this->color = color;
    }
    void setName(std::string name){
        std::strcpy(this->playerName.data(), name.data());
    }
    std::array<char,16> getName(){
        return this->playerName;
    }

    std::array<std::optional<Projectile>,16> getProjectiles(){
        return this->projectiles;
    }

    float getRotation() const
    {
        return dzialoRotation;
    }

    std::array<char,16> playerName;
};
#endif //SIECI_PLAYER_H
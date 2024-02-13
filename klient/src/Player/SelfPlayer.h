#pragma once
#include "Player.h"



class SelfPlayer : public Player {
    raylib::Rectangle& boardColision;
    Sound sound = WindowManager::getSound();
public:
    int projectileid=0;
    short id;
    explicit SelfPlayer(raylib::Rectangle& boardArea) : boardColision(boardArea){};
    SelfPlayer(NetCreatePlayerPacket pckg,raylib::Rectangle& boardArea):
        Player(pckg),boardColision(boardArea){
        this->id = pckg.playerId;
    };
    void move();

    SelfPlayer& operator=(const SelfPlayer& other) {
        if (this != &other) {
            this->id = other.id;
            this->boardColision = other.boardColision;
            this->projectileid = other.projectileid;
            this->sound = other.sound;
            this->dzialoRotation = other.dzialoRotation;
            this->projectiles = other.projectiles;
            this->x = other.x;
            this->y = other.y;
            this->size = other.size;
            this->lastTimeShoot = other.lastTimeShoot;
            this->shootingDisabled = other.shootingDisabled;
            this->playerName = other.playerName;
            this->width = other.width;
            this->height = other.height;
            this->speed = other.speed;
            this->color = other.color;
            this->shootCd = other.shootCd;

        }
        return *this;
    }
};

class RotatingPlayer : public Player {
    Sound sound = WindowManager::getSound();
public:
    bool blocked = false;
    int projectileid=0;
    inline RotatingPlayer(float x, float y, float size) : Player(x,y,size) {
        this->shouldShown = false;
    };

    inline void move(){
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (GetTime() - lastTimeShoot > this->shootCd) {

                this->projectiles[projectileid]=(Projectile(this->x  + size * 2 * cos(DEG2RAD * (dzialoRotation + 90)),
                                                                                                       this->y  + 2 * size * sin(DEG2RAD * (dzialoRotation + 90)), 5.0f,
                                                                                                       Speed(this->speed, this->dzialoRotation) * 2.0f, this->color));
                this->projectileid = this->projectileid<14 ? ++this->projectileid : 0;
                lastTimeShoot = GetTime();
            }
        }

        float pixY = this->y;
        float pixX = this->x;

        float rotation = 0;
        float tg;
        float r = pixY - GetMouseY();
        if (r == 0)
        {
            if (GetMouseX() > pixX)
                rotation = 270;
            if (GetMouseX() < pixX)
                rotation = 90;
        }
        else {
            tg = (pixX - GetMouseX()) / r;
            rotation = -RAD2DEG * atan(tg);

        }
        if (GetMouseY() < pixY && blocked)
            rotation = 360-rotation ;
        if (GetMouseY() < pixY && !blocked)
            rotation = rotation+180 ;
        this->dzialoRotation = rotation;
        if (!this->projectiles.empty()) {
            double time = GetTime();
            for (auto& projectile: projectiles) {
                if(projectile->cleared) continue;
                if (time - projectile->getSpawnTime() > projectile->getDeltaTime()) {
                    projectile = std::nullopt;
                }
                else {
                    projectile->move();
                }
            }
        }
    }
};


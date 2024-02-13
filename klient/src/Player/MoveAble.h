#pragma once
#ifndef SIECI_MOVEABLE_H
#define SIECI_MOVEABLE_H
#include "raylib/raylib-cpp.hpp"
#include <iostream>
#include "../../networking/src/Packets.h"

struct Speed {
    raylib::Vector2 currentSpeed;
    float acceleration;
    float maxSpeed;

    Speed() {
        this->currentSpeed = raylib::Vector2(0,0);
        this->acceleration = 0.2;
        this->maxSpeed = 2;
    };

    Speed(const Speed& speed, float kat) {
        this->currentSpeed = (raylib::Vector2(-sin(DEG2RAD * kat), cos(DEG2RAD * kat)))* speed.maxSpeed + speed.currentSpeed;
        this->acceleration = speed.acceleration;
        this->maxSpeed = speed.maxSpeed;
    }

    explicit Speed(const NetSpeed& speed) {
        this->currentSpeed = raylib::Vector2(speed.currentSpeedX, speed.currentSpeedY);
        this->acceleration = speed.acceleration;
        this->maxSpeed = speed.maxSpeed;
    }

    NetSpeed Serialize() {
        auto speed = NetSpeed{
                this->currentSpeed.GetX(),
                this->currentSpeed.GetY(),
                this->acceleration,
                this->maxSpeed
        };
        return {};
    }
};

inline Speed operator*( const Speed& y, float x){
    Speed result;
    result.currentSpeed.x = x*y.currentSpeed.x;
    result.currentSpeed.y = x*y.currentSpeed.y;
    result.acceleration = y.acceleration;
    result.maxSpeed = y.maxSpeed;
    return result;
}

class MoveAble : public Rectangle {
protected:
    raylib::Color color;
    Speed speed;
    float size;

    virtual void setPos(float x, float y) = 0;
public:
    virtual void Draw() = 0;
    Speed getSpeed(){
        return this->speed;
    }


};

#endif //SIECI_MOVEABLE_H
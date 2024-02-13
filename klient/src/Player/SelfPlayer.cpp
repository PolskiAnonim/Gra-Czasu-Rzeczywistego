#include <algorithm>
#include "SelfPlayer.h"


void SelfPlayer::move() {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        if (IsKeyDown(KEY_A))
            this->speed.currentSpeed.x = std::max(-this->speed.maxSpeed,
                                                  this->speed.currentSpeed.x - this->speed.acceleration);
        if (IsKeyDown(KEY_D))
            this->speed.currentSpeed.x = std::min(this->speed.maxSpeed,
                                                  this->speed.currentSpeed.x + this->speed.acceleration);
    }
    else {
        if (this->speed.currentSpeed.x < 0)
            this->speed.currentSpeed.x = std::min(0.0f, this->speed.currentSpeed.x + this->speed.acceleration);
        else
            this->speed.currentSpeed.x = std::max(0.0f, this->speed.currentSpeed.x - this->speed.acceleration);
    }

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) {

        if (IsKeyDown(KEY_W))
            this->speed.currentSpeed.y = std::max(-this->speed.maxSpeed,
                                                  this->speed.currentSpeed.y - this->speed.acceleration);

        if (IsKeyDown(KEY_S))
            this->speed.currentSpeed.y = std::min(this->speed.maxSpeed,
                                                  this->speed.currentSpeed.y + this->speed.acceleration);
    }
    else {
        if (this->speed.currentSpeed.y < 0)
            this->speed.currentSpeed.y = std::min(0.0f, this->speed.currentSpeed.y + this->speed.acceleration);
        else
            this->speed.currentSpeed.y = std::max(0.0f, this->speed.currentSpeed.y - this->speed.acceleration);
    }
    this->x = std::clamp(this->x + this->speed.currentSpeed.x, 0.0f, this->boardColision.width);
    this->y = std::clamp(this->y + this->speed.currentSpeed.y, 0.0f, this->boardColision.height);

    this->setPos(this->x + this->speed.currentSpeed.x, this->y + this->speed.currentSpeed.y);

    raylib::Vector2 windowSize = WindowManager::getInstance().getWindowSize();
    float pixY = windowSize.y / 2;
    float pixX = windowSize.x / 2;

    if (IsKeyDown(KEY_SPACE)) {
        PlaySound(this->sound);
        if (GetTime() - lastTimeShoot > this->shootCd) {

            this->projectiles[projectileid]=(Projectile(this->x  + size * 2 * cos(DEG2RAD * (dzialoRotation + 90)),
                                                        this->y  + 2 * size * sin(DEG2RAD * (dzialoRotation + 90)), 5.0f,
                                                        Speed(this->speed, this->dzialoRotation) * 2.0f, this->color));
            this->projectileid = this->projectileid<14 ? ++this->projectileid : 0;
            lastTimeShoot = GetTime();
        }
    }

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
    if (GetMouseY() < pixY)
        rotation = rotation + 180;
    this->dzialoRotation = rotation;
    if (!this->projectiles.empty()) {
        double time = GetTime();
        for (auto& projectile: projectiles) {
            if(projectile == std::nullopt) continue;
            if (time - projectile->getSpawnTime() > projectile->getDeltaTime() ||
                !CheckCollisionPointRec(raylib::Vector2(projectile->x - projectile->getSpeed().currentSpeed.x ,projectile->y - projectile->getSpeed().currentSpeed.y),this->boardColision)) {
                projectile = std::nullopt;
            }
            else {
                projectile->move();
                if((projectile->x < 0 && projectile->x > this->boardColision.width) || (projectile->y < 0 && projectile->y > this->boardColision.height))
                    projectile = std::nullopt;

            }
        }
    }
}


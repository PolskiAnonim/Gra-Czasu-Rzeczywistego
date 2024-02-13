#include "Player.h"

void Player::setPos(float x, float y) {
    this->x = x;
    this->y = y;
}


void Player::ApplyUpdate(const NetPlayerUpdatePacket& update) {
    this->speed = Speed{ update.speed };
    this->size = update.size;
    this->x = update.x;
    this->y = update.y;
    this->dzialoRotation = update.dzialoRotation;
    for (int i=0;i<16;i++)
        this->projectiles[i]= Projectile(update.projectilki[i], this->color);
}

Player::Player() {
    this->x = 250;
    this->y = 250;
    this->size = 10;
    color = raylib::Color(GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255));
    this->dzialo = raylib::Rectangle(this->x + (size / 4), this->y - (size / 4), size / 2, size * 2);
}


Player::Player(float x, float y, float size) {
    this->x = x;
    this->y = y;
    this->size = size;
    color = raylib::Color(GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255));
    this->dzialo = raylib::Rectangle(this->x + (size / 4), this->y - (size / 4), size / 2, size * 2);
}

Player::Player(NetCreatePlayerPacket pckg) {
    this->x=pckg.x;
    this->y=pckg.y;
    this->playerName=pckg.playerName;
    this->shootCd=pckg.shootCd;
    this->size=pckg.size;
    this->speed=Speed(pckg.speed);
    this->color = raylib::Color(pckg.color.r,pckg.color.g,pckg.color.b,pckg.color.a); //TODO

    this->dzialo = raylib::Rectangle(this->x + (size / 4), this->y - (size / 4), size / 2, size * 2);
    this->shootingDisabled=false;
}

void Player::Draw() {
    dzialo.x = this->x;
    dzialo.y = this->y;
    if(shouldShown)
        DrawText(this->playerName.data(),this->x - (MeasureTextEx(GetFontDefault(),playerName.data(),22,1).x)/2.0f,this->y-60,22,BLACK);

    DrawRectanglePro(dzialo,raylib::Vector2(this->size/4, 0),this->dzialoRotation, GRAY);
    DrawCircle(this->x, this->y, this->size * 1.1, BLACK);
    DrawCircle(this->x, this->y, this->size, color);
    if(!shootingDisabled) {
        for (auto &projectile: projectiles) {
            if(projectile != std::nullopt && (projectile->x != 0 && projectile->y != 0))
                 projectile.value().Draw();
        }
    }

}




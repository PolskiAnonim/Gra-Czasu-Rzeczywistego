#include "Projectile.h"

void Projectile::setPos(float x, float y)  {
    this->x = x;
    this->y = y;
}

void Projectile::move(){
    this->setPos(this->x + this->speed.currentSpeed.x, this->y + this->speed.currentSpeed.y);
}



NetProjectile Projectile::Serialize() {
    NetProjectile netproj = NetProjectile{
            this->speed.Serialize(),
            this->spawnTime,
            this->x,
            this->y
    };

    return netproj;
}


Projectile::Projectile(float x, float y, float size, Speed speedVector,raylib::Color color) {
    this->x = x;
    this->y = y;
    this->size = size;
    this->speed = speedVector;
    this->color = color;
    this->spawnTime = GetTime();
    this->width =5;
    this->height=5;
}

Projectile::Projectile(std::optional<NetProjectile> netProjectile,  raylib::Color color) {
    this->x = netProjectile->x;
    this->y = netProjectile->y;
    this->speed = Speed(netProjectile->speed);
    this->size = 5.0f;
    this->color = color;
    this->spawnTime = netProjectile->spawnTime;
}

void Projectile::Draw(){
    DrawCircle(this->x, this->y, this->size * 1.1f, BLACK);
    DrawCircle(this->x, this->y, this->size, color);
}

double Projectile::getSpawnTime() const{
    return this->spawnTime;
}

double Projectile::getDeltaTime() const {
    return this->halfLife;
}

Projectile &Projectile::operator=(NetProjectile obj) {
    this->speed=Speed(obj.speed);
    this->spawnTime=obj.spawnTime;
    this->x=obj.x;
    this->y=obj.y;
    this->halfLife=obj.halfLife;
    return *this;
}


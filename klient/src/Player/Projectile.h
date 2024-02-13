#include "raylib/raylib-cpp.hpp"
#include "MoveAble.h"
#include "../../networking/src/Packets.h"


class Projectile : public MoveAble {
    float halfLife = 2; // Sekundy +- 1/4 Niby
    double spawnTime;

public:
    void setPos(float x, float y) override;
    void move();
    Projectile(float x, float y, float size,
               Speed speedVector,raylib::Color color);
    explicit Projectile(std::optional<NetProjectile> netProjectile, raylib::Color color);
    void Draw() override;
    NetProjectile Serialize();

    Projectile& operator=(NetProjectile);
    [[nodiscard]] double getSpawnTime() const;
    [[nodiscard]] double getDeltaTime() const;

    bool cleared = false;
};
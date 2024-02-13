#pragma once
#ifndef SIECI_GAMESTAGEQUEUE_H
#define SIECI_GAMESTAGEQUEUE_H
#include "raylib/raylib-cpp.hpp"
#include <memory>
#include "../Game.h"
#include "../../Board/WindowManager.h"
#include "../../Player/Player.h"
#include "../GameManager.h"
#include "../../Player/SelfPlayer.h"


class TableLine{
    Rectangle border;
    std::array<char, 16> name;
public:
    RotatingPlayer player;
    void draw();
    TableLine(int x, int y, int width, int height);
    void setPlayerColor(raylib::Color color);
    void setPlayerName(std::string name);
    void replaceQueue(NetQueue queue);
    void clear();

};

class GameStageQueue : public Game, public MessageHandler {
    bool firstPackageFree = true;

    raylib::Color playerColor = GameManager::getInstance().playerColor;

    std::unique_ptr<const raylib::Vector2> window{
            std::make_unique<const raylib::Vector2>(WindowManager::getInstance().getWindowSize())};
    std::vector<TableLine> table;
    bool countDownStarted = false;
    int count = 99;
    std::string time;
    void s() override;
    void w() override;
    void k(char kto, char kogo) override;
    void t(char ile) override;

public:
    void serialiseNetQueue(std::vector<NetQueue>);
    GameStageQueue();

    void input() override;

    void updateDrawFrame() override;

    static inline raylib::Color serializeNetColor(NetColor netColor) {
        return {static_cast<unsigned char>(netColor.r), static_cast<unsigned char>(netColor.g), static_cast<unsigned char>(netColor.b), static_cast<unsigned char>(netColor.a)};
    }
    inline int getStage() override{
        return GameStageQueueLevel;
    }
};


#endif //SIECI_GAMESTAGEQUEUE_H

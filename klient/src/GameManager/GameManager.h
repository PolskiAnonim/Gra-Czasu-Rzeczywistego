#pragma once
#include "raylib/raylib-cpp.hpp"
#include <memory>
#include "Game.h"
#include "../../../networking/src/Packets.h"
#include "../Board/WindowManager.h"
#include <queue>
#include <utility>
#include <unistd.h>
#include <optional>
#include <map>

extern inline raylib::Rectangle operator*(const raylib::Rectangle& rect, float scalar){
    raylib::Rectangle result = rect;
    result.width += scalar*scalar;
    result.height *= scalar;

    result.x -= (result.width - rect.width) / 2.0f;
    result.y -= (result.height - rect.height) / 2.0f;
    return result;
}


struct MsgLog{
    std::string msgBody;
    double time;
    float halfLife;
    int stage;

    inline MsgLog(std::string msg,float halfLifeGot,int toStage,double timeGot) :
        msgBody(std::move(msg)), time(timeGot), halfLife(halfLifeGot), stage(toStage) {};

    std::string extractText();
};

class MsgManager{
    std::list<MsgLog> log;

    // argumenty podaje jako prawy góry róg
    float drawSingleMsg(MsgLog msg, int x, int y);
    void manageLog();
public:
    void drawMsgLog();
    void addMsg(std::string msg, float halfLife, int toStage);
};

class GameManager {
public:
    inline static MsgManager msgManager;
    inline static std::queue<std::unique_ptr<Game>> stages;
    inline static std::unique_ptr<Game> currentStage;

    GameManager() = default;
    ~GameManager() = default;
    void changeState();
    void reset();
    void run();
    void addStage(std::unique_ptr<Game> stage);
    void setUp();


public:
    static inline std::string resetReason = "";
    raylib::Color playerColor;
    inline static std::string playerName;
    inline static int playerId;
    inline static std::map<int, std::array<char, 16>> playerList;

    inline static bool shouldReset = false;
    inline static bool stageShouldChange = false;
    GameManager(const GameManager&) = delete;
    GameManager(GameManager&&) = delete;
    GameManager& operator= (const GameManager&) = delete;
    GameManager& operator= (GameManager&&) = delete;


    static GameManager& getInstance();
    static void resetRequest(std::string reason);
    void setChangeState(){
        this->stageShouldChange = true;
    }

    static int getCurrentStage();
};




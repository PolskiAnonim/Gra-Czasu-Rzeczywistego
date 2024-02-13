#define RAYGUI_IMPLEMENTATION
#include <numeric>
#include <utility>
#include "GameManager.h"
#include "stages/GameStage1.h"
#include "stages/GameStageLogin.h"
#include "stages/GameStageSelect.h"
#include "stages/GameStageQueue.h"


GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::run() {
    // GamePlay loop
    this->setUp();
    while (!this->stages.empty()) {
        if(WindowShouldClose()) return;
        (*currentStage).input();
        (*currentStage).updateDrawFrame();

        if(shouldReset){
            this->reset();
        }
        if (stageShouldChange) {
            this->changeState();
            stageShouldChange = false;
            currentStage = std::move(this->stages.front());
        }
    }
}

void GameManager::addStage(std::unique_ptr<Game> stage) {
    stages.push(std::move(stage));
}

 int GameManager::getCurrentStage(){
    return currentStage->getStage();
}

void GameManager::changeState() {
    stages.pop();
}

void GameManager::resetRequest(std::string reason) {
    resetReason = reason;
    shouldReset = true;
}

void GameManager::reset() {
    if(SocketInfo::getSocket() > 0) {
        close(SocketInfo::getSocket());
        SocketInfo::setSocket(-1);
    }
    this->shouldReset = false;
    std::queue<std::unique_ptr<Game>>().swap(this->stages);
    this->setUp();
}

void GameManager::setUp() {
    addStage(std::make_unique<GameStageLogin>());
    addStage(std::make_unique<GameStageSelect>());
    addStage(std::make_unique<GameStageQueue>());
    addStage(std::make_unique<GameStage1>());
    currentStage = std::move(this->stages.front());
}

// Zwraca y - długoś całego tekstu
float MsgManager::drawSingleMsg(MsgLog msg, int x, int y)  {
    raylib::Vector2 width = MeasureTextEx(GetFontDefault(),msg.extractText().c_str(),12.0f,1.0f);
    width.x+=60;
    raylib::Rectangle rozmiar = raylib::Rectangle(x - width.x,y,width.x,width.y);
    DrawRectangleRounded(rozmiar*1.1f,1.0f,1,BLACK);
    DrawRectangleRounded(rozmiar,1.0f,1,raylib::Color(246,215,176));
    DrawText(msg.extractText().c_str(),rozmiar.x + (rozmiar.x - width.x)/8,rozmiar.y,12,BLACK); //TODO NAPRAW
    return width.y + 10.0f;
}

void MsgManager::manageLog() {
    double time = GetTime();
    for(auto it = this->log.begin(); it != this->log.end();){
        int stageToCheck = GameManager::getCurrentStage();
        if(it->stage !=stageToCheck || it->time + it->halfLife < time){
            it = this->log.erase(it);
        }
        else it++;
    }
}

void MsgManager::drawMsgLog() {
    manageLog();
    raylib::Vector2 windowSize = WindowManager::getWindowSize();
    float offset = 0.0f;
    for(auto& msg : this->log){
        offset += drawSingleMsg(msg, windowSize.x - 40, offset + 20.0f);
    }
}

void MsgManager::addMsg(std::string msg, float halfLife, int toStage) {
    log.emplace_back(MsgLog(std::move(msg),halfLife,toStage,GetTime()));
}

std::string MsgLog::extractText() {
    return std::accumulate(this->msgBody.begin(), this->msgBody.end(), std::string{});
}

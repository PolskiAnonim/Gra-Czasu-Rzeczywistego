#include "GameStageQueue.h"
#include "../../Sockets/SocketInfo.h"
#include "../../Sockets/SocketHandling.hpp"


void GameStageQueue::input() {
    for(auto& row: this->table){
        row.player.move();
    }

    int response = SocketHandling::getNetMessageType(SocketInfo::getSocket());
    if(response != NONE)
        timeSinceLastPacket = GetTime();
    if(response == NETCREATEPLAYER){
        GameManager::getInstance().setChangeState();
    }
    else if(response == NETQUEUEPLAYER){
        std::vector<NetQueue> queue;
        int resp = SocketHandling::getQueue(SocketInfo::getSocket(), queue);
        serialiseNetQueue(queue);
    }
    else if(response == MESSAGE){
        NetMessage message;
        int resp = SocketHandling::getTCPMessage(SocketInfo::getSocket(), message);
        this->handleMessage(message);
    }
    if(firstPackageFree){
        timeSinceLastPacket = GetTime();
        firstPackageFree = false;
    }
    if(GetTime() > timeSinceLastPacket + 3){
        GameManager::resetRequest("No connection with server!");
    }
}

void GameStageQueue::updateDrawFrame() {
    BeginDrawing();
    ClearBackground(WHITE);

    if(this->count == 99){
        DrawText("Awaiting more players",this->window->x/2.0f,this->window->y/2.0f,20,GRAY);
    }
    else{
        if(this->count > 27){
            DrawText("The countdown will start soon",this->window->x/2.0f,this->window->y/2.0f,20,GRAY);
        }
        else if(this->count < 5){
            DrawText("The game is starting",this->window->x/2.0f,this->window->y/2.0f,20,GRAY);
        }
        else{
            DrawText("The game will start in", this->window->x / 2.0f + 75, this->window->y / 2.0f - 50, 20, GRAY);
            DrawText(std::to_string(count - '\0').c_str(), this->window->x / 2.0f + 80, this->window->y / 2.0f, 20, GRAY);

        }
    }
    for(auto& row : table){
        row.draw();
    }

    DrawFPS(0,0);

    EndDrawing();
}

void TableLine::draw() {
    DrawRectangleLinesEx(this->border,1.0f,BLACK);
    this->player.Draw();
    DrawText(this->player.getName().data(),this->border.x+5,this->border.y+32,18,BLACK);
    DrawLine(this->border.x+150,this->border.y,this->border.x+150,this->border.y+80, BLACK);
}

TableLine::TableLine(int x, int y, int width, int height) :
border(raylib::Rectangle(x,y,width,height)), player(RotatingPlayer(x + 175, y + 30,15.0f)) {
    this->clear();
    this->player.blocked = false;
}

GameStageQueue::GameStageQueue() {
    this->timeSinceLastPacket = GetTime();
    for(int i = 0;i!=4;i++){
        table.emplace_back(this->window->x/3.0f - 200,this->window->y/3.0f - 100 + i * 80,200,80);
        table[i].player.shootingDisabled = false;
    }
}

void TableLine::setPlayerColor(raylib::Color color) {
    this->player.setColor(color);
}

void TableLine::setPlayerName(std::string myName) {
    memcpy(this->name.data(),myName.data(),myName.size());
}

void TableLine::replaceQueue(NetQueue queue) {
    this->player.setName(queue.name.data());
    this->player.setColor(GameStageQueue::serializeNetColor(queue.color));
}

void TableLine::clear() {
    this->player.setColor(WHITE);
    this->player.setName("Awaiting player");
}


void GameStageQueue::serialiseNetQueue(std::vector<NetQueue> arr) {
    for (int i = 0; i != this->table.size(); i++) {
        if (i < arr.size()) {
            table[i].replaceQueue(arr[i]);
        } else {
            table[i].clear();
        }
    }
}

    // Start
    void GameStageQueue::s() {
    std::cout << "START" << std::endl;
        GameManager::getInstance().setChangeState();
    }

    //Win
    [[maybe_unused]] void GameStageQueue::w() {}
    [[maybe_unused]] void GameStageQueue::k(char kto, char kogo) {}

    void GameStageQueue::t(char ile) {
        this->count=ile;
    }

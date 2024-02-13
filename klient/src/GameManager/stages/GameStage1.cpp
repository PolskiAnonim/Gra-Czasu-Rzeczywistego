#include "GameStage1.h"
#include "../GameManager.h"

#include <utility>


void inline GameStage1::input() {
    if(!iniciated){
        handleCreate();
        iniciated=true;
    }
    this->player.move();
    handleUpdate();
}

void GameStage1::updateDrawFrame() {
    BeginDrawing();
    ClearBackground(WHITE);
    BeginMode2D(this->camera);
    board.Draw();
    cameraSetTarget(raylib::Vector2(player.x - this->window->x / 2, player.y - this->window->y / 2));
    this->player.Draw();

    for(auto& pl : players){
        pl.second.Draw();
    }


    EndMode2D();
    GameManager::msgManager.drawMsgLog();
    EndDrawing();
}

void GameStage1::cameraSetTarget(raylib::Vector2 target) {
    WindowManager::getInstance().getWindowSize();
    this->camera.SetTarget(target);
}

void GameStage1::handleUpdate() {
    array<std::optional<NetProjectile>, 16> npr;
    for (int i=0;i<16;i++) {
        if(player.getProjectiles()[i].has_value())
            npr[i] = player.getProjectiles()[i]->Serialize();
        else
            npr[i] = nullopt;
    }

    SocketHandling::sendPlayerUpdate(SocketInfo::getSocket(),NetPlayerUpdatePacket(
        player.getSpeed().Serialize(),player.x, player.y, player.getRotation(),npr)
        );
    //wait for validation?
    int response;
    do {
        response = SocketHandling::getNetMessageType(SocketInfo::getSocket());
        if (response == NETUPDATEPLAYER) {
            vector<NetPlayerUpdatePacket> data;
            SocketHandling::recvPlayerUpdate(SocketInfo::getSocket(), data);
            for (auto &packet: data)
                this->players.at(packet.playerId).ApplyUpdate(packet);
        }
        if (response == MESSAGE) {
            NetMessage msg;
            SocketHandling::getTCPMessage(SocketInfo::getSocket(), msg); // tutaj jest k id id
            switch (msg.message[0]) {
                case 'K':
                    if (msg.message[1] == GameManager::playerId)
                        GameManager::resetRequest("You've been defeated");
                    else {
                        std::string message =
                                std::string(GameManager::playerList[msg.message[1]].data()) + " defeated " +
                                std::string(GameManager::playerList[msg.message[2]].data());
                        this->players.erase(msg.message[1]);
                        GameManager::msgManager.addMsg(message, 3.0f, GameStageGameLevel);
                    }
                    break;
                case 'W':
                    GameManager::resetRequest("You've won the game!");
            }

        }
    } while (response!=NONE);

}

void GameStage1::handleCreate() {
    vector<NetCreatePlayerPacket> pl;
    SocketHandling::recvPlayerCreate(SocketInfo::getSocket(), pl);
    player = SelfPlayer(pl[0],board.getBoardCollision());
    GameManager::playerId = player.id;
    GameManager::playerName = player.playerName.data();
    for (int i=1;i<pl.size();i++)
        players[pl[i].playerId]=Player(pl[i]); //...
    for(auto& id : players){
        GameManager::playerList[id.first] = id.second.playerName;
    }
}



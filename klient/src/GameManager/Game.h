#pragma once

#include <iostream>
#include <list>
#include "../../../networking/src/Packets.h"

#ifndef SIECI_GAME_H
#define SIECI_GAME_H

enum stage{
    GameStageLoginLevel = 0,
    GameStageSelectLevel,
    GameStageQueueLevel,
    GameStageGameLevel,
    GameStageFinishLevel
};

class Game {
public:
    float timeSinceLastPacket = 0;
	virtual void inline input() = 0;
	virtual void updateDrawFrame() = 0;
    virtual int getStage() = 0;
	virtual ~Game() = default;
};

#endif

class MessageHandler{
public:
    void handleMessage(NetMessage msg){
        if( msg.message[0] > 45 &&  msg.message[0] < 90)
            switch(msg.message[0]){
            case 'S': s(); break;
            case 'W': w(); break;
            case 'K': k(msg.message[1], msg.message[2]); break;
            case 'T': t(msg.message[1]); break;
        }
    }

    virtual void s() = 0;
    virtual void w() = 0;
    virtual void k(char kto, char kogo) = 0;
    virtual void t(char ile) = 0;
};

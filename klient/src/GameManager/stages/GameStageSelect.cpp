#include "GameStageSelect.h"
#include "../../Sockets/SocketHandling.hpp"
#include "../../Sockets/SocketInfo.h"
#include "../GameManager.h"


void GameStageSelect::updateDrawFrame() {
    BeginDrawing();
    ClearBackground(WHITE);
    GuiColorPicker(colorPicker,"WYBIERZ KOLOR GRACZA",&color);
    DrawText("Choose player's color",colorPicker.x,colorPicker.y - 50,20,GRAY);
    DrawText("Choose player's name",colorPicker.x + 350,colorPicker.y + - 50,20,GRAY);

    szablon.Draw();
    char* buff = const_cast<char*>(playerName.c_str());
    GuiTextBox(raylib::Rectangle(szablon.x - 60,szablon.y - 100,120,40), buff, 30, true);
    playerName = std::string(buff);
    if (!playerName.empty() && !std::isalpha(playerName.back())) {
        playerName.pop_back();
    }


    DrawRectangleRec(button, LIGHTGRAY);
    if (this->mouseOnButton) DrawRectangleLines((int)button.x, (int)button.y, (int)button.width, (int)button.height, RED);
    else DrawRectangleLines((int)button.x, (int)button.y, (int)button.width , (int)button.height, DARKGRAY);
    DrawText("Join Queue", (int)button.x + 5, (int)button.y + 4, 40, MAROON);
    GameManager::msgManager.drawMsgLog();
    EndDrawing();
}



void GameStageSelect::queue(){
    int odp;
    if(playerName[0] == 0){
        GameManager::msgManager.addMsg("PLAYER NAME CANNOT BE EMPTY!",5.0f,GameStageSelectLevel);
        logShowed = true;
    }
    else{
        //no tutaj masz to wysylac na serwer a potem serwer odbiera tam gdzie bylismy...
        //a potem odsyla wektor caly ze wszystkimi i z niego mozesz zupdatowac wyglad kolejki
        //serwer czeka (teoretycznie bo odbieranie nie istnieje) na NetQueue dla klientów i odbiera je tylko raz
        //albo raczej odbieralby jakby mial receive bo narazie to tyle ze to przewala jak gracz sie pojawi
        NetQueue nq=NetQueue(GameStageSelect::serializeRaylibColor(this->color), this->playerName);
        odp = sendData<NetQueue>(SocketInfo::getSocket(), nq, MSG_NOSIGNAL);
        if(odp > 0){
            GameManager::getInstance().setChangeState();
        }
        else if(odp == -1){
            GameManager::resetRequest("No connection with server!");
        }
    }

}

void GameStageSelect::input() {
    if(enabled){
        GameManager::shouldReset = false;
        GameManager::resetReason = "";
        enabled = false;
    }
    szablon.setColor(color);
    this->szablon.move();
    this->mouseOnButton = (CheckCollisionPointRec(GetMousePosition(), this->button));
    if(playerName.length() > 16) {
        playerName = playerName.substr(0, 16);
    }
    if (this->mouseOnButton) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            queue();
            WaitTime(0.2);
        }
    }

}

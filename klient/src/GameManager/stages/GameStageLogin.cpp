#include "GameStageLogin.h"
#include "../GameManager.h"
#include "../../Sockets/SocketHandling.hpp"
#include "raygui/raygui.h"


void GameStageLogin::updateDrawFrame() {
    BeginDrawing();
    ClearBackground(WHITE);

    DrawText("Provide IP address of the server", window->x / 2.0f - 160, window->y / 2.0f - 320, 20, GRAY);
    DrawRectangleRec(textBox, LIGHTGRAY);
    if (this->mouseOnTextBox) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
    else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(reinterpret_cast<char*>(address.data()), (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

    DrawRectangleRec(button, LIGHTGRAY);
    if (this->mouseOnButton) DrawRectangleLines((int)button.x, (int)button.y, (int)button.width, (int)button.height, RED);
    else DrawRectangleLines((int)button.x, (int)button.y, (int)button.width, (int)button.height, DARKGRAY);
    DrawText("Connect", (int)button.x + 5, (int)button.y + 8, 40, MAROON);
    if(!GameManager::resetReason.empty()){
        raylib::Vector2 width = MeasureTextEx(GetFontDefault(),GameManager::resetReason.c_str(),16.0f,1.0f);
        raylib::Rectangle box = raylib::Rectangle(this->window->x/2.0f - width.x/2.0f,this->window->y/2.0f + 100.0f, width.x + 10.0f, width.y + 10.0f);
        DrawRectangleLinesEx(box,1.0f,GRAY);
        DrawText(GameManager::resetReason.c_str(),box.x+3,box.y+3,16,BLACK);
    }

    GameManager::msgManager.drawMsgLog();

    EndDrawing();
}

void GameStageLogin::connect() {
    if(SocketHandling::connectToServer(address)==1){
        GameManager::resetReason = "";
        GameManager::getInstance().setChangeState();
    }
}

void GameStageLogin::input() {
    this->textBox.width = MeasureTextEx(GetFontDefault(), reinterpret_cast<char *>(address.data()), 40, 4).x + 15;
    this->textBox.x = this->window->x / 2 - this->textBox.width / 2;

    this->mouseOnButton = (CheckCollisionPointRec(GetMousePosition(), this->button));
    mouseOnTextBox = (CheckCollisionPointRec(GetMousePosition(), this->textBox));


    int key = GetCharPressed();
    while (key > 0) {
        if (((key >= 48 && key <= 57) || key == 46) && (this->address.length() <= 14))
            this->address.push_back(key);

        key = GetCharPressed();  // Check next character in the queue
    }
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (!this->address.empty())
            this->address.pop_back();
    }

    if (this->mouseOnButton) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            connect();
        }
    }
}




#include "raylib/raylib-cpp.hpp"
#include <memory>
#include "../Game.h"
#include "../../Board/WindowManager.h"


class GameStageLogin : public Game{
    std::unique_ptr<const raylib::Vector2> window{std::make_unique<const raylib::Vector2>(WindowManager::getInstance().getWindowSize())};
    std::string address = "127.0.0.1";
    raylib::Font& font = WindowManager::getFont();

    raylib::Rectangle textBox = raylib::Rectangle(window->x/2.0f,window->y/2.0f - 200,30,50);
    bool mouseOnTextBox = false;
    bool mouseOnButton = false;
    raylib::Rectangle button = raylib::Rectangle(window->x/2.0f - 85,window->y/2.0f, MeasureTextEx(font,"Connect",40,6).x, textBox.height);

public:
    GameStageLogin() = default;
    void input() override;
    void updateDrawFrame() override;

    void connect();
    inline int getStage() override{
        return GameStageLoginLevel;
    }
};
#include "raylib/raylib-cpp.hpp"
#include <memory>
#include "../Game.h"
#include "../../Board/WindowManager.h"
#include "raygui/raygui.h"
#include "../../Player/Player.h"
#include "../../Player/SelfPlayer.h"
#include "../GameManager.h"


class GameStageSelect : public Game {
    std::unique_ptr<const raylib::Vector2> window{std::make_unique<const raylib::Vector2>(WindowManager::getInstance().getWindowSize())};
    std::string playerName;
    raylib::Color color = raylib::Color(GetRandomValue(0,255),GetRandomValue(0,255),GetRandomValue(0,255));
    raylib::Font& font = WindowManager::getFont();
    RotatingPlayer szablon = RotatingPlayer(window->x/2.0f + 150,window->y/2.0f - 100,40);
    int logDelay = 500;
    bool logShowed = false;
    bool changeReady = false;

    raylib::Rectangle colorPicker = raylib::Rectangle(window->x/2.0f - 300,window->y/2.0f - 200,200,200);
    bool enabled = true;
    bool mouseOnButton = false;

    raylib::Rectangle button = raylib::Rectangle(window->x/2.0f - 110,window->y/2.0f + 200, MeasureTextEx(GetFontDefault(),"Join Queue",40,6).x, 40);

    static inline NetColor serializeRaylibColor(raylib::Color color){
        return {NetColor{color.r,color.g,color.b,color.a}};
    }
public:
    void queue();
    inline GameStageSelect(){
        this->szablon.blocked = true;
        this->szablon.shootingDisabled = false;
    }
    void input() override;
    void updateDrawFrame() override;
    inline int getStage() override{
        return GameStageSelectLevel;
    }
};


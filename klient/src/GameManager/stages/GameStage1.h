#include "raylib/raylib-cpp.hpp"
#include <memory>
#include "../../Board/WindowManager.h"
#include "../../Player/Player.h"
#include "../../Board/Board.h"
#include "../../Player/SelfPlayer.h"
#include "../Game.h"
#include <map>
#include <utility>
#include "../../Sockets/SocketHandling.hpp"
#include "../../Sockets/SocketInfo.h"
#include <list>



class GameStage1 : public Game {
    std::unique_ptr<const raylib::Vector2> window;
    std::map<int, Player> players;

    raylib::Camera2D camera = raylib::Camera2D({ 0,0 }, { 0,0 });

    bool iniciated = false;
    Board board;
    SelfPlayer player{board.getBoardCollision()};
public:
    explicit inline GameStage1(){
        window = std::make_unique<const raylib::Vector2>(WindowManager::getInstance().getWindowSize());
    }
    void inline input() override;
    void updateDrawFrame() override;
    void cameraSetTarget(raylib::Vector2 target);

    void handleCreate();
    void handleUpdate();

    inline int getStage() override{
        return GameStageGameLevel;
    }
};
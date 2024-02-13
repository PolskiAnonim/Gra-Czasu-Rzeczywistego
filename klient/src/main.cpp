#include "GameManager/GameManager.h"


int main(int argc, char* argv[]) {
    GameManager& gameManager = GameManager::getInstance();
    gameManager.run();
}

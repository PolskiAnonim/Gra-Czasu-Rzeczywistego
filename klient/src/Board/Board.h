#pragma once
#include "raylib/raylib-cpp.hpp"


class Board {
	const int cellSize = 100;
	const int boardSize = 20; // ZAWSZE KWADRATOWE

    raylib::Rectangle boardCollision = raylib::Rectangle(0, 0, cellSize * boardSize, cellSize * boardSize);

	raylib::Color color[2] = { BLACK, GRAY };
public:
	void Draw();
    raylib::Rectangle& getBoardCollision();
};
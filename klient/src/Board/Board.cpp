#include "Board.h"

void Board::Draw() {
	for (int y = 0; y != boardSize; y++) {
		for (int x = 0; x != boardSize; x++) {
			DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, color[0]);
		}
	}
}

raylib::Rectangle& Board::getBoardCollision() {
    return this->boardCollision;
}

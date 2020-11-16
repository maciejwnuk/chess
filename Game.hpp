#pragma once

#include "Board.hpp"

#include <string>
#include <allegro5/allegro5.h>

const unsigned int WINDOW_SIZE = 960;
const unsigned int MARGIN = 90;
const unsigned int FONT_SIZE = 32;
const unsigned int SPRITE_SIZE = 90;

const float TILE_SIZE = (WINDOW_SIZE - 2 * MARGIN) / 8;

const std::string ASSETS_PATH = "./assets/";

class Game {
	ALLEGRO_DISPLAY* display;

	Board* board;
public:
	Game();
	int run();
	~Game();
};
#pragma once

#include <string>
#include <optional>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

static const unsigned int WINDOW_SIZE = 960;
static const unsigned int MARGIN = 90;
static const unsigned int FONT_SIZE = 32;
static const unsigned int SPRITE_SIZE = 90;

static const float TILE_SIZE = (WINDOW_SIZE - 2 * MARGIN) / 8;

static const std::string ASSETS_PATH = "./assets/";

#include "Board.hpp"

class Game {
	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* event_queue;
	ALLEGRO_BITMAP* spritesheet;
	ALLEGRO_FONT* font;

	Side turn;

	void setup_allegro();
	void load_assets();

	Board* board;

	void draw_board();
public:
	Game();
	int run();
	~Game();
};
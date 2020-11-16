#pragma once

#include "Piece.hpp"

#include <array>
#include <string>
#include <optional>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

typedef enum {
	None,
	Check,
	Checkmate
} Event;

class Board {
	std::array< std::array<std::optional<Piece>, 8>, 8> squares;
	ALLEGRO_BITMAP* spritesheet;
	ALLEGRO_FONT* font;
public:
	Board(std::string path_to_sprite, std::string path_to_font);
	Event move();
	void draw();
	~Board();
};
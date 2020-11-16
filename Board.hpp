#pragma once

#include "Piece.hpp"

#include <array>
#include <string>
#include <optional>
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
public:
	Board(std::string path);
	Event move();
	void draw();
	~Board();
};
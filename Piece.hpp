#pragma once

#include <allegro5/allegro5.h>

typedef enum {
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King
} Role;

typedef enum {
	White,
	Black
} Side;

class Piece {
	bool is_alive;
	Role role;
	Side side;
	ALLEGRO_BITMAP* sprite;

	friend class Board;
public:
	Piece(Side color, Role type);
};
#include "Piece.hpp"

Piece::Piece(Side color, Role type) {
	is_alive = true;
	side = color;
	role = type;
}
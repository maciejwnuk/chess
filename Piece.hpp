#pragma once

struct Position {
	int x, y;

	bool operator==(const Position& pos) { return (x == pos.x && y == pos.y); }

	Position operator+(const Position& pos) { return Position { x + pos.x, y + pos.y }; }
	Position operator-(const Position& pos) { return Position { x - pos.x, y - pos.y }; }
};

enum Role {
	King,
	Queen,
	Bishop,
	Knight,
	Rook,
	Pawn,
};

enum Side {
	White,
	Black
};

typedef entt::tag<0> Moved;
typedef entt::tag<1> Selected;
// typedef entt::tag<2> Dragged; TODO
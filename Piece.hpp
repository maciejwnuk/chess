#pragma once

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

typedef entt::tag<"movd"_hs> Moved;
typedef entt::tag<"seld"_hs> Selected;
// typedef entt::tag<"drag"_hs> Dragged; TODO

typedef entt::tag<"onb"_hs> OnBoard;
// typedef entt::tag<"chkd"_hs> Checked;
// typedef entt::tag<"matd"_hs> Mated;

// Where to put this??
struct Position {
	int x, y;

	bool operator==(const Position& pos) { return (x == pos.x && y == pos.y); }

	Position operator+(const Position& pos) { return Position { x + pos.x, y + pos.y }; }
	Position operator-(const Position& pos) { return Position { x - pos.x, y - pos.y }; }
};

using PossibleMoves = std::vector<Position>;
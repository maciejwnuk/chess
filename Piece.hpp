#pragma once

#include <allegro5/allegro5.h>

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
typedef entt::tag<"onb"_hs> OnBoard;
typedef entt::tag<"chkd"_hs> Checked;
typedef entt::tag<"cpst"_hs> EnPassantable;

struct Sprite {
	ALLEGRO_BITMAP* ptr;
};


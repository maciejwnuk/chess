#pragma once

#include <string>
#include <optional>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "entt.hpp"

#include "Piece.hpp"

static const unsigned int WINDOW_SIZE = 960;
static const unsigned int MARGIN = 90;
static const unsigned int FONT_SIZE = 32;
static const unsigned int SPRITE_SIZE = 90;

static const float TILE_SIZE = (WINDOW_SIZE - 2 * MARGIN) / 8;

static const std::string ASSETS_PATH = "./assets/";

static const std::tuple<Position, Role, Side> INITIAL_VALUES[32] = {

	// Black first row
	std::make_tuple(Position {1, 1}, Rook, Black),
	std::make_tuple(Position {2, 1}, Knight, Black),
	std::make_tuple(Position {3, 1}, Bishop, Black),
	std::make_tuple(Position {4, 1}, Queen, Black),
	std::make_tuple(Position {5, 1}, King, Black),
	std::make_tuple(Position {6, 1}, Bishop, Black),
	std::make_tuple(Position {7, 1}, Knight, Black),
	std::make_tuple(Position {8, 1}, Rook, Black),

	// Black second row
	std::make_tuple(Position {1, 2}, Pawn, Black),
	std::make_tuple(Position {2, 2}, Pawn, Black),
	std::make_tuple(Position {3, 2}, Pawn, Black),
	std::make_tuple(Position {4, 2}, Pawn, Black),
	std::make_tuple(Position {5, 2}, Pawn, Black),
	std::make_tuple(Position {6, 2}, Pawn, Black),
	std::make_tuple(Position {7, 2}, Pawn, Black),
	std::make_tuple(Position {8, 2}, Pawn, Black),

	// White second row
	std::make_tuple(Position {1, 7}, Pawn, White),
	std::make_tuple(Position {2, 7}, Pawn, White),
	std::make_tuple(Position {3, 7}, Pawn, White),
	std::make_tuple(Position {4, 7}, Pawn, White),
	std::make_tuple(Position {5, 7}, Pawn, White),
	std::make_tuple(Position {6, 7}, Pawn, White),
	std::make_tuple(Position {7, 7}, Pawn, White),
	std::make_tuple(Position {8, 7}, Pawn, White),

	// White first row
	std::make_tuple(Position {1, 8}, Rook, White),
	std::make_tuple(Position {2, 8}, Knight, White),
	std::make_tuple(Position {3, 8}, Bishop, White),
	std::make_tuple(Position {4, 8}, Queen, White),
	std::make_tuple(Position {5, 8}, King, White),
	std::make_tuple(Position {6, 8}, Bishop, White),
	std::make_tuple(Position {7, 8}, Knight, White),
	std::make_tuple(Position {8, 8}, Rook, White),
};

class Game {
	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* event_queue;
	ALLEGRO_BITMAP* spritesheet;
	ALLEGRO_FONT* font;

	Side turn;

	entt::registry registry;

	// Keep track of kings, no need to look for them
	entt::entity kings[2];
	bool is_check;

	void setup_allegro();
	void load_assets();

	void setup_pieces();

	void draw_board();
	void draw_notation();
	void draw_pieces();
	// void draw_helpers(); TODO

	std::optional<entt::entity> get_selected_piece();
	std::optional<entt::entity> get_piece_by_tile_pos(Position pos);

	std::vector<Position> get_possible_moves(entt::entity entity);

	bool is_piece_endangered(entt::entity entity);

	void make_turn();
public:
	Game();
	int run();
	~Game();
};

typedef struct {
	ALLEGRO_BITMAP* ptr;
} Sprite;

bool is_position_in_board(Position pos);
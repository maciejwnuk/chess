#pragma once

#include "entt.hpp"

struct Position {
	int x, y;

	bool operator==(const Position& pos) { return (x == pos.x && y == pos.y); }

	Position operator+(const Position& pos) { return Position { x + pos.x, y + pos.y }; }
	Position operator-(const Position& pos) { return Position { x - pos.x, y - pos.y }; }
};

bool is_position_in_board(Position pos);

using PossibleMoves = std::vector<Position>;

#include "Piece.hpp"

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

class Board {
	entt::registry pieces;

	entt::entity kings[2];

	Sprite queen_sprites[2]; // For promotion

	void calc_possible_moves(entt::registry &registry, entt::entity _);

	void check_for_checks(entt::registry &registry, entt::entity _);

	void promote(entt::registry &registry, entt::entity _);
public:
	Board(ALLEGRO_BITMAP* spritesheet);

	entt::registry* get_pieces_ptr();
	std::optional<entt::entity> get_piece_by_pos(Position pos);

	Side get_pieces_side(entt::entity piece);
	Role get_pieces_role(entt::entity piece);
	Position get_pieces_position(entt::entity piece);

	bool is_piece_selected(entt::entity piece);
	std::optional<entt::entity> get_selected_piece();
	void change_selection(entt::entity piece);
	void clear_selection();
	void clear_enpassant_for(Side side);

	bool try_to_move(entt::entity piece, Position pos);
	bool try_to_castle(entt::entity king, Position difference);
	bool try_to_take(entt::entity taking, entt::entity taken);
	bool try_to_take_enpassant(entt::entity taking, entt::entity taken);

	bool is_king_mated(Side side);
};
#include "Board.hpp"
#include "Game.hpp"

#include <iostream>

bool is_position_in_board(Position pos) {
	return (pos.x > 0 && pos.x < 9 && pos.y > 0 && pos.y < 9);
}

Board::Board(ALLEGRO_BITMAP* spritesheet) {
	pieces.clear();

	queen_sprites[0] = { al_create_sub_bitmap(spritesheet, SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE) };
	queen_sprites[1] = { al_create_sub_bitmap(spritesheet, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) };

	Sprite sprites[] = {
		{ al_create_sub_bitmap(spritesheet, 0, 0, SPRITE_SIZE, SPRITE_SIZE) },                          // white_king
		queen_sprites[0],	                															// white_queen
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 2, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_bishop
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 3, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_knight
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 4, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_rook
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 5, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_pawn
		{ al_create_sub_bitmap(spritesheet, 0, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },                // black_king
		queen_sprites[1],																			    // black_queen
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 2, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_bishop
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 3, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_knight
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 4, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_rook
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 5, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_pawn
	};

	// Create entities and emplace components
	for (int i = 0; i < 32; i++) {
		auto entity = pieces.create();

		pieces.emplace<Position>(entity, std::get<0>(INITIAL_VALUES[i]));
		pieces.emplace<Role>(entity, std::get<1>(INITIAL_VALUES[i]));
		pieces.emplace<Side>(entity, std::get<2>(INITIAL_VALUES[i]));
		pieces.emplace<Sprite>(entity, sprites[std::get<1>(INITIAL_VALUES[i]) + std::get<2>(INITIAL_VALUES[i]) * 6]);
		pieces.emplace<PossibleMoves>(entity);
		pieces.emplace<OnBoard>(entity);

		if (std::get<1>(INITIAL_VALUES[i]) == King) 
			kings[std::get<2>(INITIAL_VALUES[i])] = entity;
	}

	calc_possible_moves(pieces, entt::null);

	pieces.on_update<Position>().connect<&Board::calc_possible_moves>(*this);
	pieces.on_update<Position>().connect<&Board::check_for_checks>(*this);
	pieces.on_update<Position>().connect<&Board::promote>(*this);
}

entt::registry* Board::get_pieces_ptr() {
	return &pieces;
}

std::optional<entt::entity> Board::get_piece_by_pos(Position pos) {
	auto view = pieces.view<Position, OnBoard>();

	for (auto entity: view) {
		if (view.get<Position>(entity) == pos) {
			return entity;
		}
	}

	return {};
}

Side Board::get_pieces_side(entt::entity piece) {
	return pieces.get<Side>(piece);
}

Role Board::get_pieces_role(entt::entity piece) {
	return pieces.get<Role>(piece);
}

Position Board::get_pieces_position(entt::entity piece) {
	return pieces.get<Position>(piece);
}

bool Board::is_piece_selected(entt::entity piece) {
	return pieces.has<Selected>(piece);
}

std::optional<entt::entity> Board::get_selected_piece() {
	auto view = pieces.view<Selected>();

	for (auto entity: view) {
		return entity;
	}

	return {};
}

void Board::change_selection(entt::entity piece) {
	pieces.emplace_or_replace<Selected>(piece);
}

void Board::clear_selection() {
	pieces.clear<Selected>();
}

void Board::clear_enpassant_for(Side side) {
	auto view = pieces.view<EnPassantable, Side>();

	for (auto entity: view) {
		if (pieces.get<Side>(entity) == side)
			pieces.remove<EnPassantable>(entity);
	}
}

bool Board::try_to_move(entt::entity piece, Position pos) {
	auto moves = pieces.get<PossibleMoves>(piece);
	auto turn = pieces.get<Side>(piece);

	// If position is in possible moves
	if (std::count(moves.begin(), moves.end(), pos)) {
		Position old_pos = pieces.get<Position>(piece);
		pieces.emplace_or_replace<Position>(piece, pos);

		// If move is checking own king - is illegal
		if (pieces.has<Checked>(kings[turn]))
			pieces.emplace_or_replace<Position>(piece, old_pos);
		else {
			pieces.emplace_or_replace<Moved>(piece);

			// Enpassantable anchor
			if (pieces.get<Role>(piece) == Pawn) {
				Position difference = old_pos - pos;

				if (difference == Position { 0, 2 } || difference == Position { 0, -2 })
					pieces.emplace<EnPassantable>(piece);
			}

			return true;
		}
	}

	return false;
}

bool Board::try_to_castle(entt::entity king, Position difference) {
	// Check if king moved
	if (pieces.has<Moved>(king))
		return false;

	// First check if opponents pieces look between castling zone
	auto direction = (difference.x == 2) ? Position { 1, 0 } : Position { -1, 0 };
    auto pos = pieces.get<Position>(king);
    auto opposing_side = (pieces.get<Side>(king)) ? White : Black;

    while (true) {
    	auto view = pieces.view<Side, PossibleMoves, OnBoard>();

		for (auto entity: view) {
		    if (pieces.get<Side>(entity) == opposing_side) {
		    	auto moves = pieces.get<PossibleMoves>(entity);

				if (std::count(moves.begin(), moves.end(), pos)) {
					return false;
				}
		    }
		}

		pos = pos + direction;

		if (!is_position_in_board(pos))
			break;
    }

    pos = pieces.get<Position>(king);

    // Check for rook
    while (true) {
    	pos = pos + direction;

    	if (auto piece = get_piece_by_pos(pos)) {
    		auto role = pieces.get<Role>(*piece);
    		bool has_moved = pieces.has<Moved>(*piece);

    		// Do castling if true
    		if (role == Rook && !has_moved) {
    			pos = pieces.get<Position>(king);

    			Position rook_pos = pos + Position { direction.x, 0 };
    			Position king_pos = pos + Position { 2 * direction.x, 0 };

    			pieces.replace<Position>(*piece, rook_pos);
    			pieces.replace<Position>(king, king_pos);

    			return true;
    		} else
    			break;
    	}

		if (!is_position_in_board(pos))
			break;
    }

	return false;
}

bool Board::try_to_take(entt::entity taking, entt::entity taken) {
	auto moves = pieces.get<PossibleMoves>(taking);
	auto turn = pieces.get<Side>(taking);
	auto pos = pieces.get<Position>(taken);

	// If position is in possible moves
	if (std::count(moves.begin(), moves.end(), pos)) {
		Position old_pos = pieces.get<Position>(taking);

		pieces.remove<OnBoard>(taken);
		pieces.emplace_or_replace<Position>(taking, pos);

		// If take is checking own king - is illegal
		if (pieces.has<Checked>(kings[turn])) {
			pieces.emplace_or_replace<OnBoard>(taken);
			pieces.emplace_or_replace<Position>(taking, old_pos);
		} else {
			pieces.emplace_or_replace<Moved>(taking);

			return true;
		}
	}

	return false;
}

bool Board::try_to_take_enpassant(entt::entity taking, entt::entity taken) {
	// If thats true just take
	if (pieces.has<EnPassantable>(taken)) {
		auto side = pieces.get<Side>(taking);
		auto difference = (side) ? Position { 0, 1 } : Position { 0, -1 };
		auto pos = pieces.get<Position>(taken) + difference;

		Position old_pos = pieces.get<Position>(taking);

		pieces.remove<OnBoard>(taken);
		pieces.emplace_or_replace<Position>(taking, pos);

		// If take is checking own king - is illegal
		if (pieces.has<Checked>(kings[side])) {
			pieces.emplace_or_replace<OnBoard>(taken);
			pieces.emplace_or_replace<Position>(taking, old_pos);
		} else {
			pieces.emplace_or_replace<Moved>(taking);

			return true;
		}
	}

	return false;
}

void Board::calc_possible_moves(entt::registry &registry, entt::entity _) {
	auto view = registry.view<OnBoard>();

	for(auto entity: view) {
    	auto origin = registry.get<Position>(entity);
		auto role = registry.get<Role>(entity);
		auto side = registry.get<Side>(entity);

		Position position;
		PossibleMoves possibilities = {};

		switch (role) {
			case King: {
				for (int i = -1; i < 2; ++i) {
					for (int j = -1; j < 2; ++j) {
						if (i == 0 && j == 0)
							continue;

						position = origin + Position { i, j };

						if (is_position_in_board(position)) {
							if (auto piece = get_piece_by_pos(position)) {
								if (side != registry.get<Side>(*piece)) {
									possibilities.push_back(position);
								}
							} else {
								possibilities.push_back(position);
							}
						}
					}
				}

				break;
			}

			case Queen: {
				for (int i = -1; i < 2; ++i) {
					for (int j = -1; j < 2; ++j) {
						if (i == 0 && j == 0)
							continue;

						for (int k = 1;; ++k) {
							position = origin + Position { k * i, k * j };

							if (is_position_in_board(position)) {
								if (auto piece = get_piece_by_pos(position)) {
									if (side != registry.get<Side>(*piece)) {
										possibilities.push_back(position);
									}

									break;
								} else {
									possibilities.push_back(position);
								}
							} else {
								break;
							}
						}
					}
				}

				break;
			}

			case Bishop: {
				for (int i = -1; i < 2; i += 2) {
					for (int j = -1; j < 2; j += 2) {
						for (int k = 1;; ++k) {
							position = origin + Position { k * i, k * j };

							if (is_position_in_board(position)) {
								if (auto piece = get_piece_by_pos(position)) {
									if (side != registry.get<Side>(*piece)) {
										possibilities.push_back(position);
									}

									break;
								} else {
									possibilities.push_back(position);
								}
							} else {
								break;
							}
						}
					}
				}

				break;
			}

			case Knight: {
				for (int i = -2; i < 3; ++i) {
					if (i == 0)
						continue;

					if (i % 2) {
						for (int j = -2; j < 3; j += 4) {
							position = origin + Position { i, j };

							if (is_position_in_board(position)) {
								if (auto piece = get_piece_by_pos(position)) {
									if (side != registry.get<Side>(*piece)) {
										possibilities.push_back(position);
									}
								} else {
									possibilities.push_back(position);
								}
							}
						}
					} else {
						for (int j = -1; j < 2; j += 2) {
							position = origin + Position { i, j };

							if (is_position_in_board(position)) {
								if (auto piece = get_piece_by_pos(position)) {
									if (side != registry.get<Side>(*piece)) {
										possibilities.push_back(position);
									}
								} else {
									possibilities.push_back(position);
								}
							}
						}
					}
				}

				break;
			}

			case Rook: {
				for (int i = -1; i < 2; ++i) {
					for (int j = -1; j < 2; ++j) {
						if (i == j || i == -j)
							continue;

						for (int k = 1;; ++k) {
							position = origin + Position { k * i, k * j };

							if (is_position_in_board(position)) {
								if (auto piece = get_piece_by_pos(position)) {
									if (side != registry.get<Side>(*piece)) {
										possibilities.push_back(position);
									}

									break;
								} else {
									possibilities.push_back(position);
								}
							} else {
								break;
							}
						}
					}
				}

				break;
			}

			case Pawn: {
				// One forward
				position = origin + ((side) ? Position { 0, 1 } : Position { 0, -1 });

				{
					auto piece = get_piece_by_pos(position);

					if (!piece) {
						possibilities.push_back(position);
					}
				}

				// Two forward
				if (!registry.has<Moved>(entity)) {
					position = origin + ((side) ? Position { 0, 2 } : Position { 0, -2 });

					auto piece = get_piece_by_pos(position);

					if (!piece) {
						possibilities.push_back(position);
					}
				}

				// Take diagonally
				for (int i = -1; i < 2; i += 2) {
					position = origin + ((side) ? Position { i, 1 } : Position { i, -1 });

					if (auto piece = get_piece_by_pos(position)) {
						possibilities.push_back(position);
					}
				}

				break;
			}

		}

	    registry.emplace_or_replace<PossibleMoves>(entity, possibilities);
	}
}

void Board::check_for_checks(entt::registry &registry, entt::entity _) {
	registry.clear<Checked>();

	for (Side side = White;; side = Black) {
		auto king = kings[side];
	    auto pos = registry.get<Position>(king);
	    auto opposing_side = (registry.get<Side>(king)) ? White : Black;

		auto view = registry.view<Side, PossibleMoves, OnBoard>();

		for (auto entity: view) {
		    if (registry.get<Side>(entity) == opposing_side) {
		    	auto moves = registry.get<PossibleMoves>(entity);

				if (std::count(moves.begin(), moves.end(), pos)) {
					registry.emplace_or_replace<Checked>(king);
				}
		    }
		}

		if (side == Black)
			break;
	}
}

void Board::promote(entt::registry &registry, entt::entity _) {
	auto view = registry.view<Side, Role, Position, OnBoard>();

	for (auto entity: view) {
		auto pos = registry.get<Position>(entity);
		auto role = registry.get<Role>(entity);
		auto side = registry.get<Side>(entity);

	    if (role == Pawn && (pos.y == 1 || pos.y == 8)) {
	    	registry.emplace_or_replace<Role>(entity, Queen);
	    	registry.emplace_or_replace<Sprite>(entity, queen_sprites[side]);
	    }
	}
}

bool Board::is_king_mated(Side side) {
	auto view = pieces.view<Position, Side, PossibleMoves, OnBoard>();

	for (auto entity: view) {
	    if (pieces.get<Side>(entity) == side) {
	    	auto moves = pieces.get<PossibleMoves>(entity);
	    	auto old_pos = pieces.get<Position>(entity);

	    	for (auto pos: moves) {
	    		auto piece = get_piece_by_pos(pos);

	    		if (piece) {
	    			pieces.remove<OnBoard>(*piece);
	    		}

	    		pieces.emplace_or_replace<Position>(entity, pos);

	    		if (!pieces.has<Checked>(kings[side])) {
	    			pieces.emplace_or_replace<Position>(entity, old_pos);

		    		if (piece) {
		    			pieces.emplace_or_replace<OnBoard>(*piece);
		    		}

	    			return false;
	    		}

	    		if (piece) {
	    			pieces.emplace_or_replace<OnBoard>(*piece);
	    		}

	    		pieces.emplace_or_replace<Position>(entity, old_pos);
	    	}
	    }
	}

    return true;
}
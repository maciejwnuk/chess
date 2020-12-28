#include "Game.hpp"

#include <iostream>
#include <algorithm>

Game::Game() {
	registry.clear();

	setup_allegro();
	load_assets();

	setup_pieces();

	turn = White;
	is_check = false;
}

void Game::setup_allegro() {
	// Initialize allegro
	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();

	// Make display
	display = al_create_display(WINDOW_SIZE, WINDOW_SIZE);
	al_set_target_backbuffer(display);

	// Antialiasing
	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

	// Setup peripherals
	if(!al_install_mouse()) {
		std::cout << "Unable to init mouse" << std::endl;
	}

	// Create event queue
	event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());
}

void Game::setup_pieces() {
	Sprite sprites[] = {
		{ al_create_sub_bitmap(spritesheet, 0, 0, SPRITE_SIZE, SPRITE_SIZE) },                          // white_king
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE) },                // white_queen
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 2, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_bishop
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 3, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_knight
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 4, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_rook
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 5, 0, SPRITE_SIZE, SPRITE_SIZE) },            // white_pawn
		{ al_create_sub_bitmap(spritesheet, 0, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },                // black_king
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },      // black_queen
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 2, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_bishop
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 3, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_knight
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 4, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_rook
		{ al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 5, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE) },  // black_pawn
	};

	// Create entities and emplace components
	for (int i = 0; i < 32; i++) {
		auto entity = registry.create();

		registry.emplace<Position>(entity, std::get<0>(INITIAL_VALUES[i]));
		registry.emplace<Role>(entity, std::get<1>(INITIAL_VALUES[i]));
		registry.emplace<Side>(entity, std::get<2>(INITIAL_VALUES[i]));
		registry.emplace<Sprite>(entity, sprites[std::get<1>(INITIAL_VALUES[i]) + std::get<2>(INITIAL_VALUES[i]) * 6]);

		if (std::get<1>(INITIAL_VALUES[i]) == King) {
			kings[std::get<2>(INITIAL_VALUES[i])] = entity;
		}
	}
}

void Game::load_assets() {
	spritesheet = al_load_bitmap((ASSETS_PATH + "sprite.png").c_str()); 
	font = al_load_ttf_font((ASSETS_PATH + "font.ttf").c_str(), FONT_SIZE, 0);
}

int Game::run() {
	ALLEGRO_EVENT event;
	ALLEGRO_MOUSE_STATE mstate;

	bool done = false;
	bool redraw = true;

	// Game loop
	while (!done) {
		al_wait_for_event(event_queue, &event);

		switch (event.type) {
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
				al_get_mouse_state(&mstate);

				// Calculate tile position
				int x = (mstate.x - MARGIN) / TILE_SIZE + 1;
				int y = (mstate.y - MARGIN) / TILE_SIZE + 1;

				Position tile_pos = Position { x, y };

				bool moved = false;

				// If there is previously selected piece
				auto selected = get_selected_piece();

				if (selected) {
					registry.clear<Selected>();

					// If there is a piece on clicked tile
					if (auto piece = get_piece_by_tile_pos(tile_pos)) {
						auto side = registry.get<Side>(*piece);

						// If piece has the same color as turn then select it
						if (turn == side) {
							registry.emplace<Selected>(*piece);
						// Otherwise check if it can be taken
						} else {
							auto moves = get_possible_moves(*selected);

							if (std::count(moves.begin(), moves.end(), tile_pos)) {
								registry.destroy(*piece);
								registry.replace<Position>(*selected, tile_pos);
								moved = true;
							}
						}
					// Or just try to move
					} else {
						auto moves = get_possible_moves(*selected);

						if (std::count(moves.begin(), moves.end(), tile_pos)) {
							registry.replace<Position>(*selected, tile_pos);
							moved = true;
						}
					}
				// If not try to select piece (if there is any)
				} else if (auto piece = get_piece_by_tile_pos(tile_pos)) {
					auto side = registry.get<Side>(*piece);

					// If piece has the same color as turn then select it
					if (turn == side) {
						registry.emplace<Selected>(*piece);
					}
				} 

				if (moved) {
					make_turn();

					// Check for checks
					auto pos = registry.get<Position>(kings[turn]);
					auto moves = get_possible_moves(*selected);

					if (std::count(moves.begin(), moves.end(), pos)) {
						std::cout << "Check!" << std::endl;
					}

					moved = false;

					// TODO: Player cannot check own king by self
					// TODO: Mate
				}

				// Redraw every click ??
				redraw = true;	

				break;
			}

			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				done = true;
			break;
		}

		if (redraw) {

			draw_board();
			draw_pieces();
			// draw_helpers();
			
			al_flip_display();

			redraw = false;
		}
	}

	return 0;
}

void Game::draw_board() {
	// Background
	al_clear_to_color(al_map_rgb(35, 18, 8));

	// Tiles
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			auto color = ((row + col) % 2) ? al_map_rgb(128, 57, 30) : al_map_rgb(252, 234, 185); 

			al_draw_filled_rectangle(
				MARGIN + TILE_SIZE * col,
				MARGIN + TILE_SIZE * row,
				TILE_SIZE + MARGIN + TILE_SIZE * col,
				TILE_SIZE + MARGIN + TILE_SIZE * row,
				color
			);
		}
	}

	draw_notation();
}

void Game::draw_notation() {
	// Columns
	for (int i = 0; i < 8; ++i) {
		al_draw_glyph(
			font, 
			al_map_rgb(255, 255, 255), 
			(MARGIN - al_get_glyph_width(font, '1' + i)) / 2, 
			WINDOW_SIZE - MARGIN - (TILE_SIZE + FONT_SIZE) / 2 - TILE_SIZE * i,
			'1' + i
		);
		al_draw_glyph(
			font, 
			al_map_rgb(255, 255, 255), 
			WINDOW_SIZE - (MARGIN + al_get_glyph_width(font, '1' + i)) / 2, 
			WINDOW_SIZE - MARGIN - (TILE_SIZE + FONT_SIZE) / 2 - TILE_SIZE * i,
			'1' + i
		);
	}

	// Rows
	for (char c = 'a'; c < 'i'; ++c) {
		al_draw_glyph(
			font, 
			al_map_rgb(255, 255, 255), 
			MARGIN + TILE_SIZE / 2 + TILE_SIZE * (c - 'a') - al_get_glyph_width(font, c) / 2, 
			(MARGIN - FONT_SIZE) / 2, 
			c
		);
		al_draw_glyph(
			font, 
			al_map_rgb(255, 255, 255), 
			MARGIN + TILE_SIZE / 2 + TILE_SIZE * (c - 'a') - al_get_glyph_width(font, c) / 2, 
			WINDOW_SIZE - MARGIN + FONT_SIZE / 2, 
			c
		);
	}
}

void Game::draw_pieces() {
	auto view = registry.view<Position, Sprite>();

	for (auto entity: view) {
		auto pos = view.get<Position>(entity);
		auto sprite = view.get<Sprite>(entity);

		// Highlight selected
		if (registry.has<Selected>(entity)) {
			al_draw_filled_circle(
				MARGIN + TILE_SIZE * (pos.x - 1) + TILE_SIZE / 2, 
				MARGIN + TILE_SIZE * (pos.y - 1) + TILE_SIZE / 2, 
				TILE_SIZE / 4,
				al_map_rgba(255, 255, 255, 200)
			);
		}

		// Draw actual pieces
		al_draw_scaled_bitmap(
			sprite.ptr,
			0, 
			0, 
			SPRITE_SIZE, 
			SPRITE_SIZE,
			MARGIN + TILE_SIZE * (pos.x - 1), 
			MARGIN + TILE_SIZE * (pos.y - 1), 
			TILE_SIZE, 
			TILE_SIZE, 
			0
		);
	}
}

std::optional<entt::entity> Game::get_piece_by_tile_pos(Position pos) {
	auto view = registry.view<Position>();

	for (auto entity: view) {
		if (view.get<Position>(entity) == pos) {
			return entity;
		}
	}

	return {};
}

std::optional<entt::entity> Game::get_selected_piece() {
	auto view = registry.view<Selected>();

	for (auto entity: view) {
		return entity;
	}

	return {};
}

std::vector<Position> Game::get_possible_moves(entt::entity entity) {
	auto origin = registry.get<Position>(entity);
	auto role = registry.get<Role>(entity);
	auto side = registry.get<Side>(entity);

	Position position;
	std::vector<Position> possibilities = {};

	switch (role) {
		case King: {
			for (int i = -1; i < 2; ++i) {
				for (int j = -1; j < 2; ++j) {
					if (i == 0 && j == 0)
						continue;

					position = origin + Position { i, j };

					if (is_position_in_board(position)) {
						if (auto piece = get_piece_by_tile_pos(position)) {
							if (side != registry.get<Side>(*piece)) {
								possibilities.push_back(position);
							}
						} else {
							possibilities.push_back(position);
						}
					}
				}
			}

			// TODO: Castling

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
							if (auto piece = get_piece_by_tile_pos(position)) {
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
							if (auto piece = get_piece_by_tile_pos(position)) {
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
							if (auto piece = get_piece_by_tile_pos(position)) {
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
							if (auto piece = get_piece_by_tile_pos(position)) {
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
							if (auto piece = get_piece_by_tile_pos(position)) {
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
				auto piece = get_piece_by_tile_pos(position);

				if (!piece) {
					possibilities.push_back(position);
				}
			}

			// Two forward
			if (!registry.has<Moved>(entity)) {
				position = origin + ((side) ? Position { 0, 2 } : Position { 0, -2 });

				auto piece = get_piece_by_tile_pos(position);

				if (!piece) {
					possibilities.push_back(position);
				}
			}

			// Take diagonally
			for (int i = -1; i < 2; i += 2) {
				position = origin + ((side) ? Position { i, 1 } : Position { i, -1 });

				if (auto piece = get_piece_by_tile_pos(position)) {
					possibilities.push_back(position);
				}
			}

			// TODO: Take en passant

			break;
		}

	}

	return possibilities;
}

void Game::make_turn() {
	turn = (turn) ? White : Black;
}

// bool Game::is_tile_occupied(Position pos) {
//     auto view = registry.view<Position>();

//     for (auto entity: view) {
//         if (view.get<Position>(entity) == pos) {
//             return true;
//         }
//     }

//     return false;
// }

Game::~Game() {
	al_uninstall_mouse();
	al_destroy_font(font);
	al_destroy_bitmap(spritesheet);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
}

// Helpers
bool is_position_in_board(Position pos) {
	return (pos.x > 0 && pos.x < 9 && pos.y > 0 && pos.y < 9);
}
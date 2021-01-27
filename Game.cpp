#include "Game.hpp"

#include <iostream>
#include <algorithm>

Game::Game() {
	setup_allegro();
	load_assets();

	board = new Board(spritesheet);

	turn = White;
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

void Game::load_assets() {
	spritesheet = al_load_bitmap((ASSETS_PATH + "sprite.png").c_str()); 
	font = al_load_ttf_font((ASSETS_PATH + "font.ttf").c_str(), FONT_SIZE, 0);
}

int Game::run() {
	// Some helpers to get through
	ALLEGRO_EVENT event;
	ALLEGRO_MOUSE_STATE mstate;

	bool done = false;
	bool redraw = true;
	bool turned = false;

	Position tile_pos;

	// Game loop
	while (!done) {
		al_wait_for_event(event_queue, &event);

		switch (event.type) {
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
				al_get_mouse_state(&mstate);

				// Calculate tile position
				int x = (mstate.x - MARGIN) / TILE_SIZE + 1;
				int y = (mstate.y - MARGIN) / TILE_SIZE + 1;

				tile_pos = Position { x, y };

				// If there is a selected previously piece
				if (auto selected = board->get_selected_piece()) {
					// Clean
					board->clear_selection();

					// If there is a piece on clicked tile
					if (auto piece = board->get_piece_by_pos(tile_pos)) {

						// If piece has the same color as turn then select it
						if (turn == board->get_pieces_side(*piece))
							board->change_selection(*piece);
						// Otherwise check if it can be taken
						else
							turned = board->try_to_take(*selected, *piece);

					// Maybe it's en passant?
					} else if (board->get_pieces_role(*selected) == Pawn) {
						Position direction = Position { 0, ((turn) ? -1 : 1) };
						Position behind_pos = tile_pos + direction;

						if (auto piece = board->get_piece_by_pos(behind_pos)) {
							if (board->get_pieces_role(*piece) == Pawn && board->get_pieces_side(*piece) != turn) 
								turned = board->try_to_take_enpassant(*selected, *piece);
							else
								turned = board->try_to_move(*selected, tile_pos);
						} else 
							turned = board->try_to_move(*selected, tile_pos);

					// Maybe it's castling?
					} else if (board->get_pieces_role(*selected) == King) {
						Position difference = tile_pos - board->get_pieces_position(*selected);

						if (difference == Position { -2, 0 } || difference == Position { 2, 0 })
							turned = board->try_to_castle(*selected, difference);
						else
							turned = board->try_to_move(*selected, tile_pos);

					// Or just try to move
					} else 
						turned = board->try_to_move(*selected, tile_pos);
					
				// If not try to select piece (if there is any)
				} else if (auto piece = board->get_piece_by_pos(tile_pos)) {
					if (turn == board->get_pieces_side(*piece))
						board->change_selection(*piece);
				}

				redraw = true;	

				break;
			}

			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				done = true;
			break;
		}

		if (redraw) {
			draw_board();
			
			al_flip_display();

			redraw = false;
		}

		if (turned) {
			turn = (turn) ? White : Black;

			board->clear_enpassant_for(turn);

			if (board->is_king_mated(turn)) {
				auto mate_side = (turn) ? "Black" : "White";
				auto opposing_side = (turn) ? "White" : "Black";

				std::cout << mate_side << " king is mated." << std::endl
						  << opposing_side << " wins!" << std::endl;
			}

			turned = false;
		}
	}

	return 1;
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

	// Notation
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

	auto view = board->get_pieces_ptr()->view<Position, Sprite, OnBoard>();

	for (auto piece: view) {
		auto pos = view.get<Position>(piece);
		auto sprite = view.get<Sprite>(piece);

		// Highlight selected
		if (board->is_piece_selected(piece)) {
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

Game::~Game() {
	al_uninstall_mouse();
	al_destroy_font(font);
	al_destroy_bitmap(spritesheet);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
}
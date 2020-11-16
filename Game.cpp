#include "Game.hpp"

#include <iostream>

Game::Game() {
    al_init();
    al_init_ttf_addon();
    al_init_font_addon();
    al_init_image_addon();
    al_init_primitives_addon();

    display = al_create_display(WINDOW_SIZE, WINDOW_SIZE);

    al_set_target_backbuffer(display);

    // Antialiasing
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    font = al_load_ttf_font((ASSETS_PATH + "font.ttf").c_str(), FONT_SIZE, 0);

	board = new Board(ASSETS_PATH + "sprite.png");
}

int Game::run() {
    bool done = false;
    bool redraw = true;

	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);

    ALLEGRO_EVENT event;
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_start_timer(timer);

    // Game loop
    while (!done) {
	    al_wait_for_event(queue, &event);

	    switch (event.type) {
	        case ALLEGRO_EVENT_TIMER:
	            // game logic goes here.
	            redraw = true;
	        break;

	        case ALLEGRO_EVENT_DISPLAY_CLOSE:
	            done = true;
	        break;
	    }

	    if (redraw) {
	        // Background
	        al_clear_to_color(al_map_rgb(35, 18, 8));

	        // Chessboard
	        board->draw();

	        // Letters to support notational thinking
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
	        
	        al_flip_display();

	        redraw = false;
    	}
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

Game::~Game() {
    al_destroy_font(font);
    al_destroy_display(display);
}
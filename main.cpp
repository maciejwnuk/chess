#include <string> 
#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

const uint WINDOW_SIZE = 960;
const uint MARGIN = 90;
const uint FONT_SIZE = 32;
const uint SPRITED_SIZE = 90;

const float TILE_SIZE = (WINDOW_SIZE - 2 * MARGIN) / 8;

const std::string ASSETS_PATH = "./assets/";

int main(int argc, char **argv) 
{
    // Game constructor
    if (!al_init()) {
        std::cout << "Couldn't initialize allegro" << std::endl;
        return 1;
    }

    al_init_ttf_addon();
    al_init_font_addon();
    al_init_image_addon();
    al_init_primitives_addon();

    ALLEGRO_FONT* font = al_load_ttf_font((ASSETS_PATH + "font.ttf").c_str(), FONT_SIZE, 0);

    ALLEGRO_BITMAP* sprite = al_load_bitmap((ASSETS_PATH + "sprite.png").c_str());
    ALLEGRO_BITMAP* white_king = al_create_sub_bitmap(sprite, 0, 0, SPRITED_SIZE, SPRITED_SIZE);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    ALLEGRO_DISPLAY* disp = al_create_display(WINDOW_SIZE, WINDOW_SIZE);
    al_set_target_backbuffer(disp);

    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);

    // Game event loop
    while (!done) {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // game logic goes here.
                redraw = true;
            break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
            break;
        }

        if(redraw && al_is_event_queue_empty(queue))
        {
            // Background
            al_clear_to_color(al_map_rgb(35, 18, 8));

            // Chessboard
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    auto color = ((i + j) % 2) ? al_map_rgb(128, 57, 30) : al_map_rgb(252, 234, 185); 
                    al_draw_filled_rectangle(
                        MARGIN + TILE_SIZE * i,
                        MARGIN + TILE_SIZE * j,
                        TILE_SIZE + MARGIN + TILE_SIZE * i,
                        TILE_SIZE + MARGIN + TILE_SIZE * j,
                        color
                    );
                }
            }

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

            // Pieces (Piece.draw_at(Position))
            al_draw_scaled_bitmap(
            	white_king,
                0, 
                0, 
                SPRITED_SIZE, 
                SPRITED_SIZE,
                MARGIN + TILE_SIZE * 4, 
                MARGIN + TILE_SIZE * 7, 
                TILE_SIZE, 
                TILE_SIZE, 
                0
            );
            
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_bitmap(white_king);
    al_destroy_bitmap(sprite);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
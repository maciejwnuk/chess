#include "Board.hpp"
#include "Game.hpp"

Board::Board(std::string path) {
    spritesheet = al_load_bitmap(path.c_str());

    // Create reference objects
	Piece white_king(White, King);
	white_king.sprite = al_create_sub_bitmap(spritesheet, 0, 0, SPRITE_SIZE, SPRITE_SIZE);
	Piece black_king(Black, King);
	black_king.sprite = al_create_sub_bitmap(spritesheet, 0, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

	Piece white_queen(White, Queen);
	white_queen.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE);
	Piece black_queen(Black, Queen);
	black_queen.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

	Piece white_bishop(White, Bishop);
	white_bishop.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 2, 0, SPRITE_SIZE, SPRITE_SIZE);
	Piece black_bishop(Black, Bishop);
	black_bishop.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 2, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

	Piece white_knight(White, Knight);
	white_knight.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 3, 0, SPRITE_SIZE, SPRITE_SIZE);
	Piece black_knight(Black, Knight);
	black_knight.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 3, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

	Piece white_rook(White, Rook);
	white_rook.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 4, 0, SPRITE_SIZE, SPRITE_SIZE);
	Piece black_rook(Black, Rook);
	black_rook.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 4, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

	Piece white_pawn(White, Pawn);
	white_pawn.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 5, 0, SPRITE_SIZE, SPRITE_SIZE);
	Piece black_pawn(Black, Pawn);
	black_pawn.sprite = al_create_sub_bitmap(spritesheet, SPRITE_SIZE * 5, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

	// Insert the pieces
	squares[0][0].emplace(black_rook);
	squares[0][1].emplace(black_knight);
	squares[0][2].emplace(black_bishop);
	squares[0][3].emplace(black_queen);
	squares[0][4].emplace(black_king);
	squares[0][5].emplace(black_bishop);
	squares[0][6].emplace(black_knight);
	squares[0][7].emplace(black_rook);

	squares[1].fill( { black_pawn } );
	squares[6].fill( { white_pawn } );

	squares[7][0].emplace(white_rook);
	squares[7][1].emplace(white_knight);
	squares[7][2].emplace(white_bishop);
	squares[7][3].emplace(white_queen);
	squares[7][4].emplace(white_king);
	squares[7][5].emplace(white_bishop);
	squares[7][6].emplace(white_knight);
	squares[7][7].emplace(white_rook);
}

Event Board::move() {
	return None;
}

void Board::draw() {
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

            if (squares[row][col].has_value())
            	al_draw_scaled_bitmap(
		        	squares[row][col]->sprite,
		            0, 
		            0, 
		            SPRITE_SIZE, 
		            SPRITE_SIZE,
		            MARGIN + TILE_SIZE * col, 
		            MARGIN + TILE_SIZE * row, 
		            TILE_SIZE, 
		            TILE_SIZE, 
		            0
		        );
        }
    }
}

Board::~Board() {
    for (int row = 0; row < 8; ++row) 
        for (int col = 0; col < 8; ++col) 
            if (squares[row][col].has_value())
            	al_destroy_bitmap(squares[row][col]->sprite);

    al_destroy_bitmap(spritesheet);
}

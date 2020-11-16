#include "Game.hpp"

int main(int argc, char **argv) 
{
    Game chess;

    int exit_code = chess.run();

    return exit_code;
}
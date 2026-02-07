#ifndef FEN_H
#define FEN_H

#include <stdbool.h>

#define FEN_START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

bool load_fen_position(const char* fen);
void generate_fen_position();

#endif

#ifndef FEN_H
#define FEN_H

#include "defines.h"
#include "types.h"
#include "utils.h"
#include <stdbool.h>

extern Positions positions;

bool verify_fen_position(const char* fen_pos);
bool load_fen_position(const char* fen);
void generate_fen_position(char* dest);

#endif

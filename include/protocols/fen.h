#ifndef FEN_H
#define FEN_H

#include "defines.h"
#include "types.h"
#include "utils.h"
#include <stdbool.h>

extern Positions positions;

bool modify_initial_fen(const char* fen);
bool verify_fen(const char* fen_pos);
bool load_fen(const char* fen);
void generate_fen(char* dest);

#endif

#ifndef FEN_H
#define FEN_H

#include <stdbool.h>
#include "utils.h"

typedef struct {
  char fen_p[256];
  int c;
} Position;

typedef ut_da_declare(Position) Positions;
extern Positions positions;
extern char current_fen[512];

void iterate_fen_positions();
bool load_fen_position(const char* fen);
void generate_fen_position();

#endif

#ifndef FEN_H
#define FEN_H

#include <stdbool.h>
#include "utils.h"

#define FEN_START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef struct {
  char fen_p[256];
  int c;
} Position;

typedef ut_da_declare(Position) Positions;
extern Positions positions;

bool load_fen_position(const char* fen);
void generate_fen_position();

#endif

#ifndef FEN_H
#define FEN_H

#include <stdbool.h>
#include "utils.h"

#define STARTING_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_MAX_LEN 86

typedef struct {
  char fen_p[FEN_MAX_LEN];
  int c;
} Position;

typedef ut_da_declare(Position) Positions;
extern Positions positions;

bool verify_fen_position(const char* fen_pos);
bool load_fen_position(const char* fen);
void generate_fen_position(char* dest);

#endif

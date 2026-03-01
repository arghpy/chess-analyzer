#ifndef SAN_H
#define SAN_H

#include "utils.h"

typedef struct {
  char san_w[8];
  char san_b[8];
  int move_nr;
} SanMove;

typedef ut_da_declare(SanMove) SanMoves;

extern SanMoves san_moves;

void generate_san(void);

#endif

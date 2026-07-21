#ifndef INIT_H
#define INIT_H

#include "protocols/fen.h"
#include "raylib.h"
#include "sound.h"
#include "types.h"
#include "utils.h"

extern ChessMoveNode *ll_chess_move_head;
extern ChessMove moving_piece;
extern ChessMoveNode *ll_chess_move_tail;
extern ChessMoveNode *ll_chess_move_current;
extern ChessBoard chess_board;
extern Texture2D chess_pieces_texture[];
extern const Color square_color[];
extern const Color occupied_square_color[];
extern const Color red_square_color[];
extern ChessSquare piece_promotions[4];

bool initialize_chess_board(void);
void load_pawn_promotions(void);
bool load_chess_pieces(void);
void unload_chess_pieces(void);

#endif

#include "protocols/fen.h"
#include "protocols/common.h"
#include "core.h"
#include "init.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

Positions positions = {0};

void iterate_fen_positions()
{
  printf("\n\n--------------------------------------------------\n");
    for (size_t i = 0; i < positions.count; i++) {
      printf("%d: %s\n", positions.items[i].c, positions.items[i].fen_p);
    }
  printf("--------------------------------------------------\n");
  fflush(stdout);
}

void add_fen_position(const char* fen)
{
  Position p = {0};
  p.c = 1;
  strcpy(p.fen_p, fen);

  if (positions.count == 0) ut_da_push(&positions, p);
  else {
    bool found = false;
    for (size_t i = 0; i < positions.count; i++) {
      if (strcmp(p.fen_p, positions.items[i].fen_p) == 0) {
        found = true;
        positions.items[i].c++;
        if (positions.items[i].c == 3) chess_board.result = DRAW;
        if (found || (chess_board.result == DRAW)) break;
      }
    }
    if (!found) ut_da_push(&positions, p);
  }
}

bool load_fen_position(const char* fen_pos)
{
  char fen[256];
  strcpy(fen, fen_pos);

  // Get the sections
  char *board, *color_turn, *castling, *enpassant_square, *halfmoves, *fullmoves, *end;
  board            = strtok(fen,  " ");
  color_turn       = strtok(NULL, " ");
  castling         = strtok(NULL, " ");
  enpassant_square = strtok(NULL, " ");
  halfmoves        = strtok(NULL, " ");
  fullmoves        = strtok(NULL, " ");
  end              = strtok(NULL, " ");

  if (end != NULL) {
    fprintf(stderr, "Wrong FEN(too long): %s.\n", fen);
    return false;
  }

  // Process board
  char *rank;
  int rank_c = 0;
  int column_c = 0;
  rank = strtok(board, "/");
  assert(rank != NULL);
  while (rank != NULL) {
    for (size_t i = 0; i < strlen(rank); i++) {
      switch(rank[i]) {
        case 'b':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = BISHOP, .color = B
          };
          column_c++;
          break;
        case 'k':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = KING, .color = B
          };
          column_c++;
          break;
        case 'n':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = KNIGHT, .color = B
          };
          column_c++;
          break;
        case 'p':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = PAWN, .color = B
          };
          column_c++;
          break;
        case 'q':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = QUEEN, .color = B
          };
          column_c++;
          break;
        case 'r':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = ROOK, .color = B
          };
          column_c++;
          break;
        case 'B':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = BISHOP, .color = W
          };
          column_c++;
          break;
        case 'K':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = KING, .color = W
          };
          column_c++;
          break;
        case 'N':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = KNIGHT, .color = W
          };
          column_c++;
          break;
        case 'P':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = PAWN, .color = W
          };
          column_c++;
          break;
        case 'Q':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = QUEEN, .color = W
          };
          column_c++;
          break;
        case 'R':
          chess_board.squares[rank_c][column_c].piece = (ChessPiece) {
            .type = ROOK, .color = W
          };
          column_c++;
          break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8': {
                    int limit = column_c + rank[i] - '0';
                    for (int k = column_c; k < limit; k++) {
                      reset_chess_square(&chess_board.squares[rank_c][k]);
                    }
                    column_c += rank[i] - '0';
                    break;
                  }
        default:
                  fprintf(stderr, "Unrecognized character: %c\n", rank[i]);
                  for (int y = 0; y < NS; y++) {
                    for (int x = 0; x < NS; x++) {
                      reset_chess_square(&chess_board.squares[y][x]);
                    }
                  }
                  return false;
      }
    }
    rank = strtok(NULL, "/");
    rank_c++;

    assert(column_c <= NS);
    assert(rank_c <= NS);
    column_c = 0;
  }

  // Process color turn
  if      (strcmp(color_turn, "w") == 0) chess_board.color_turn = W;
  else if (strcmp(color_turn, "b") == 0) chess_board.color_turn = B;
  else {
    fprintf(stderr, "Unrecognized character: %s\n", color_turn);
    chess_board.color_turn = N;
    return false;
  }

  // Process castling
  for (size_t i = 0; i < strlen(castling); i++) {
    if (castling[i] == '-') {
      if (strlen(castling) > 1) {
        fprintf(stderr, "Wrong castling notation: %s.\n", castling);
        return false;
      }
      chess_board.castle.w_s_can_castle = false;
      chess_board.castle.w_l_can_castle = false;
      chess_board.castle.b_s_can_castle = false;
      chess_board.castle.b_l_can_castle = false;
    } else {
      switch(castling[i]) {
        case 'K':
          chess_board.castle.w_s_can_castle = true;
          break;
        case 'Q':
          chess_board.castle.w_l_can_castle = true;
          break;
        case 'k':
          chess_board.castle.b_s_can_castle = true;
          break;
        case 'q':
          chess_board.castle.b_l_can_castle = true;
          break;
        default:
          fprintf(stderr, "Unrecognized character: %s\n", color_turn);
          return false;
      }
    }
  }

  // Process enpassant_square
  char *row    = "abcdefgh";
  char *column = "12345678";
  if (strlen(enpassant_square) > 2) {
    fprintf(stderr, "Wrong enpassant notation: %s.\n", enpassant_square);
    return false;
  }
  if (enpassant_square[0] == '-') {
    if (strlen(enpassant_square) > 1) {
      fprintf(stderr, "Wrong enpassant notation: %s.\n", enpassant_square);
      return false;
    }
    chess_board.enpassant.allowed = false;
    chess_board.enpassant.square = NULL;
  } else {
    size_t x = 0;
    for (; x < strlen(row); x++)
      if (row[x] == enpassant_square[0]) break;

    size_t y = 0;
    for (; y < strlen(column); y++)
      if (column[y] == enpassant_square[1]) break;

    chess_board.enpassant.allowed = true;
    chess_board.enpassant.square = &chess_board.squares[NS - 1 - y][x];
    chess_board.enpassant.square->board_color = RED;
    chess_board.enpassant.allowed_by_color = chess_board.color_turn == W ? B : W;
  }

  // Process halfmoves and fullmoves
  if (strlen(halfmoves) > 1 || !isdigit(halfmoves[0])) {
    fprintf(stderr, "Wrong halfmoves notation: %s.\n", halfmoves);
    return false;
  }
  chess_board.halfmoves = halfmoves[0] - '0';

  if (strlen(fullmoves) > 1 || !isdigit(fullmoves[0])) {
    fprintf(stderr, "Wrong fullmoves notation: %s.\n", fullmoves);
    return false;
  }
  chess_board.fullmoves = fullmoves[0] - '0';
  return true;
}

void generate_fen_position()
{
  char fen[256] = {0};

  int j = !chess_board.board_flipped ? 0 : NS - 1;
  // Process each rank
  for (; !chess_board.board_flipped ? j < NS : j >= 0; !chess_board.board_flipped ? j++ : j--) {
    int i = !chess_board.board_flipped ? 0 : NS - 1;

    char rank[20] = {0};
    for (; !chess_board.board_flipped ? i < NS : i >= 0; !chess_board.board_flipped ? i++ : i--) {
      ChessSquare *s = &chess_board.squares[j][i];
      char *piece_notation = get_piece_notation(s->piece);
      if (piece_notation != NULL) strcat(rank, piece_notation);
    }
    // Transform '0's in numbers for empty spaces
    char rank_tmp[50] = {0};

    for (size_t i = 0; i < strlen(rank); i++) {
      if (rank[i] != '0') strncat(rank_tmp, &rank[i], 1);
      else {
        size_t empty_c = 0;
        char empty_buf[5];
        size_t k = i;

        for (;k < strlen(rank) && rank[k] == '0'; k++) empty_c++;
        snprintf(empty_buf, sizeof(empty_buf), "%zu", empty_c);
        strcat(rank_tmp, empty_buf);
        i += empty_c - 1;
      }
    }
    strcpy(rank, rank_tmp);
    if (j != (NS - 1) || j != 0) strcat(rank, "/");
    strcat(fen, rank);
  }

  // Active color
  if      (chess_board.color_turn == W) strcat(fen, " w ");
  else if (chess_board.color_turn == B) strcat(fen, " b ");

  // Castling
  if (chess_board.castle.w_s_can_castle) strcat(fen, "K");
  if (chess_board.castle.w_l_can_castle) strcat(fen, "Q");
  if (chess_board.castle.b_s_can_castle) strcat(fen, "k");
  if (chess_board.castle.b_l_can_castle) strcat(fen, "q");
  if (!chess_board.castle.w_s_can_castle &&
      !chess_board.castle.w_l_can_castle &&
      !chess_board.castle.b_s_can_castle &&
      !chess_board.castle.b_l_can_castle) strcat(fen, "-");

  // Enpassant
  strcat(fen, " ");

  char *row    = "abcdefgh";
  char *column = "12345678";

  if (chess_board.enpassant.allowed && chess_board.enpassant.square != NULL) {
    ptrdiff_t index = chess_board.enpassant.square - &chess_board.squares[0][0];
    int y = index / NS;
    int x = index % NS;

    if (!chess_board.board_flipped) y = NS - y - 1;
    else x = NS - x - 1;
    strncat(fen, &row[x], 1);
    strncat(fen, &column[y], 1);
  } else strcat(fen, "-");
  add_fen_position(fen);

  // Half moves and Full moves
  char halfmoves[10] = {0};
  char fullmoves[10] = {0};
  snprintf(halfmoves, sizeof(halfmoves), " %d", chess_board.halfmoves);
  snprintf(fullmoves, sizeof(fullmoves), " %d", chess_board.fullmoves);

  strcat(fen, halfmoves);
  strcat(fen, fullmoves);
}

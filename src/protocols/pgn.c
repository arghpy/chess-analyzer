#include "protocols/pgn.h"
#include "defines.h"
#include <stddef.h>
#include "render.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "rules/general.h"
#include "init.h"
#include "core.h"
#include <stdio.h>
#include <string.h>

void process_san(const char* san)
{
  if (san && strlen(san) > 0) {
    switch (san[0]) {
      case 'K':
        moving_piece.piece = (ChessPiece) {.type = KING, .color = chess_board.color_turn};
        break;
        // Castle
      case 'O':
        moving_piece.piece = (ChessPiece) {.type = KING, .color = chess_board.color_turn};
        break;
      case 'B':
        moving_piece.piece = (ChessPiece) {.type = BISHOP, .color = chess_board.color_turn};
        break;
      case 'N':
        moving_piece.piece = (ChessPiece) {.type = KNIGHT, .color = chess_board.color_turn};
        break;
      case 'Q':
        moving_piece.piece = (ChessPiece) {.type = QUEEN, .color = chess_board.color_turn};
        break;
      case 'R':
        moving_piece.piece = (ChessPiece) {.type = ROOK, .color = chess_board.color_turn};
        break;
        // Pawn
      default:
        if (san[0] >= 97 && san[0] <= 104) {
          moving_piece.piece = (ChessPiece) {.type = PAWN, .color = chess_board.color_turn};
          break;
        } else {
          if (strstr(san, "{[\%clk")) fprintf(stderr, "No support for time notation\n");
          else fprintf(stderr, "Wrong san notation. Got: %s\n", san);
          exit(EXIT_FAILURE);
        }
    }
  }

  const char* promotion = strchr(san, '=');
  bool correct_move_found = false;
  // Performance can be increased here if desired, based on how many square are possible given a piece
  // But it parses in no time the longest game ever:
  // https://www.chess.com/article/view/the-4-longest-chess-games-in-history#nikolic-arsovic
  for (int j = 0; j < NS; j++) {
    for (int i = 0; i < NS; i++) {
      ChessSquare *s     = &chess_board.squares[j][i];
      ChessSquare s_copy = chess_board.squares[j][i];
      if (same_chess_piece(moving_piece.piece, s->piece)) {
        moving_piece.piece = s->piece;
        moving_piece.src = s;
        reset_chess_square(s);
        // ---------- place_piece ---------------
        ChessBoard old_chess_board = chess_board;
        for (int y = 0; y < NS; y++) {
          for (int x = 0; x < NS; x++) {
            chess_board = old_chess_board;
            ChessSquare *square_ptr      = &chess_board.squares[y][x];
            ChessPiece square_piece_copy = chess_board.squares[y][x].piece;
            if (valid_move(moving_piece.src, square_ptr, moving_piece.piece)) {
              // Needed for verifying if current player is in check
              // Check for promotion
              if (!promotion) {
                square_ptr->piece = moving_piece.piece;

                // Sounds:
                if (square_piece_copy.type != NO_PIECE || chess_board.enpassant.done) {
                  chess_board.state.captured = true;
                  chess_board.captured_piece = square_piece_copy;
                  chess_board.action_sound = CAPTURE;
                } else if (chess_board.castle.castled) {
                  chess_board.action_sound = CASTLE;
                } else chess_board.action_sound = MOVE;
              } else {
                ChessPiece promotion_piece = {0};
                if (strlen(promotion) != 2) {
                  fprintf(stderr, "Wrong promotion notation: %s\n", promotion);
                  exit(EXIT_FAILURE);
                } else {
                  switch (promotion[1]) {
                    case 'B':
                      promotion_piece = (ChessPiece) {.type = BISHOP, .color = moving_piece.piece.color};
                      break;
                    case 'N':
                      promotion_piece = (ChessPiece) {.type = KNIGHT, .color = moving_piece.piece.color};
                      break;
                    case 'Q':
                      promotion_piece = (ChessPiece) {.type = QUEEN,  .color = moving_piece.piece.color};
                      break;
                    case 'R':
                      promotion_piece = (ChessPiece) {.type = ROOK,   .color = moving_piece.piece.color};
                      break;
                    default:
                      fprintf(stderr, "Wrong promotion notation. Got: %s -> %s\n", san, promotion);
                      exit(EXIT_FAILURE);
                  }
                }

                // Sounds:
                if (square_piece_copy.type != NO_PIECE || chess_board.enpassant.done) {
                  chess_board.state.captured = true;
                  chess_board.captured_piece = square_piece_copy;
                  chess_board.action_sound = CAPTURE;
                } else if (chess_board.castle.castled) {
                  chess_board.action_sound = CASTLE;
                } else chess_board.action_sound = MOVE;

                square_ptr->piece = promotion_piece;
              }

              if (in_check(moving_piece.piece.color)) {
                chess_board.action_sound = ILLEGAL;
                square_ptr->piece = square_piece_copy;
                continue;
              }

              chess_board.state.piece_placed = true;
              moving_piece.dest = square_ptr;

              change_chess_board_turn();
              char current_san[20] = {0};
              generate_san(current_san);
              if (strcmp(san, current_san) == 0) {
                strcpy(moving_piece.san, current_san);
                correct_move_found = true;
              } else {
                change_chess_board_turn();
                moving_piece.dest = NULL;
                chess_board.state.piece_placed = false;
                square_ptr->piece = square_piece_copy;
                continue;
              }

              // Reset original colors and set colors for new valid moves
              reset_square_color_chess_move(ll_chess_move_tail);
              reset_square_color(moving_piece.src);
              reset_square_color(moving_piece.dest);

              moving_piece.src->board_color  = color_occupied_square(moving_piece.src);
              moving_piece.dest->board_color = color_occupied_square(moving_piece.dest);

              break;
            }
          }
          if (chess_board.state.piece_placed) break;
        }
        if (!chess_board.state.piece_placed) {
          s->piece = s_copy.piece;

          // Start again the main loop setting the source
          continue;
        }

        // ---------- place_piece ---------------
        if (in_check(chess_board.color_turn)) chess_board.action_sound = MOVE_CHECK;

        if (chess_board.enpassant.allowed)
          if (chess_board.enpassant.allowed_by_color != moving_piece.piece.color)
            chess_board.enpassant.allowed = false;

        // Full moves
        if (ll_chess_move_current->prev != NULL && moving_piece.piece.color == W)
          chess_board.fullmoves += 1;

        moving_piece.sound   = chess_board.action_sound;
        moving_piece.move_nr = chess_board.fullmoves;
        generate_fen(moving_piece.fen);
        generate_san(moving_piece.san);
        ut_ll_push(ChessMoveNode, ll_chess_move_head, moving_piece, ll_chess_move_tail);
        ll_chess_move_current = ll_chess_move_tail;
        advance_game_parameters();
        break;
      }
    }
    if (correct_move_found) break;
  }
  if (!correct_move_found) {
    fprintf(stderr, "Could not find a suitable move for: %s\n", san);
    exit(EXIT_FAILURE);
  }
}

bool load_pgn(const char* pgn_text)
{
  char* data = strdup(pgn_text);
  char* pgn_data = strdup(pgn_text);

  char* line_strtok_state;
  char* line = strtok_r(data, "\n", &line_strtok_state);

  // Process [] and ignore empty lines
  // TODO: here is an issue if there are no [] labels
  while (line && line[0] == '[') {
    if (!strcasestr(line, "FEN")) {
      line = strtok_r(NULL, "\n", &line_strtok_state);
    } else {
      char* fen_line = strdup(line);
      char* fen_line_strtok_state;
      char* fen = strtok_r(fen_line, "\"", &fen_line_strtok_state);
      fen = strtok_r(NULL, "\"", &fen_line_strtok_state);

      if (!fen) {
        fprintf(stderr, "Wrong FEN notation in PGN: %s\n", fen_line);
        return false;
      }
      if (!modify_initial_fen(fen)) return false;

      free(fen_line);
      line = strtok_r(NULL, "\n", &line_strtok_state);
    }
  }
  char* pgn = strstr(pgn_data, line);
  str_chr_replace_all(pgn, '\n', ' ');
  str_chr_remove_all(pgn, '.');

  // TODO: there is a problem when san is '..', made by our hand
  char* pgn_element_strtok_state;
  char* pgn_element = strtok_r(pgn, " ", &pgn_element_strtok_state);
  while (pgn_element) {
    if (str_is_number(pgn_element)) {
      moving_piece.move_nr = atoi(pgn_element);
      pgn_element = strtok_r(NULL, " ", &pgn_element_strtok_state);

      // White
      if (!pgn_element) break;
      process_san(pgn_element);
      pgn_element = strtok_r(NULL, " ", &pgn_element_strtok_state);

      // TODO: it's not correctly declared who won
      if (pgn_element) {
        if (strcmp(pgn_element, "1-0") == 0 || strcmp(pgn_element, "0-1") == 0) {
          game_state = CHECKMATE;
          break;
        } else if (strcmp(pgn_element, "1/2-1/2") == 0) {
          game_state = DRAW;
          break;
        } else if (strcmp(pgn_element, "*") == 0) {
          game_state = PLAYING;
          break;
        }
      }

      // Black
      if (!pgn_element) break;
      process_san(pgn_element);
      pgn_element = strtok_r(NULL, " ", &pgn_element_strtok_state);

      // TODO: it's not correctly declared who won
      if (pgn_element) {
        if (strcmp(pgn_element, "1-0") == 0 || strcmp(pgn_element, "0-1") == 0) {
          game_state = CHECKMATE;
          break;
        } else if (strcmp(pgn_element, "1/2-1/2") == 0) {
          game_state = DRAW;
          break;
        } else if (strcmp(pgn_element, "*") == 0) {
          game_state = PLAYING;
          break;
        }
      }
    } else {
      fprintf(stderr, "Wrong PGN notation. Expecting move number, got %s\n", pgn_element);
      break;
    }
  }

  free(data);
  free(pgn_data);
  return true;
}

#include "render.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "raylib.h"
#include "core.h"
#include "rules/general.h"
#include "rules/pieces.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

bool dragging = false;

float SQUARE_SIZE = 0.0f;

bool is_dragging(void)
{
  bool dragging = false;
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *square = &chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square->rect) &&
          (square->piece.type != NO_PIECE)
         ) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          dragging = true;
          chess_board.moving.src_piece = chess_board.squares[y][x].piece;
          chess_board.moving.c_src     = &chess_board.squares[y][x];
          reset_chess_square(square);
          break;
        }
      }
    }
    if (dragging) break;
  }
  return dragging;
}

void draw_piece_on_mouse(void)
{
  Vector2 mouse_pos = GetMousePosition();
  Rectangle square = chess_board.moving.c_src->rect;
  square.x      = mouse_pos.x - SQUARE_SIZE / 2;
  square.y      = mouse_pos.y - SQUARE_SIZE / 2;

  ChessSquare moving_piece = {0};
  moving_piece.piece = chess_board.moving.src_piece;
  moving_piece.rect = square;
  draw_piece(&moving_piece);
}

void draw_drag_and_place(void)
{
  dragging = dragging ? dragging : is_dragging();

  if (dragging) {
    draw_piece_on_mouse();

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      dragging = false;
      place_piece();
      chess_board.moving.wrong_move = false;
      if (chess_board.state.piece_placed) {
        chess_board.state.piece_placed = false;

        // Check promotion
        if (chess_board.moving.c_dest->piece.type == PAWN) {
          ptrdiff_t d_index = chess_board.moving.c_dest - &chess_board.squares[0][0];
          int yd = d_index / NS;

          if (yd == 0 || yd == (NS - 1)) {
            chess_board.state.promote = true;
            chess_board.promotion_square = chess_board.moving.c_dest;
            draw_promotion_pieces(chess_board.promotion_square);
            select_for_promotion(chess_board.promotion_square);
          }
        }
        if (!chess_board.state.promote) {
          if (chess_board.enpassant.done) chess_board.enpassant.done = false;

          // Record which color piece moved
          if      (chess_board.moving.src_piece.color == W) chess_board.state.w_moved = true;
          else if (chess_board.moving.src_piece.color == B) chess_board.state.b_moved = true;

          // Full moves
          if (chess_board.state.w_moved && chess_board.state.b_moved) {
            chess_board.fullmoves += 1;
            chess_board.state.w_moved = false;
            chess_board.state.b_moved = false;
          }

          // Half moves
          if (chess_board.moving.src_piece.type == PAWN || chess_board.state.captured)
            chess_board.halfmoves = 0;
          else chess_board.halfmoves += 1;

          if (chess_board.halfmoves == 50) chess_board.result = DRAW;

          generate_fen_position();
          verify_if_any_legal_move(chess_board.color_turn);
          generate_san();

          // Reset
          chess_board.moving.c_src = NULL;
          chess_board.moving.c_dest = NULL;
          chess_board.castle.castled = NO;

          if (chess_board.state.captured) {
            chess_board.state.captured = false;
            chess_board.moving.captured_piece = (ChessPiece){0};
          }
        }
      }
    }
  }
}

void check_pieces_hovering(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare square = chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square.rect) &&
          (square.piece.type != NO_PIECE)) {
        chess_board.state.hovering_piece = true;
        break;
      }
    }
    if (chess_board.state.hovering_piece) break;
  }
}

void set_mouse_cursor(void)
{
  SetMouseCursor(
      chess_board.state.hovering_piece ||
      dragging ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
  chess_board.state.hovering_piece = false;
}

void draw_moving_piece(void)
{
  check_pieces_hovering();
  draw_drag_and_place();
}

void draw_result(const Font *font)
{
  bool draw = false;
  char text[10];
  float font_size = roundf(SQUARE_SIZE * 0.8f);

  switch(chess_board.result) {
    case NONE: break;
    case CHECKMATE: {
                      draw = true;
                      snprintf(text, sizeof(text), "%s",
                          chess_board.color_turn == W ?
                          "BLACK WON" : "WHITE WON");
                      break;
                    }
    case DRAW:      {
                      draw = true;
                      snprintf(text, sizeof(text), "%s", "DRAW");
                      break;
                    }
  }
  if (draw) {
    float rsize = NS * SQUARE_SIZE / 2;
    Rectangle r = {
      .x      = rsize - rsize / 2,
      .y      = rsize - rsize / 2,
      .width  = rsize,
      .height = rsize,
    };
    DrawRectangleRounded(r, 0.20f, 90, background_color);
    DrawRectangleRoundedLinesEx(r, 0.20f, 90, 2.0f, WHITE);

    Vector2 text_size = MeasureTextEx(*font, text, font_size, 0);
    Vector2 pos = {
      .x = r.x + rsize / 2 - text_size.x / 2,
      .y = r.y + rsize / 5
    };
    DrawTextEx(*font, text, pos, font_size, 0, WHITE);
  }
}

Color color_occupied_square(const ChessSquare *s)
{
  return ColorIsEqual(s->board_color, square_color[LIGHT_TILE]) ?
    (Color){0xF6, 0xEA, 0x72, 0xFF} : (Color){0xDD, 0xC3, 0x4C, 0xFF};
}

void flip_board(void)
{
  for (int y = 0; y < NS / 2; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare tmp = chess_board.squares[y][x];
      chess_board.squares[y][x] = chess_board.squares[NS - 1 - y][NS - 1 - x];
      chess_board.squares[NS - 1 - y][NS - 1 - x] = tmp;
    }
  }
  if (chess_board.moving.p_src != NULL) {
    ptrdiff_t p_s_index = chess_board.moving.p_src - &chess_board.squares[0][0];
    int ys = p_s_index / NS;
    int xs = p_s_index % NS;

    chess_board.moving.p_src = &chess_board.squares[NS - 1 - ys][NS - 1 - xs];
  }

  if (chess_board.moving.p_dest != NULL) {
    ptrdiff_t p_d_index = chess_board.moving.p_dest - &chess_board.squares[0][0];
    int yd = p_d_index / NS;
    int xd = p_d_index % NS;

    chess_board.moving.p_dest = &chess_board.squares[NS - 1 - yd][NS - 1 - xd];
  }

  if (chess_board.enpassant.square != NULL) {
    ptrdiff_t e_index = chess_board.enpassant.square - &chess_board.squares[0][0];
    int y = e_index / NS;
    int x = e_index % NS;

    chess_board.enpassant.square = &chess_board.squares[NS - 1 - y][NS - 1 - x];
  }
}

void scale_chess_board(void)
{
  SQUARE_SIZE = roundf(
      GetScreenWidth() < GetScreenHeight()
      ? GetScreenWidth() / NS
      : GetScreenHeight() / NS
  );

  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++) {
      chess_board.squares[y][x].rect = (Rectangle) {
        .x      = x * SQUARE_SIZE,
        .y      = y * SQUARE_SIZE,
        .width  = SQUARE_SIZE,
        .height = SQUARE_SIZE,
      };
      chess_board.squares[y][x].center_proximity = (Circle) {
        .center = (Vector2) {
          .x = x * SQUARE_SIZE + SQUARE_SIZE / 2,
          .y = y * SQUARE_SIZE + SQUARE_SIZE / 2
        },
        .r = SQUARE_SIZE / 2.5f,
      };
    }
}

void draw_board(void)
{
  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++)
      DrawRectangleRec(chess_board.squares[y][x].rect, chess_board.squares[y][x].board_color);
}

void draw_board_coordinates(const Font *font)
{
  char text[10];
  int square_spacing = 5;
  float font_size = roundf(SQUARE_SIZE * 0.3f);

  // Numbers
  for (int y = 0; y < NS; y++) {
    snprintf(text, sizeof(text), "%d", chess_board.board_flipped ? y + 1 : NS - y);

    Vector2 pos = {
      .x = chess_board.squares[y][0].rect.x + square_spacing,
      .y = chess_board.squares[y][0].rect.y + square_spacing
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[(y + 1) % 2]);
  }

  // Letters
  for (int x = 0; x < NS; x++) {
    snprintf(text, sizeof(text), "%c", chess_board.board_flipped ? 'h' - x : 'a' + x);

    Vector2 text_size = MeasureTextEx(*font, text, font_size, 0);

    Vector2 pos = {
      .x = chess_board.squares[NS - 1][x].rect.x + (SQUARE_SIZE - text_size.x - square_spacing),
      .y = chess_board.squares[NS - 1][x].rect.y + (SQUARE_SIZE - text_size.y - square_spacing)
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[x % 2]);
  }
}

void draw_piece(const ChessSquare *square)
{
  float piece_size = SQUARE_SIZE * 0.9;

  Rectangle rect = square->rect;
  Texture2D *piece_texture = &chess_pieces_texture[2*(square->piece.type - 1) + square->piece.color - 1];

  Rectangle source = {
    0, 0,
    piece_texture->width,
    piece_texture->height
  };

  Rectangle dest = {
    rect.x + (SQUARE_SIZE / 2 - piece_size / 2),
    rect.y + (SQUARE_SIZE / 2 - piece_size / 2),
    piece_size,
    piece_size
  };

  DrawTexturePro(*piece_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void draw_chess_pieces(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessPieceType type   = chess_board.squares[y][x].piece.type;
      if (type != NO_PIECE) {
        // Account for NO_PIECE
        draw_piece(&chess_board.squares[y][x]);
      }
    }
  }
}

void draw_chess_board(const Font *font)
{
  scale_chess_board();
  draw_board();
  draw_board_coordinates(font);
  draw_chess_pieces();
}

#include "render.h"
#include "utils.h"
#include "init.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "raylib.h"
#include "core.h"
#include "rules/general.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

bool dragging = false;

Rectangle fen_button_r = {0};
bool copying_fen = false;

Rectangle pgn_button_r = {0};
bool copying_pgn = false;

// float SQUARE_SIZE = 0.0f;

void reset_colors_for_current_move(void)
{
  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++)
      if (!ColorIsEqual(chess_board.squares[y][x].board_color, RED_SQUARE))
        reset_square_color(&chess_board.squares[y][x]);

  if (ll_chess_move_current->value.src != NULL &&
      ll_chess_move_current->value.dest != NULL)
  {
    ll_chess_move_current->value.src->board_color  = color_occupied_square(ll_chess_move_current->value.src);
    ll_chess_move_current->value.dest->board_color = color_occupied_square(ll_chess_move_current->value.dest);
  }
}

char *generate_pgn(void)
{
  size_t pgn_capacity = 100;
  char* pgn = calloc(pgn_capacity, sizeof(char));

  switch (game_state) {
    case REWINDING:
    case PLAYING:
      sprintf(pgn, "\n[Result \"*\"]\n\n");
      break;
    case PROMOTING:
      break;
    case DRAW:
      sprintf(pgn, "\n[Result \"1/2-1/2\"]\n\n");
      break;
    case CHECKMATE: {
                      int white_result = chess_board.color_turn == W ? 0 : 1;
                      int black_result = chess_board.color_turn == W ? 1 : 0;
                      sprintf(pgn, "\n[Result \"%d-%d\"]\n\n", white_result, black_result);
                      break;
                    }
  }
  ChessMoveNode *ll_n = ll_chess_move_head->next;
  while (ll_n != NULL) {
    char move[50] = {0};
    char tmp[50] = {0};

    if (ll_n->value.move_nr != ll_n->prev->value.move_nr) sprintf(move, "%d. ", ll_n->value.move_nr);
    sprintf(tmp, "%s ", ll_n->value.san);
    strcat(move, tmp);

    // Allocate more memory for PGN
    if (strlen(pgn) + strlen(move) + 2 > pgn_capacity) {
      pgn_capacity *= 2;
      pgn = realloc(pgn, pgn_capacity);
      if(!pgn) fprintf(stderr, "Not enough memory.\n");
    }
    strcat(pgn, move);

    ll_n = ll_n->next;
  }
  return pgn;
}

void draw_copy_pgn_button(const Font* font)
{
  float pgn_button_r_thickness = 1.0f;
  pgn_button_r = (Rectangle) {
    .x      = fen_button_r.x + fen_button_r.width,
    .y      = GetScreenHeight() - SQUARE_SIZE / 2.0f,
    .width  = (GetScreenWidth() - SQUARE_SIZE * NS) / 2.0f,
    .height = SQUARE_SIZE / 2.0f,
  };

  if (CheckCollisionPointRec(GetMousePosition(), pgn_button_r)) pgn_button_r_thickness *= 3;

  DrawRectangleRec(pgn_button_r, BROWN);
  DrawRectangleLinesEx(pgn_button_r, pgn_button_r_thickness, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), pgn_button_r)) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      char *current_pgn = generate_pgn();
      SetClipboardText(current_pgn);
      free(current_pgn);
      copying_pgn = true;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) copying_pgn = false;
  }

  if (copying_pgn) DrawRectangleRec(pgn_button_r, Fade(GRAY, 0.3f));

  char *text = "Copy PGN";
  Vector2 text_size = MeasureTextEx(*font, text, font->baseSize, 0);
  Vector2 pos = {
    .x = pgn_button_r.x + pgn_button_r.width  / 2 - text_size.x / 2,
    .y = pgn_button_r.y + pgn_button_r.height / 2 - text_size.y / 2
  };
  DrawTextEx(*font, text, pos, font->baseSize, 0, WHITE);
}

void draw_copy_fen_button(const Font* font)
{
  float fen_button_r_thickness = 1.0f;
  fen_button_r = (Rectangle) {
    .x      = SQUARE_SIZE * NS,
    .y      = GetScreenHeight() - SQUARE_SIZE / 2.0f,
    .width  = (GetScreenWidth() - SQUARE_SIZE * NS) / 2.0f,
    .height = SQUARE_SIZE / 2.0f,
  };

  if (CheckCollisionPointRec(GetMousePosition(), fen_button_r)) fen_button_r_thickness *= 3;

  DrawRectangleRec(fen_button_r, BROWN);
  DrawRectangleLinesEx(fen_button_r, fen_button_r_thickness, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), fen_button_r)) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (ll_chess_move_current == NULL) SetClipboardText(STARTING_POSITION);
      else SetClipboardText(ll_chess_move_current->value.fen);
      copying_fen = true;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) copying_fen = false;
  }

  if (copying_fen) DrawRectangleRec(fen_button_r, Fade(GRAY, 0.3f));

  char *text = "Copy FEN";
  Vector2 text_size = MeasureTextEx(*font, text, font->baseSize, 0);
  Vector2 pos = {
    .x = fen_button_r.x + fen_button_r.width  / 2 - text_size.x / 2,
    .y = fen_button_r.y + fen_button_r.height / 2 - text_size.y / 2
  };
  DrawTextEx(*font, text, pos, font->baseSize, 0, WHITE);
}

// Made with the help of claude from previous commit
void draw_san_window(const Font *font)
{
  float san_r_thickness = 1.0f;
  Rectangle san_r = {
    .x      = SQUARE_SIZE * NS,
    .y      = 0,
    .width  = GetScreenWidth() - SQUARE_SIZE * NS,
    .height = GetScreenHeight(),
  };
  int   spacing     = 5;
  float font_height = MeasureTextEx(*font, "1", font->baseSize, 0).y;

  DrawRectangleLinesEx(san_r, san_r_thickness, WHITE);

  float content_height   = font_height * ll_chess_move_tail->value.move_nr;
  float window_height    = (float)GetScreenHeight() - fen_button_r.height;
  bool  needs_scroll     = content_height > window_height;

  // Minimum bar height
  float min_bar_height   = font_height * 3.0f;
  float natural_bar_h    = needs_scroll
                           ? window_height * (window_height / content_height)
                           : window_height;
  float scroll_bar_height = fmaxf(natural_bar_h, min_bar_height);

  // How far bar and content can each travel
  float scrollable_bar     = window_height - scroll_bar_height;
  float scrollable_content = content_height - window_height;

  static float bar_y = 0.0f;
  float scroll_one_distance = 8.0f;

  static bool initialized = false;
  static float bar_x = 0.0f, bar_w = 0.0f;
  if (!initialized) {
    bar_x       = GetScreenWidth() - SQUARE_SIZE / 5.0f;
    bar_w       = SQUARE_SIZE / 5.0f;
    initialized = true;
  }

  // Auto-advance bar when new moves push content down
  static int last_move_count = 0;

  if (ll_chess_move_tail->value.move_nr != last_move_count) {
      // A new move was just added — snap bar to bottom
      bar_y = scrollable_bar;
      last_move_count = ll_chess_move_tail->value.move_nr;
  }

  if (needs_scroll) {
    float wheel = 0.0f;
    Rectangle san_hover_r = san_r;
    if (CheckCollisionPointRec(GetMousePosition(), san_hover_r))
      wheel = GetMouseWheelMove(); // + UP, - DOWN

    bar_y -= wheel * scroll_one_distance;
    if      (bar_y < 0.0f)           bar_y = 0.0f;
    else if (bar_y > scrollable_bar) bar_y = scrollable_bar;

    // Draw scroll track + bar
    Rectangle bar = {
      .x      = bar_x,
      .y      = bar_y,
      .width  = bar_w,
      .height = scroll_bar_height,
    };
    DrawRectangleRounded(bar, 0.85f, 32, WHITE);
  }

  // Map bar position → content offset
  float t              = (scrollable_bar > 0.0f) ? bar_y / scrollable_bar : 0.0f;
  float content_scroll = t * scrollable_content;

  ChessMoveNode *ll_n = ll_chess_move_head->next;
  char notation[64] = {0};
  while (ll_n != NULL) {
    char buf[64] = {0};

    if (ll_n->value.move_nr != ll_n->prev->value.move_nr) {
      sprintf(buf, "%d.", ll_n->value.move_nr);
      sprintf(notation, "%-5s", buf);
    }

    if (strcmp(ll_n->value.san, "") > 0) {
      sprintf(buf, "%10s", ll_n->value.san);
      strcat(notation, buf);
    }

    if (ll_n->value.move_nr == ll_n->prev->value.move_nr) strcat(notation, "\n");

    Vector2 text_pos = {
      .x = san_r.x + spacing,
      .y = san_r.y + font_height * (ll_n->value.move_nr - 1) + spacing - content_scroll - 5,
    };

    ll_n = ll_n->next;

    // Cull lines fully outside the visible area
    if (text_pos.y + font_height < 0 || text_pos.y > window_height)
      continue;

    if (strcmp(notation, "") > 0)
      DrawTextEx(*font, notation, text_pos, font->baseSize, 0, WHITE);
  }
}

bool is_dragging_piece(void)
{
  bool dragging = false;
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *square = &chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square->rect) &&
          (square->piece.type != NO_PIECE)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          dragging = true;
          moving_piece.piece = chess_board.squares[y][x].piece;
          moving_piece.src   = &chess_board.squares[y][x];
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
  Rectangle square = moving_piece.src->rect;
  square.x      = mouse_pos.x - SQUARE_SIZE / 2.0f;
  square.y      = mouse_pos.y - SQUARE_SIZE / 2.0f;

  ChessSquare s = {0};
  s.piece = moving_piece.piece;
  s.rect = square;
  draw_piece(&s);
}

void draw_drag_and_place(void)
{
  dragging = dragging ? dragging : is_dragging_piece();

  if (dragging) {
    draw_piece_on_mouse();

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      dragging = false;
      place_piece();
      if (chess_board.state.piece_placed) {
        change_chess_board_turn();
        if (in_check(chess_board.color_turn)) chess_board.action_sound = MOVE_CHECK;

        if (chess_board.enpassant.allowed)
          if (chess_board.enpassant.allowed_by_color != moving_piece.piece.color)
            chess_board.enpassant.allowed = false;

        // Check promotion
        if (moving_piece.piece.type == PAWN) {
          ptrdiff_t d_index = moving_piece.dest - &chess_board.squares[0][0];
          int yd = d_index / NS;

          if (yd == 0 || yd == (NS - 1)) {
            game_state = PROMOTING;
            chess_board.action_sound = NOTHING;
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
      dragging ||
      CheckCollisionPointRec(GetMousePosition(), fen_button_r) ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
  chess_board.state.hovering_piece = false;
}

void draw_moving_piece(void)
{
  check_pieces_hovering();
  draw_drag_and_place();
}

void draw_result(const Font *font, const char* result)
{
  float rsize = NS * SQUARE_SIZE / 2.0f;
  Rectangle r = {
    .x      = rsize - rsize / 2,
    .y      = rsize - rsize / 2,
    .width  = rsize,
    .height = rsize,
  };
  DrawRectangleRounded(r, 0.20f, 90, background_color);
  DrawRectangleRoundedLinesEx(r, 0.20f, 90, 2.0f, WHITE);

  Vector2 text_size = MeasureTextEx(*font, result, font->baseSize, 0);
  Vector2 pos = {
    .x = r.x + rsize / 2 - text_size.x / 2,
    .y = r.y + rsize / 5
  };
  DrawTextEx(*font, result, pos, font->baseSize, 0, WHITE);
}

Color color_occupied_square(const ChessSquare *s)
{
  return ColorIsEqual(s->board_color, square_color[LIGHT_TILE]) ?
    occupied_square_color[LIGHT_OCCUPIED_TILE] : occupied_square_color[DARK_OCCUPIED_TILE];
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
  // TODO: You really need to check this
  if (ll_chess_move_head != NULL) {
    ChessMoveNode *tmp = ll_chess_move_head;
    while (tmp != NULL) {
      if (tmp->value.src != NULL) {
        ptrdiff_t p_s_index = tmp->value.src - &chess_board.squares[0][0];
        int ys = p_s_index / NS;
        int xs = p_s_index % NS;

        tmp->value.src = &chess_board.squares[NS - 1 - ys][NS - 1 - xs];
      }

      if (tmp->value.dest != NULL) {
        ptrdiff_t p_d_index = tmp->value.dest - &chess_board.squares[0][0];
        int yd = p_d_index / NS;
        int xd = p_d_index % NS;

        tmp->value.dest = &chess_board.squares[NS - 1 - yd][NS - 1 - xd];
      }

      tmp = tmp->next;
    }
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
  // SQUARE_SIZE = roundf(
  //     GetScreenWidth() < GetScreenHeight()
  //     ? GetScreenWidth() / NS
  //     : GetScreenHeight() / NS
  // );

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
          .x = x * SQUARE_SIZE + SQUARE_SIZE / 2.0f,
          .y = y * SQUARE_SIZE + SQUARE_SIZE / 2.0f
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

  // Numbers
  for (int y = 0; y < NS; y++) {
    snprintf(text, sizeof(text), "%d", chess_board.board_flipped ? y + 1 : NS - y);

    Vector2 pos = {
      .x = chess_board.squares[y][0].rect.x + square_spacing,
      .y = chess_board.squares[y][0].rect.y + square_spacing
    };
    DrawTextEx(*font, text, pos, font->baseSize, 0, square_color[(y + 1) % 2]);
  }

  // Letters
  for (int x = 0; x < NS; x++) {
    snprintf(text, sizeof(text), "%c", chess_board.board_flipped ? 'h' - x : 'a' + x);

    Vector2 text_size = MeasureTextEx(*font, text, font->baseSize, 0);

    Vector2 pos = {
      .x = chess_board.squares[NS - 1][x].rect.x + (SQUARE_SIZE - text_size.x - square_spacing),
      .y = chess_board.squares[NS - 1][x].rect.y + (SQUARE_SIZE - text_size.y - square_spacing)
    };
    DrawTextEx(*font, text, pos, font->baseSize, 0, square_color[x % 2]);
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
    rect.x + (SQUARE_SIZE / 2.0f - piece_size / 2),
    rect.y + (SQUARE_SIZE / 2.0f - piece_size / 2),
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

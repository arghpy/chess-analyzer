#include "core.h"
#include "init.h"
#include "protocols/fen.h"
#include "protocols/pgn.h"
#include "protocols/san.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include "rules/general.h"
#include "utils.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

bool dragging = false;

Rectangle fen_button_r = {0};
bool copying_fen = false;

Rectangle pgn_button_r = {0};
bool copying_pgn = false;

Rectangle title_r = {0};
Rectangle san_r = {0};
Rectangle bar_r = {0};

Arrows drawn_arrows = {0};

typedef struct {
  Rectangle rect;
  char* text;
  bool interacting;
  bool text_pasted;
  size_t frame_counter;
} WindowContext;

WindowContext paste_fen = {0};
WindowContext paste_pgn = {0};
WindowContext import_fen_pgn = {0};

float px(float v)
{
  return floorf(v + 0.5f);
}

void draw_paste_fen_window(const Font* font)
{
  float font_height = MeasureTextEx(*font, "1", font->baseSize, 0).y;
  paste_fen.rect = (Rectangle) {
    .x      = px(title_r.x + font_height/2.0f),
    .y      = px(title_r.y + title_r.height + font_height),
    .width  = px(GetScreenWidth() - SQUARE_SIZE * NS - font_height/2.0f * 2),
    .height = px(font_height * 2),
  };
  DrawRectangleRounded(paste_fen.rect, 0.12f, 8, paste_window_color);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (CheckCollisionPointRec(GetMousePosition(), paste_fen.rect)) paste_fen.interacting = true;
    else paste_fen.interacting = false;
  }

  Vector2 pos = {
    .x = paste_fen.rect.x,
    .y = paste_fen.rect.y + paste_fen.rect.height / 2 - font_height / 2
  };

  char window_text[10] = {0};
  if (! paste_fen.text_pasted) {
    if (! paste_fen.interacting) {
      sprintf(window_text, "Paste FEN");
      paste_fen.frame_counter = 0;
      Vector2 text_size = MeasureTextEx(*font, window_text, font->baseSize, 0);
      pos = (Vector2) {
        .x = paste_fen.rect.x + paste_fen.rect.width  / 2 - text_size.x / 2,
        .y = paste_fen.rect.y + paste_fen.rect.height / 2 - text_size.y / 2
      };
    } else {
      if((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
        const char *text = GetClipboardText();
        if (text != NULL && strlen(text) > 0) {
          if (paste_fen.text == NULL) paste_fen.text = calloc(FEN_MAX_LEN, sizeof(char));
          strncpy(paste_fen.text, text, FEN_MAX_LEN);
          paste_fen.text_pasted = true;
          paste_fen.interacting = false;
          paste_fen.frame_counter = 0;
        }
      } else {
        if ((paste_fen.frame_counter/20)%2 == 0) strcpy(window_text, "|");
        else strcpy(window_text, "");
        paste_fen.frame_counter++;
      }
    }
  }
  BeginScissorMode(paste_fen.rect.x, paste_fen.rect.y, paste_fen.rect.width, paste_fen.rect.height);
  DrawTextEx(*font, paste_fen.text == NULL ? window_text : paste_fen.text, pos, font->baseSize, 0, WHITE);
  EndScissorMode();
}

void draw_paste_pgn_window(const Font* font)
{
  float font_height = MeasureTextEx(*font, "1", font->baseSize, 0).y;
  paste_pgn.rect = (Rectangle) {
    .x      = px(paste_fen.rect.x),
    .y      = px(paste_fen.rect.y + paste_fen.rect.height + font_height/2.0f),
    .width  = px(GetScreenWidth() - SQUARE_SIZE * NS - font_height/2.0f * 2),
    .height = px(font_height * 10),
  };
  DrawRectangleRounded(paste_pgn.rect, 0.08f, 8, paste_window_color);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (CheckCollisionPointRec(GetMousePosition(), paste_pgn.rect)) paste_pgn.interacting = true;
    else paste_pgn.interacting = false;
  }

  Vector2 pos = {
    .x = paste_pgn.rect.x,
    .y = paste_pgn.rect.y
  };

  char window_text[10] = {0};
  if (! paste_pgn.text_pasted) {
    if (! paste_pgn.interacting) {
      sprintf(window_text, "Paste PGN");
      paste_pgn.frame_counter = 0;
      Vector2 text_size = MeasureTextEx(*font, window_text, font->baseSize, 0);
      pos = (Vector2) {
        .x = paste_pgn.rect.x + paste_pgn.rect.width  / 2 - text_size.x / 2,
        .y = paste_pgn.rect.y + paste_pgn.rect.height / 2 - text_size.y / 2
      };
    } else {
      if((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
        const char *text = GetClipboardText();
        if (text != NULL && strlen(text) > 0) {
          if (paste_pgn.text == NULL) paste_pgn.text = calloc(PGN_MAX_LEN, sizeof(char));
          strncpy(paste_pgn.text, text, PGN_MAX_LEN);
          paste_pgn.text_pasted = true;
          paste_pgn.interacting = false;
          paste_pgn.frame_counter = 0;
        }
      } else {
        if ((paste_pgn.frame_counter/20)%2 == 0) strcpy(window_text, "|");
        else strcpy(window_text, "");
        paste_pgn.frame_counter++;
      }
    }
  }
  BeginScissorMode(paste_pgn.rect.x, paste_pgn.rect.y, paste_pgn.rect.width, paste_pgn.rect.height);
  DrawTextEx(*font, paste_pgn.text == NULL ? window_text : paste_pgn.text, pos, font->baseSize, 0, WHITE);
  EndScissorMode();
}

void draw_import_fen_pgn_button(const Font* font)
{
  float rect_thickness = 1.0f;
  float font_height = MeasureTextEx(*font, "1", font->baseSize, 0).y;
  import_fen_pgn.rect = (Rectangle) {
    .x      = px(paste_pgn.rect.x),
    .y      = px(paste_pgn.rect.y + paste_pgn.rect.height + font_height/2.0f),
    .width  = px(GetScreenWidth() - SQUARE_SIZE * NS - font_height/2.0f * 2),
    .height = SQUARE_SIZE / 2.0f,
  };

  if (CheckCollisionPointRec(GetMousePosition(), import_fen_pgn.rect)) rect_thickness *= 3;

  DrawRectangleRounded(import_fen_pgn.rect, 0.08f, 8, button_color);
  DrawRectangleRoundedLinesEx(import_fen_pgn.rect, 0.08f, 8, rect_thickness, WHITE);

  import_fen_pgn.text = "Import";
  Vector2 text_size = MeasureTextEx(*font, import_fen_pgn.text, font->baseSize, 0);
  Vector2 pos = {
    .x = import_fen_pgn.rect.x + import_fen_pgn.rect.width  / 2 - text_size.x / 2,
    .y = import_fen_pgn.rect.y + import_fen_pgn.rect.height / 2 - text_size.y / 2
  };
  DrawTextEx(*font, import_fen_pgn.text, pos, font->baseSize, 0, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), import_fen_pgn.rect)) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      import_fen_pgn.interacting = true;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      if (paste_fen.text != NULL) {
        if (modify_initial_fen(paste_fen.text)) menu_state = ANALYSIS;
        free(paste_fen.text);
        paste_fen.text = NULL;
        paste_fen.text_pasted = false;
      } else {
        if (paste_pgn.text != NULL) {
          load_pgn(paste_pgn.text);
          free(paste_pgn.text);
          paste_pgn.text = NULL;
          menu_state = ANALYSIS;
        }
      }
      import_fen_pgn.interacting = false;
    }
  }

  if (import_fen_pgn.interacting) DrawRectangleRec(import_fen_pgn.rect, Fade(GRAY, 0.3f));
}

// Vector v1 is dest, or head of arrow
void draw_arrow(const Arrow* arrow)
{
  DrawTriangle(arrow->v1, arrow->v2, arrow->v3, arrow->color);

  Vector2 s = arrow->src;
  Vector2 d = {
      arrow->v1.x - arrow->direction.x * SQUARE_SIZE/3.0f,
      arrow->v1.y - arrow->direction.y * SQUARE_SIZE/3.0f
  };

  DrawLineEx(s, d, SQUARE_SIZE / 4.0f, arrow->color);
}

void add_arrow(const ChessSquare* src, const ChessSquare* dest)
{
  ptrdiff_t s_index = src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  int y = yd - ys;
  int x = xd - xs;

  // You need to learn math behind game programming
  Vector2 direction     = Vector2Normalize((Vector2){ x, y});
  Vector2 perpendicular = {-direction.y, direction.x};

  float size = SQUARE_SIZE/3.0f;

  Vector2 v1 = dest->center_proximity.center;
  Vector2 v2 = {
    .x = v1.x + (-direction.x - perpendicular.x) * size,
    .y = v1.y + (-direction.y - perpendicular.y) * size,
  };
  Vector2 v3 = {
    .x = v1.x + (-direction.x + perpendicular.x) * size,
    .y = v1.y + (-direction.y + perpendicular.y) * size,
  };

  Arrow arrow = {
    .direction = direction,
    .src = src->center_proximity.center,
    .v1 = v1,
    .v2 = v2,
    .v3 = v3,
    .color = arrow_color,
  };

  bool exists = false;
  for (size_t i = 0; i < drawn_arrows.count; i++) {
    if (Vector2Equals(arrow.direction, drawn_arrows.items[i].direction) &&
        Vector2Equals(arrow.src, drawn_arrows.items[i].src) &&
        Vector2Equals(arrow.v1, drawn_arrows.items[i].v1) &&
        Vector2Equals(arrow.v2, drawn_arrows.items[i].v2) &&
        Vector2Equals(arrow.v3, drawn_arrows.items[i].v3) &&
        ColorIsEqual(arrow.color, drawn_arrows.items[i].color)
        ) {
      ut_da_remove(&drawn_arrows, i);
      exists = true;
      break;
    }
  }
  if (!exists) ut_da_push(&drawn_arrows, arrow);
}

void draw_title(const Font* font)
{
  float font_height = MeasureTextEx(*font, "1", font->baseSize, 0).y;
  title_r = (Rectangle) {
    .x      = SQUARE_SIZE * NS,
    .y      = 0,
    .width  = GetScreenWidth() - SQUARE_SIZE * NS,
    .height = font_height * 2,
  };
  DrawRectangleRec(title_r, GRAY);

  char *text = "Analysis";
  Vector2 text_size = MeasureTextEx(*font, text, font->baseSize, 0);
  Vector2 pos = {
    .x = title_r.x + title_r.width  / 2 - text_size.x / 2,
    .y = title_r.y + title_r.height / 2 - text_size.y / 2
  };
  DrawTextEx(*font, text, pos, font->baseSize, 0, WHITE);
}

void reset_colors_for_current_move(void)
{
  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++)
      if (!ColorIsEqual(chess_board.squares[y][x].board_color, red_square_color[(x + y) % 2]))
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
      sprintf(pgn, "\n[Result \"*\"]\n");
      break;
    case PROMOTING:
      break;
    case DRAW:
      sprintf(pgn, "\n[Result \"1/2-1/2\"]\n");
      break;
    case CHECKMATE: {
                      int white_result = chess_board.color_turn == W ? 0 : 1;
                      int black_result = chess_board.color_turn == W ? 1 : 0;
                      sprintf(pgn, "\n[Result \"%d-%d\"]\n", white_result, black_result);
                      break;
                    }
  }
  char tmp[100] = {0};
  sprintf(tmp, "[FEN \"%s\"]\n\n", ll_chess_move_head->value.fen);
  strcat(pgn, tmp);

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

  DrawRectangleRec(pgn_button_r, button_color);
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

  DrawRectangleRec(fen_button_r, button_color);
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

void draw_san_text_moves(const Font* font, float offset)
{
  int spacing = 5;
  float font_height = MeasureTextEx(*font, "1", font->baseSize, 0).y;

  // Skip starting position
  ChessMoveNode *ll_n = ll_chess_move_head->next;
  char notation[64] = {0};
  while (ll_n != NULL) {
    char tmp[64] = {0};

    // Move number
    sprintf(notation, "%d.", ll_n->value.move_nr);
    size_t move_nr_len = strlen(notation);
    int base_padding   = 15;
    int padding        = base_padding - move_nr_len;

    // White
    if (strcmp(ll_n->value.san, "") > 0) {
      sprintf(tmp, "%*s", padding, ll_n->value.san);
      strcat(notation, tmp);
    }

    // Black
    if (ll_n->next != NULL && strcmp(ll_n->next->value.san, "") > 0) {
      sprintf(tmp, "%*s", base_padding, ll_n->next->value.san);
      strcat(notation, tmp);
    }

    int relative_move_nr = ll_chess_move_head->next->value.move_nr == 1 ?
                           ll_n->value.move_nr - 1 :
                           ll_n->value.move_nr - ll_chess_move_head->next->value.move_nr;

    Vector2 text_pos = {
      .x = san_r.x + spacing,
      .y = san_r.y + font_height * relative_move_nr - offset,
    };


    if (strcmp(notation, "") > 0) {
      // Don't write if not visible
      if ((text_pos.y + font_height) >= san_r.y && text_pos.y <= (san_r.y + san_r.height)) {
        // This is in order to prevent matching 'h4' to 'h4' and 'Qh4'
        char tmp_san[10] = {0};
        tmp_san[0] = ' ';
        strcat(tmp_san, ll_chess_move_current->value.san);
        char* current_san = strstr(notation, tmp_san);
        if (current_san && ll_chess_move_current->value.move_nr == ll_n->value.move_nr) {
          current_san = current_san + 1;
          float san_length          = MeasureTextEx(*font, ll_chess_move_current->value.san, font->baseSize, 0).x;
          float notation_length     = MeasureTextEx(*font, notation, font->baseSize, 0).x;
          float san_position_length = MeasureTextEx(*font, current_san, font->baseSize, 0).x;
          Rectangle r = {
            .x = text_pos.x + notation_length - san_position_length,
            .y = text_pos.y,
            .width = san_length,
            .height = font_height,
          };
          DrawRectangleRec(r, GRAY);
        }
        DrawTextEx(*font, notation, text_pos, font->baseSize, 0, WHITE);
      }
    }


    if (ll_n->next != NULL) ll_n = ll_n->next->next;
    else ll_n = NULL;
  }
}

void draw_san_window(const Font *font)
{
  san_r = (Rectangle) {
    .x      = SQUARE_SIZE * NS,
    .y      = title_r.y + title_r.height,
    .width  = GetScreenWidth() - SQUARE_SIZE * NS,
    .height = GetScreenHeight() - title_r.height - fen_button_r.height,
  };
  float font_height = MeasureTextEx(*font, "1", font->baseSize, 0).y;

  int relative_move_nr = ll_chess_move_tail->value.move_nr;
  if (ll_chess_move_head->next->value.move_nr != 1)
    relative_move_nr = ll_chess_move_tail->value.move_nr - ll_chess_move_head->next->value.move_nr;

  float san_text_height = font_height * relative_move_nr;

  static float offset = 0.0f;
  static float scrollbar_target = 0.0f;

  // The mother of all scroll bars
  if (san_text_height > san_r.height) {
    float scrollable_san = san_text_height - san_r.height;

    // Minimum bar height
    float min_bar_h = font_height * 3.0f;
    float nat_bar_h = san_r.height * (san_r.height / san_text_height);

    bar_r.height = fmaxf(nat_bar_h, min_bar_h);
    bar_r.width  = SQUARE_SIZE / 5.0f;
    bar_r.x      = GetScreenWidth() - bar_r.width;

    // Auto-advance bar when new moves push san_text down
    // no smoothing
    static ChessMoveNode *last_move = NULL;
    if (ll_chess_move_tail != last_move) {
      offset = scrollable_san;
      scrollbar_target = san_r.y + san_r.height - bar_r.height;
      last_move = ll_chess_move_tail;
    } else {
      float wheel = 0.0f;
      if (CheckCollisionPointRec(GetMousePosition(), san_r))
        wheel = GetMouseWheelMove(); // + UP, - DOWN

      float scroll_unit = 15.0f;
      float delta = wheel * scroll_unit;
      float next = scrollbar_target - delta;
      scrollbar_target = Clamp(next, san_r.y, san_r.y + san_r.height - bar_r.height);

      // This is ChatGPT. I'm not smart enough
      float smoothing = 20.0f;
      float alpha = 1.0f - expf(-smoothing * GetFrameTime());
      bar_r.y = Lerp(bar_r.y, scrollbar_target, alpha);

      // Map bar position to san_text offset
      float scrollable_san = san_text_height - san_r.height;
      float scrollable_bar = san_r.height - bar_r.height;
      assert(scrollable_bar > 0);

      float t = (bar_r.y - san_r.y) / scrollable_bar;
      offset  = t * scrollable_san;
    }
    DrawRectangleRounded(bar_r, 0.85f, 32, WHITE);
  }
  draw_san_text_moves(font, offset);
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

void highlight_square(void)
{
  bool found = false;
  ChessSquare square = {0};
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      square = chess_board.squares[y][x];
      if (CheckCollisionPointCircle(GetMousePosition(), square.center_proximity.center, square.center_proximity.r)) {
        found = true;
        break;
      }
    }
    if (found) break;
  }
  if (found) DrawRectangleLinesEx(square.rect, 3.0f, WHITE);
}

void draw_drag_and_place(void)
{
  dragging = dragging ? dragging : is_dragging_piece();

  if (dragging) {
    highlight_square();
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
  int mouse_cursor = MOUSE_CURSOR_DEFAULT;

  if (chess_board.state.hovering_piece || dragging || CheckCollisionPointRec(GetMousePosition(), fen_button_r)) {
    mouse_cursor = MOUSE_CURSOR_POINTING_HAND;
    chess_board.state.hovering_piece = false;
  }

  if (menu_state == MAIN) {
    if (CheckCollisionPointRec(GetMousePosition(), paste_fen.rect) || CheckCollisionPointRec(GetMousePosition(), paste_pgn.rect))
      mouse_cursor = MOUSE_CURSOR_IBEAM;
  }

  SetMouseCursor(mouse_cursor);
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
    occupied_square_color[LIGHT_TILE] : occupied_square_color[DARK_TILE];
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
  float piece_size = roundf(SQUARE_SIZE * 0.9);

  Rectangle rect = square->rect;
  Texture2D *piece_texture = &chess_pieces_texture[2*(square->piece.type - 1) + square->piece.color - 1];

  Rectangle source = {
    0, 0,
    piece_texture->width,
    piece_texture->height
  };

  Rectangle dest = {
    roundf(rect.x + (SQUARE_SIZE / 2.0f - piece_size / 2)),
    roundf(rect.y + (SQUARE_SIZE / 2.0f - piece_size / 2)),
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

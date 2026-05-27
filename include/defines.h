#ifndef DEFINES_H
#define DEFINES_H

// FEN
#define STARTING_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_MAX_LEN 86

#define WINDOW_FACTOR 67.5
#define WINDOW_WIDTH  (WINDOW_FACTOR * 16)
#define WINDOW_HEIGHT (WINDOW_FACTOR * 9)

#define NS 8
#define SQUARE_SIZE (((WINDOW_WIDTH) < (WINDOW_HEIGHT)) ? ((WINDOW_WIDTH) / (NS)) : ((WINDOW_HEIGHT) / (NS)))

#define background_color   (Color){0x18, 0x18, 0x18, 0xFF}
#define arrow_color        (Color){0xE5, 0x9E, 0x23, 0xD9}
#define paste_window_color (Color){0x37, 0x36, 0x33, 0xFF}
#define button_color       BROWN

#define MAX_SOUND_VOLUME 1.0f

#endif

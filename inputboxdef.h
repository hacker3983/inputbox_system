#ifndef _INPUTBOX_DEF_H
#define _INPUTBOX_DEF_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct input_character {
    char* utf8_char;
    size_t utf8_charlen;
    SDL_Rect canvas;
} inputchar_t;

typedef struct input_data {
    inputchar_t* characters;
    size_t cursor_pos, character_count, size;
} inputdata_t;

typedef struct inputbox {
    TTF_Font* font;
    int font_size;
    size_t render_pos;
    inputdata_t input;
    SDL_Color text_color;
    SDL_Color cursor_color;
    int cursor_width, cursor_height;
    SDL_Rect canvas, cursor_canvas;
    SDL_Color canvas_color;
    bool entered;
} inputbox_t;
#endif
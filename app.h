#ifndef _APP_H
#define _APP_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "inputboxdef.h"
#include "appstring.h"
#define COLOR_TOPARAM(color) color.r, color.g, color.b, color.a

enum app_cursor_types {
    APP_CURSOR_DEFAULT,
    APP_CURSOR_POINTER,
    APP_CURSOR_TYPEABLE
};

typedef struct app {
    TTF_Font* font;
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Cursor* cursors[3];
    int font_size, win_width, win_height,
        mouse_x, mouse_y, cursor_active, cursor_type;
    inputbox_t inputbox;
    char* name;
    bool mouse_clicked, mouse_down, quit;
} app_t;

void app_create(app_t* app);
bool app_rect_hover(app_t* app, SDL_Rect rect);
void app_setcursor(app_t* app, int cursor_type);
void app_run(app_t* app);
void app_render(app_t* app);
void app_destroy(app_t* app);
#endif 
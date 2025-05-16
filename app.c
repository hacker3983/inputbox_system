#include "app.h"
#include "inputbox.h"
#include "appconfig.h"

void app_create(app_t* app) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    app->window = SDL_CreateWindow(APP_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        APP_WIDTH, APP_HEIGHT, SDL_WINDOW_RESIZABLE);
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    app->font = TTF_OpenFont(APP_FONT, APP_FONTSIZE);
    app->font_size = APP_FONTSIZE;
    app->inputbox = inputbox_create(app->font, APP_FONTSIZE, (SDL_Color){0}, (SDL_Color){0x00, 0xff, 0x00, 0xff},
        (SDL_Color){0xff, 0xff, 0xff, 0xff}, 0, 0, 350, 50, 2, 50/2);
    app->cursors[APP_CURSOR_DEFAULT] = SDL_GetDefaultCursor();
    app->cursors[APP_CURSOR_POINTER] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    app->cursors[APP_CURSOR_TYPEABLE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
}

void app_run(app_t* app) {
    while(!app->quit) {
        while(SDL_PollEvent(&app->e)) {
            if(app->e.type == SDL_QUIT) {
                app->quit = true;
            } else if(app->e.type == SDL_MOUSEMOTION) {
                app->mouse_x = app->e.motion.x, app->mouse_y = app->e.motion.y;
            } else if(app->e.type == SDL_MOUSEBUTTONDOWN || app->e.type == SDL_MOUSEBUTTONUP) {
                app->mouse_x = app->e.button.x, app->mouse_y = app->e.button.y;
                if(app->e.type == SDL_MOUSEBUTTONUP) {
                    app->mouse_clicked = true;
                } else if(app->e.type == SDL_MOUSEBUTTONUP) {
                    app->mouse_down = true;
                }
            } else if(app->e.type == SDL_TEXTINPUT) {
                inputbox_handle_events(app, &app->inputbox);
            } else if(app->e.type == SDL_KEYDOWN) {
                inputbox_handle_events(app, &app->inputbox);
            }
        }
        SDL_GetWindowSize(app->window, &app->win_width, &app->win_height);
        SDL_SetRenderDrawColor(app->renderer, COLOR_TOPARAM(window_color));
        SDL_RenderClear(app->renderer);
        app_render(app);
        app_setcursor(app, APP_CURSOR_DEFAULT);
        SDL_RenderPresent(app->renderer);
        app->mouse_clicked = false;
        app->mouse_down = false;
    }
}


bool app_rect_hover(app_t* app, SDL_Rect rect) {
    if(app->mouse_x <= rect.x + rect.w && app->mouse_x >= rect.x
       && app->mouse_y <= rect.y + rect.h && app->mouse_y >= rect.y) {
        return true;
    }
    return false;
}

void app_setcursor(app_t* app, int cursor_type) {
    if(!app->cursor_active && cursor_type != APP_CURSOR_DEFAULT) {
        app->cursor_type = cursor_type;
        app->cursor_active = true;
    } else if(!app->cursor_active && cursor_type == APP_CURSOR_DEFAULT) {
        app->cursor_type = cursor_type;
    } else if(app->cursor_active && cursor_type == APP_CURSOR_DEFAULT) {
        app->cursor_active = false;
    }
    SDL_SetCursor(app->cursors[app->cursor_type]);
}

void app_render(app_t* app) {
    SDL_Rect welcome_canvas = {0}, name_canvas = {0};
    char* welcome_text = "Welcome to the input box system!";
    TTF_SetFontSize(app->font, APP_WELCOME_TEXTSIZE);
    TTF_SizeText(app->font, welcome_text, &welcome_canvas.w, &welcome_canvas.h);
    
    SDL_Color welcome_textcolor = {0xff, 0x00, 0x00, 0xff},
        name_color = {0xff, 0xff, 0xff, 0xff};
    char* name_text = "Name: ";
    TTF_SetFontSize(app->font, app->font_size);
    TTF_SizeText(app->font, name_text, &name_canvas.w, &name_canvas.h);

    SDL_Rect submit_btntext_canvas = {
        .x = 0, .y = 0,
        .w = 0, .h = 0
    };
    SDL_Color submit_btntext_color = {0x00, 0x00, 0x00, 0x00};
    char* submit_btntext = "Submit";
    TTF_SetFontSize(app->font, APP_SUBMIT_BTNSIZE);
    TTF_SizeText(app->font, submit_btntext, &submit_btntext_canvas.w, &submit_btntext_canvas.h);
    int submit_btnpadding_x = APP_SUBMIT_BTNPADDING_X,
        submit_btnpadding_y = APP_SUBMIT_BTNPADDiNG_Y,
        submit_btnwidth = submit_btntext_canvas.w + submit_btnpadding_x,
        submit_btnheight = submit_btntext_canvas.h + submit_btnpadding_y;
    SDL_Rect submit_btn = {
        .x = 0, .y = 0,
        .w = submit_btnwidth, .h = submit_btnheight
    };
    SDL_Color submit_btncolor = {0x00, 0xff, 0xff, 0xff};
    SDL_Rect greeting_msgcanvas = {
        .x = 0, .y = 0,
        .w = 0, .h = 0
    };
    SDL_Color greeting_msgcolor = {0xff, 0xff, 0xff, 0xff};  
    char* greeting_msg = NULL;
    if(app->name) {
        greeting_msg = calloc(18 + strlen(app->name) + 1, sizeof(char));
        sprintf(greeting_msg, "Nice to meet you %s!", app->name);
        TTF_SizeUTF8(app->font, greeting_msg, &greeting_msgcanvas.w, &greeting_msgcanvas.h);
    }
    SDL_Rect elements[] = {
        welcome_canvas,
        app->inputbox.canvas,
        submit_btn,
        greeting_msgcanvas
    };
    size_t element_count = sizeof(elements) / sizeof(SDL_Rect);
    int box_width = 0, box_paddingy = 50 * element_count,
        box_height = box_paddingy;
    for(size_t i=0;i<element_count;i++) {
        if(elements[i].w > box_width) {
            box_width = elements[i].w;
        }
        box_height += elements[i].h;
    }
    SDL_Rect box_container = {
        .x = 0, .y = 0,
        .w = box_width,
        .h = box_height
    };
    SDL_Color box_color = {0x12, 0x12, 0x12, 0xff};
    box_container.x = (app->win_width - box_container.w) / 2;
    box_container.y = (app->win_height - box_container.h) / 2;
    
    app->inputbox.canvas.x = box_container.x + (box_container.w - app->inputbox.canvas.w) / 2;
    name_canvas.x = app->inputbox.canvas.x - name_canvas.w - 10;
    box_container.w += app->inputbox.canvas.x - name_canvas.x;
    box_container.x = (app->win_width - box_container.w) / 2;
    
    welcome_canvas.x = box_container.x + (box_container.w - welcome_canvas.w) / 2,
    welcome_canvas.y = box_container.y + 40;

    
    app->inputbox.canvas.x = box_container.x + (box_container.w - app->inputbox.canvas.w) / 2;
    app->inputbox.canvas.y = welcome_canvas.y + welcome_canvas.h + 40;
    
    name_canvas.x = app->inputbox.canvas.x - name_canvas.w - 10;
    name_canvas.y = app->inputbox.canvas.y + (app->inputbox.canvas.h - name_canvas.h) / 2;
    
    SDL_SetRenderDrawColor(app->renderer, COLOR_TOPARAM(box_color));
    SDL_RenderDrawRect(app->renderer, &box_container);
    SDL_RenderFillRect(app->renderer, &box_container);

    TTF_SetFontSize(app->font, APP_WELCOME_TEXTSIZE);
    SDL_Surface* welcometext_surface = TTF_RenderText_Blended(app->font, welcome_text,
        welcome_textcolor);
    SDL_Texture* welcometext_texture = SDL_CreateTextureFromSurface(app->renderer, welcometext_surface);
    SDL_FreeSurface(welcometext_surface);
    SDL_RenderCopy(app->renderer, welcometext_texture, NULL, &welcome_canvas);
    SDL_DestroyTexture(welcometext_texture);

    TTF_SetFontSize(app->font, app->font_size);
    SDL_Surface* name_surface = TTF_RenderText_Blended(app->font, name_text, name_color);
    SDL_Texture* name_texture = SDL_CreateTextureFromSurface(app->renderer, name_surface);
    SDL_FreeSurface(name_surface);
    SDL_RenderCopy(app->renderer, name_texture, NULL, &name_canvas);
    SDL_DestroyTexture(name_texture);
    inputbox_display(app, &app->inputbox);

    submit_btn.x = box_container.x + (box_container.w - submit_btn.w) / 2;
    submit_btn.y = app->inputbox.canvas.y + app->inputbox.canvas.h + 40;
    submit_btntext_canvas.x = submit_btn.x + (submit_btn.w - submit_btntext_canvas.w) / 2;
    submit_btntext_canvas.y = submit_btn.y + (submit_btn.h - submit_btntext_canvas.h) / 2;
    
    SDL_SetRenderDrawColor(app->renderer, COLOR_TOPARAM(submit_btncolor));
    SDL_RenderDrawRect(app->renderer, &submit_btn);
    SDL_RenderFillRect(app->renderer, &submit_btn);
    if(app_rect_hover(app, submit_btn)) {
        app_setcursor(app, APP_CURSOR_POINTER);
        if(app->mouse_clicked) {
            free(app->name);
            app->name = inputbox_getinputdata(&app->inputbox);
            inputbox_clear(&app->inputbox);
            app->mouse_clicked = false;
        }
    }
    if(app->inputbox.entered) {
        free(app->name);
        app->name = inputbox_getinputdata(&app->inputbox);
        inputbox_clear(&app->inputbox);    
        app->inputbox.entered = false;
    }

    TTF_SetFontSize(app->font, APP_SUBMIT_BTNSIZE);
    SDL_Surface* submit_btntext_surface = TTF_RenderText_Blended(app->font, submit_btntext,
        submit_btntext_color);
    SDL_Texture* submit_btntext_texture = SDL_CreateTextureFromSurface(app->renderer, submit_btntext_surface);
    SDL_FreeSurface(submit_btntext_surface);
    SDL_RenderCopy(app->renderer, submit_btntext_texture, NULL, &submit_btntext_canvas);
    SDL_DestroyTexture(submit_btntext_texture);
    if(!greeting_msg) {
        return;
    }
    greeting_msgcanvas.x = box_container.x + (box_container.w - greeting_msgcanvas.w) / 2;
    greeting_msgcanvas.y = submit_btn.y + submit_btn.h + 40;
    SDL_Surface* greeting_msgsurface = TTF_RenderText_Blended(app->font, greeting_msg, greeting_msgcolor);
    SDL_Texture* greeting_msgtexture = SDL_CreateTextureFromSurface(app->renderer, greeting_msgsurface);
    SDL_FreeSurface(greeting_msgsurface);
    SDL_RenderCopy(app->renderer, greeting_msgtexture, NULL, &greeting_msgcanvas);
    SDL_DestroyTexture(greeting_msgtexture);
    free(greeting_msg);
}

void app_destroy(app_t* app) {
    free(app->name);
    for(size_t i=0;i<APP_CURSOR_TYPEABLE+1;i++) {
        SDL_FreeCursor(app->cursors[i]);
    }
    TTF_CloseFont(app->font);
    TTF_Quit();
    inputbox_destroy(&app->inputbox);
    SDL_DestroyWindow(app->window);
    SDL_DestroyRenderer(app->renderer);
    SDL_Quit();
}
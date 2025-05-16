#include "inputbox.h"

void inputbox_init(inputbox_t* inputbox, TTF_Font* font, int font_size, SDL_Color color, SDL_Color cursor_color,
    SDL_Color text_color, int x, int y, int width, int height, int cursor_width, int cursor_height) {
    inputbox->font = font;
    inputbox->font_size = font_size;
    inputbox->text_color = text_color;
    inputbox->canvas = (SDL_Rect) {x, y, width, height};
    inputbox->canvas_color = color;
    inputbox->cursor_color = cursor_color;
    inputbox->cursor_width = cursor_width;
    inputbox->cursor_height = cursor_height;
}

inputbox_t inputbox_create(TTF_Font* font, int font_size, SDL_Color color, SDL_Color cursor_color, SDL_Color text_color,
    int x, int y, int width, int height, int cursor_width, int cursor_height) {
    inputbox_t new_inputbox = {0};
    inputbox_init(&new_inputbox, font, font_size, color, cursor_color, text_color, x, y, width, height,
        cursor_width, cursor_height);
    return new_inputbox;
}

void inputbox_addinputchar(inputbox_t* inputbox, char* utf8_char) {
    inputdata_t* input = &inputbox->input;
    size_t new_count = input->character_count + 1;
    inputchar_t* new_characters = realloc(input->characters, new_count * sizeof(inputchar_t));
    if(!new_characters) {
        return;
    }
    new_characters[new_count-1].utf8_char = utf8_char;
    TTF_SetFontSize(inputbox->font, inputbox->font_size);
    TTF_SizeUTF8(inputbox->font, utf8_char, &new_characters[new_count-1].canvas.w,
        &new_characters[new_count-1].canvas.h);
    new_characters[new_count-1].utf8_charlen = strlen(utf8_char);
    input->size += new_characters[new_count-1].utf8_charlen;
    for(size_t i=new_count-1;i>input->cursor_pos;i--) {
        inputchar_t current_character = new_characters[i];
        new_characters[i] = new_characters[i-1];
        new_characters[i-1] = current_character;
    }
    input->cursor_pos++;
    input->characters = new_characters;
    input->character_count = new_count;
}

void inputbox_addinputdata(inputbox_t* inputbox, const char* utf8_string) {
    size_t utf8_stringsize = strlen(utf8_string);
    for(size_t i=0;i<utf8_stringsize;i++) {
        char* utf8_char = app_string_getutf8_char(utf8_string, &i, utf8_stringsize);
        inputbox_addinputchar(inputbox, utf8_char);
    }
}

char* inputbox_getinputdata(inputbox_t* inputbox) {
    if(!inputbox->input.characters) {
        return NULL;
    }
    char* new_data = calloc(inputbox->input.size+1, sizeof(char));
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        strcat(new_data, inputbox->input.characters[i].utf8_char);
    }
    return new_data;
}

void inputbox_backspace(inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(!input->characters || !input->cursor_pos) {
        return;
    }
    for(size_t i=input->cursor_pos-1;i<input->character_count-1;i++) {
        inputchar_t current_character = input->characters[i];
        input->characters[i] = input->characters[i+1];
        input->characters[i+1] = current_character;
    }
    input->size -= input->characters[input->character_count-1].utf8_charlen;
    input->cursor_pos--;
    input->character_count--;
    free(input->characters[input->character_count].utf8_char);
    if(!input->character_count) {
        free(input->characters);
        input->characters = NULL;
        return;
    }
    input->characters = realloc(input->characters, input->character_count * sizeof(inputchar_t));
}

void inputbox_handle_events(app_t* app, inputbox_t* inputbox) {
    switch(app->e.type) {
        case SDL_TEXTINPUT:
            inputbox_addinputdata(inputbox, app->e.text.text);
            char* data = inputbox_getinputdata(inputbox);
            printf("Input data is %s\n", data);
            free(data);
            break;
        case SDL_KEYDOWN:
            switch(app->e.key.keysym.sym) {
                case SDLK_LEFT:
                    if(inputbox->input.cursor_pos) {
                        inputbox->input.cursor_pos--;
                    }
                    break;
                case SDLK_RIGHT:
                    if(inputbox->input.cursor_pos < inputbox->input.character_count) {
                        inputbox->input.cursor_pos++;
                    }
                    break;
                case SDLK_BACKSPACE:
                    inputbox_backspace(inputbox);
                    char* data = inputbox_getinputdata(inputbox);
                    printf("Input data is %s\n", data);
                    free(data);
                    break;
                case SDLK_RETURN:
                    inputbox->entered = true;
                    break;
            }
            break;
    }
}

void inputbox_rendercharacters(app_t* app, inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    for(size_t i=inputbox->render_pos;i<input->character_count;i++) {
        inputchar_t* character = &input->characters[i];
        SDL_Surface* char_surface = TTF_RenderUTF8_Blended(inputbox->font, character->utf8_char,
            inputbox->text_color);
        SDL_Texture* char_texture = SDL_CreateTextureFromSurface(app->renderer, char_surface);
        SDL_FreeSurface(char_surface);
        if(i == inputbox->render_pos) {
            character->canvas.x = inputbox->canvas.x;
        } else {
            character->canvas.x = input->characters[i-1].canvas.x
                + input->characters[i-1].canvas.w;
        }
        character->canvas.y = inputbox->canvas.y + (inputbox->canvas.h - character->canvas.h) / 2;
        if(input->cursor_pos == i) {
            character->canvas.x += inputbox->cursor_width;
        }
        SDL_RenderCopy(app->renderer, char_texture, NULL, &character->canvas);
        SDL_DestroyTexture(char_texture);
        if(character->canvas.x + character->canvas.w >= inputbox->canvas.x + inputbox->canvas.w) {
            break;
        }
    }
    inputbox->cursor_canvas.x = inputbox->canvas.x;
    if(input->cursor_pos == inputbox->render_pos && inputbox->render_pos) {
        inputbox->render_pos--;
    } else if(input->characters && input->cursor_pos) {
        inputbox->cursor_canvas.x = input->characters[input->cursor_pos-1].canvas.x + input->characters[input->cursor_pos-1].canvas.w;
    }
    if(input->cursor_pos < inputbox->render_pos) {
        inputbox->render_pos--;
    } else if(inputbox->cursor_canvas.x + inputbox->cursor_canvas.w >
        inputbox->canvas.x + inputbox->canvas.w) {
        inputbox->render_pos++;
    }
}

void inputbox_rendercursor(app_t* app, inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    inputbox->cursor_canvas.w = inputbox->cursor_width;
    inputbox->cursor_canvas.h = inputbox->cursor_height;
    inputbox->cursor_canvas.y = inputbox->canvas.y + (inputbox->canvas.h - inputbox->cursor_canvas.h)/2;
    SDL_SetRenderDrawColor(app->renderer, COLOR_TOPARAM(inputbox->cursor_color));
    SDL_RenderDrawRect(app->renderer, &inputbox->cursor_canvas);
    SDL_RenderFillRect(app->renderer, &inputbox->cursor_canvas);
}

void inputbox_display(app_t* app, inputbox_t* inputbox) {
    SDL_SetRenderDrawColor(app->renderer, COLOR_TOPARAM(inputbox->canvas_color));
    SDL_RenderDrawRect(app->renderer, &inputbox->canvas);
    SDL_RenderFillRect(app->renderer, &inputbox->canvas);
    inputbox_rendercharacters(app, inputbox);
    inputbox_rendercursor(app, inputbox);
}

void inputbox_clear(inputbox_t* inputbox) {
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        free(inputbox->input.characters[i].utf8_char);
    }
    free(inputbox->input.characters); inputbox->input.characters = NULL;
    inputbox->input.size = 0;
    inputbox->input.cursor_pos = 0;
    inputbox->input.character_count = 0;
}

void inputbox_destroy(inputbox_t* inputbox) {
    inputbox_clear(inputbox);
}
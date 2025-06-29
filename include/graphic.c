#include "graphic.h"

void drawTextWithFont(const char *text, float x, float y, TTF_Font *font, SDL_Renderer *renderer, SDL_Color fg, const char* quality)
{
    size_t length = strlen(text);
    SDL_Surface *s;

    if (strcmp(quality, "high") == 0)
        s = TTF_RenderText_Blended(font, text, length, fg);
    else if (strcmp(quality, "low") == 0)
        s = TTF_RenderText_Solid(font, text, length, fg);
    else
        s = TTF_RenderText_Solid(font, text, length, fg);
    
    
    if (s != NULL)
    {
        SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
        if (t != NULL)
        {
            SDL_FRect r;
            r.x = x;
            r.y = y;
            r.w = s->w;
            r.h = s->h;
            SDL_RenderTexture(renderer, t, NULL, &r);
            SDL_DestroyTexture(t);
        }
        SDL_DestroySurface(s);
    }
}

int drawRect(float x, float y, float w, float h, SDL_Renderer *renderer, int r, int g, int b, int a, const char* type)
{   
    if (renderer == NULL || type == NULL) {
        perror("No renderer or type instance.\n");
        return -1;
    }

    SDL_FRect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    if (strcmp(type, "filled") == 0)
        SDL_RenderFillRect(renderer, &rect);
    else if (strcmp(type, "border") == 0)
        SDL_RenderRect(renderer, &rect);
    else
        perror("Error: you need to choose between border or filled.\n");

    return 0;
}

void drawHint(const char* text, float x, float y, float rectW, float rectH, TTF_Font* font, SDL_Renderer* renderer) {
    if (text == NULL || font == NULL) return;
    
    SDL_Color SDL_RECT_COLOR = { 0, 0, 0, 255 };
    SDL_Color SDL_RECT_BORDER_COLOR = { 255, 255, 255, 255 };

    drawRect(x, y, rectW, rectH, renderer, SDL_RECT_COLOR.r, SDL_RECT_COLOR.g, SDL_RECT_COLOR.b, SDL_RECT_COLOR.a, "filled");
    drawRect(x + 5, y + 5, rectW - 10, rectH - 10, renderer, SDL_RECT_BORDER_COLOR.r, SDL_RECT_BORDER_COLOR.g, SDL_RECT_BORDER_COLOR.b, SDL_RECT_BORDER_COLOR.a, "border");
    
    int wrap_width = (int)rectW - 20;
    drawWrappedText(text, x + 10, y + 10, font, SDL_RECT_BORDER_COLOR, renderer, wrap_width);
}

void drawWrappedText(const char *text, float x, float y, TTF_Font* font, SDL_Color fg, SDL_Renderer* renderer, int wrap_width_px) {
    if (text == NULL || font == NULL) return;
    SDL_Surface *s = TTF_RenderText_Blended_Wrapped(font, text, strlen(text), fg, wrap_width_px);
    if (s != NULL)
    {
        SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
        if (t != NULL)
        {
            SDL_FRect r;
            r.x = x;
            r.y = y;
            r.w = s->w;
            r.h = s->h;
            SDL_RenderTexture(renderer, t, NULL, &r);
            SDL_DestroyTexture(t);
        }
        SDL_DestroySurface(s);
    }
}

void drawGrid(Grid* grid, SDL_Renderer* renderer, SelectionMode selection_mode, Word* selected_word) {
    if (grid == NULL) return;
    SDL_Color rectBorderColor = { 0, 0, 0, 255 };

    for(int i = 0; i < grid->nrow; i++) {
        for(int j = 0; j < grid->ncol; j++) {
            SDL_Color rectBackgroundColor = { 255, 255, 255, 255 }; 
            Cell* cell = &grid->list_cells[i][j];
            
            if (selection_mode == WORD_MODE && selected_word && selected_word->is_placed) {
                if (findWordAt(i, j, words, words_count, selected_word->orientation) == selected_word) {
                    rectBackgroundColor = (SDL_Color){ 173, 216, 230, 255 };
                }

            } else if (selection_mode == LETTER_MODE && selected_word && selected_word->is_placed) {
                if (findWordAt(i, j, words, words_count, selected_word->orientation) == selected_word) {
                    rectBackgroundColor = (SDL_Color){ 211, 211, 211, 255 };
                }
            }

            if (i == grid->ai && j == grid->aj) 
                rectBackgroundColor = (SDL_Color){ 255, 255, 0, 255 };
            
            drawRect(cell->x, cell->y, cell->w, cell->h, renderer, rectBackgroundColor.r, rectBackgroundColor.g, rectBackgroundColor.b, rectBackgroundColor.a, "filled");
            drawRect(cell->x, cell->y, cell->w, cell->h, renderer, rectBorderColor.r, rectBorderColor.g, rectBorderColor.b, rectBorderColor.a, "border");
            
            char current_letter = cell->current_letter;
            if (current_letter == '\0') continue;
            if (current_letter >= 'A' && current_letter <= 'Z') {
                    int texture_index = current_letter - 'A';
                    SDL_Texture* letter_texture = grid->letter_textures_cache[texture_index];
                    if (letter_texture != NULL) {
                        SDL_FRect dest_rect;
                        dest_rect.x = cell->x;
                        dest_rect.y = cell->y;

                        SDL_GetTextureSize(letter_texture, &dest_rect.w, &dest_rect.h); // antes era SDL_GetTextureSize

                        dest_rect.x = cell->x + (cell->w - dest_rect.w) / 2.0f;
                        dest_rect.y = cell->y + (cell->h - dest_rect.h) / 2.0f;
                        SDL_RenderTexture(renderer, letter_texture, NULL, &dest_rect);
                    }
            }
        }
    }
}

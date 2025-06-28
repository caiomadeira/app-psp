#include "graphic.h"
#include "common.h"

void drawTextWithFont(const char* text, float x, float y, TTF_Font* font, SDL_Renderer *renderer, SDL_Color fg)
{
    size_t length = strlen(text);
    SDL_Surface* s = TTF_RenderText_Solid(font, text, length, fg);
    if (s != NULL) {
        SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
        if (t != NULL) {
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
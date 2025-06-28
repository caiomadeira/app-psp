#include "graphic.h"

void drawTextWithFont(const char *text, float x, float y, TTF_Font *font, SDL_Renderer *renderer, SDL_Color fg)
{
    size_t length = strlen(text);
    SDL_Surface *s = TTF_RenderText_Solid(font, text, length, fg);
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

int drawRect(float x, float y, float w, float h, SDL_Renderer *renderer, int r, int g, int b, int a)
{
    if (renderer == NULL)
    {
        perror("No renderer instance.\n");
        return -1;
    }

    if (r == NULL || g == NULL || b == NULL || a == NULL)
    {
        perror("No r,g,b,a instance.\n");
        return -1;
    }

    SDL_FRect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
    return 0;
}
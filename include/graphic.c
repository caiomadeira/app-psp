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

void drawGrid(Grid* grid, SDL_Renderer* renderer)
{    
    if (grid == NULL) return;
    for(int i = 0; i < grid->nrow*grid->ncol; i++) {
        Cell* cell = grid->list_cells[i];
        if (cell != NULL)
            drawRect(cell->x, cell->y, cell->w - 2, cell->h - 2, renderer, 255, 255, 255, 255, "filled");
    }
}
#ifndef graphic_h
#define graphic_h
#include "logic.h"
#include "common.h"

void drawTextWithFont(const char* text, float x, float y, TTF_Font* font, SDL_Renderer *renderer, SDL_Color fg);
int drawRect(float x, float y, float w, float h, SDL_Renderer *renderer, int r, int g, int b, int a, const char* type);
void drawGrid(Grid* grid, SDL_Renderer* renderer);
void drawHint(const char* text, float x, float y, float rectW, float rectH, TTF_Font* font, SDL_Renderer* renderer);

#endif
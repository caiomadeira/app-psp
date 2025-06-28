#ifndef graphic_h
#define graphic_h
#include "common.h"

void drawTextWithFont(const char* text, float x, float y, TTF_Font* font, SDL_Renderer *renderer, SDL_Color fg);

#endif
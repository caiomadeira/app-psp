#ifndef LOGIC_H
#define LOGIC_H

#include "common.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*
[   *Cell(0, 0, 'A'), *Cell(10, 10, 'B'), *Cell(20, 20, 'A'),
    *Cell(0, 0, 'O'), *Cell(10, 10, 'V'), *Cell(20, 20, 'Y'),
    *Cell(0, 0, 'R'), *Cell(10, 10, 'F'), *Cell(20, 20, 'Z'),
    *Cell(0, 0, 'B'), *Cell(10, 10, 'E'), *Cell(20, 20, 'P')    ]
*/

#define START_LETTER 'A'
#define END_LETTER 'Z'

typedef struct {
    float x;
    float y;
    float w;
    float h;
    char current_letter;
} Cell;

typedef struct {
    float x;
    float y;
    float w;
    float h;
    float padding;
} GridArea;

typedef struct {
    Cell** list_cells;
    int ai; // selected row
    int aj; // selected col
    int nrow;
    int ncol;
    GridArea* gridArea;
    TTF_Font* font;
    int font_size;
    SDL_Color font_color;
} Grid;

void print1d(char *v, int size);
void printGrid(Grid* grid, int size);
Cell* newCell(int x, int y, int w, int h, char letter);
Grid* newGrid(int nrow, int ncol, GridArea* gridArea);
GridArea* newGridArea(int x, int y, int w, int h, int padding);
void moveGridSelection(Grid* grid, int d_row, int d_col);
void moveCellLetterSelection(Grid* grid);
char* init_letters(int start, int end);

#endif
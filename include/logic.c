#include "logic.h"

void printGrid(Grid* grid, int size) {
    printf("=========== Grid Layout ==============\n");
    for(int i = 0; i < grid->nrow * grid->ncol; i++) {
        if (grid->list_cells[i] != NULL) {
            printf("Cell[%d]: x=%.f, y=%.f, letter=%c\n", i, grid->list_cells[i]->x, grid->list_cells[i]->y, grid->list_cells[i]->current_letter);
        }
    }
    printf("=================================\n");
}

Cell* newCell(int x, int y, int w, int h, char letter) {
    Cell* cell = (Cell*)malloc(sizeof(Cell));
    if (cell != NULL) {
        cell->w = w;
        cell->h = h;
        cell->x = x;
        cell->y = y;
        cell->current_letter = letter;
    }
    return cell;
}

GridArea* newGridArea(int x, int y, int w, int h, int padding) {
    GridArea* gridArea = (GridArea*)malloc(sizeof(GridArea));
    if (gridArea == NULL) return NULL;
    gridArea->x = x;
    gridArea->y = y;
    gridArea->w = w;
    gridArea->h = h;
    gridArea->padding = padding;
    return gridArea;
}

Grid* newGrid(int nrow, int ncol, GridArea* gridArea) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (grid == NULL) return NULL;

    grid->nrow = nrow;
    grid->ncol = ncol;

    grid->list_cells = (Cell**)malloc(nrow*ncol*sizeof(Cell*));
    if (grid->list_cells == NULL) {
        free(grid);
        return NULL;
    }
    grid->gridArea = gridArea;

    // calc a largura d cada cell
    float total_padding_w = grid->gridArea->padding * (ncol - 1);
    float cell_w = (grid->gridArea->w - total_padding_w) / ncol;

    // calc a altura d cada cell
    float total_padding_h = grid->gridArea->padding * (nrow - 1);
    float cell_h = (grid->gridArea->h - total_padding_h) / nrow;
    char letter = 'A';

    for(int i = 0; i < nrow; i++) {
        for(int j = 0; j < ncol; j++) {
            float x = grid->gridArea->x + (j * (cell_w + grid->gridArea->padding));
            float y = grid->gridArea->y + (i * (cell_h + grid->gridArea->padding));

            int index = i * ncol + j;

            grid->list_cells[index] = newCell(x, y, cell_w, cell_h, letter);

            // free in case of fail
            if (grid->list_cells[index] == NULL) {
                for(int k = 0; k < index; k++) {
                    free(grid->list_cells[k]);
                }
                free(grid->list_cells);
                free(grid);
                return NULL;
            }
        }
    }
    return grid;
}

void print1d(char *v, int size) {
    for(int i = 0; i < size; i++) {
        printf("%c, ", v[i]);
    }
    printf("\n");
}

char* init_letters(int start, int end) {
    int i = 0;
    size_t size = (end - start) + 1;
    printf("Letters array size: %d\n", (int)size);
    char* letters = (char*)malloc((int)size*sizeof(char));
    if (letters != NULL) {
        while(i < size) {
            *(letters + i) = start;
            start++;
            i++;
        }
        print1d(letters, size);
        return letters;
    }
    return NULL;
}
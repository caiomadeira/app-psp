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

// d_row = pode ser -1 (cima), 1 (baixo) ou 0 eh o deslocamento
// d_col pode ser -1 (esqueda), 1 (direita) ou 0
void moveGridSelection(Grid* grid, int d_row, int d_col) {
    if (grid == NULL) return;
    int new_row = grid->ai + d_row;
    int new_col = grid->aj + d_col;

    // aqui eu sou verifico os limites pra 1. linha e 2. coluna
    if (new_row >= 0 && new_row < grid->nrow) 
        grid->ai = new_row;

    if (new_col >= 0 && new_col < grid->ncol)
        grid->aj = new_col;
}

Grid* newGrid(int nrow, int ncol, GridArea* gridArea) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (grid == NULL) return NULL;

    grid->nrow = nrow;
    grid->ncol = ncol;
    grid->ai = 0;
    grid->aj = 0;
    grid->font = NULL;
    grid->font_size = 45;

    // alloc 2D matrix of cells
    grid->list_cells = (Cell**)SDL_malloc(nrow*sizeof(Cell*));
    if (grid->list_cells == NULL) {
        SDL_free(grid);
        return NULL;
    }
    for(int i = 0; i < nrow; i++) {
        grid->list_cells[i] = (Cell*)SDL_calloc(ncol, sizeof(Cell));
        if (grid->list_cells[i] == NULL) {
            // codigo pra limpeza que nao quero escrever agora
        }
    }

    // define a gridArea (struct com os dados das dimensoes)
    grid->gridArea = gridArea;
    float cell_w = grid->gridArea->w / ncol;
    float cell_h = grid->gridArea->h / nrow;

    for(int i = 0; i < nrow; i++) {
        for(int j = 0; j < ncol; j++) {
            Cell* cell = &grid->list_cells[i][j];

            // relativo ao tamanho da celula (- padding)
            cell->w = cell_w - grid->gridArea->padding;
            cell->h = cell_h - grid->gridArea->padding;

            // relativo a posicao com base no indice e no tamanho da celula
            cell->x = grid->gridArea->x + (j * cell_w) + (grid->gridArea->padding / 2.0f);
            cell->y = grid->gridArea->y + (i * cell_h) + (grid->gridArea->padding / 2.0f);

            cell->current_letter = ' ';
        }
    }
    return grid;
}

// void moveCellLetterSelection(Grid* grid) {
//     for(int i = 0; i < grid->nrow; i++) {
//         for(int j = 0; j < grid->ncol; j++) {
//             char current_letter = grid->list_cells[i][j].current_letter;
//             if (current_letter >= 'A' && current_letter <= 'Z') 
//                 current_letter++;
//             else
//                 current_letter = 'A';
//         }
//     }
// }

void moveCellLetterSelection(Grid* grid) {
    if (grid == NULL) return;

    Cell* selected_cell = &grid->list_cells[grid->ai][grid->aj];
    char letter = selected_cell->current_letter;

    if (letter == 'Z') selected_cell->current_letter = 'A';
    else if (letter >= 'A' && letter < 'Z') selected_cell->current_letter++;
    else selected_cell->current_letter = 'A';
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
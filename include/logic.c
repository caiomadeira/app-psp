#include "logic.h"
#include<time.h>

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

    grid->total_words = 5; // TODO: Pensar em algum calculo pra definir quantas palavras cabem a partir do mxn da matriz
    grid->words = (Word**)malloc(grid->total_words*sizeof(Word*));
    if (grid->words == NULL) return NULL;

    // define o tamanho maximo que uma palavra pode alcançar baseado na maior dimensao da matriz
    int max_word_len = 0;
    if (grid->ncol > grid->nrow) {
        max_word_len = grid->ncol;
    } else {
        max_word_len = grid->nrow
    }

    for(int i = 0; i < max_word_len; i++) {
        grid->words[i] = NULL; // inicializo todas as palavras com null
    }

    return grid;
}

void moveCellLetterSelection(Grid* grid) {
    if (grid == NULL) return;

    Cell* selected_cell = &grid->list_cells[grid->ai][grid->aj];
    char letter = selected_cell->current_letter;

    if (letter == 'Z') selected_cell->current_letter = 'A';
    else if (letter >= 'A' && letter < 'Z') selected_cell->current_letter++;
    else selected_cell->current_letter = 'A';
}

void print1d(char *v, int size) {
    for(int i = 0; i < size; i++) printf("%c, ", v[i]);
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

void populateGridWithWords(Grid* grid, Word words[], int word_count Word) {
    if (grid == NULL || words == NULL) return;

    for(int r = 0; r < grid->nrow; r++) {
        for(int c = 0; c < grid->ncol; c++) {
            grid->list_cells[r][c].current_letter = '\0'; // iniciando todas as celulas com caractere nulo
        }
    }
    bool word_end_pos_defined = false;

    // aqui estou iterando sobre cada palavra no banco
    for(int i = 0; i < word_count; i++) {
        Word* current_word = &words[i]; // banco de palavras
        int word_len = strlen(current_word->word);
        
        // verifica se a palavra é maior que 2
        if (word_len > 2) {
            // verificando se esta dentro do grid da grade w definindo a orientação
            // verifica se o tamanho da palavra é menor ou igual ao numero de COLUNAS
            if (word_len <= grid->ncol) {
                current_word->orientation = HORIZONTAL;

                // checa se essa é a primeira palavra a ser adicionada.
                if (grid->words[0] == NULL) {

                    // se essa é a primeira palavra entao temos mais opções pra adicionar onde quisermos na matriz
                    /*
                    Vamos gerar uma primeira posição aleatoria.
                    É importante notar que ela nao pode ser sempre muito aleatoria.
                    Quando eu escolher uma posicao pra primeira letra, as outras devem caber no resto da row ou coluna subsequente
                    de modo que não ultrapasse os limites da grade
                    */
                   if (word_end_pos_defined == false) {
                        goto define_end_pos;
                   } 

                    define_end_pos:
                        srand(time(NULL));
                        int r = rand() % (grid->nrow * grid->ncol); // numero alterorio entre 0 e grid->nrow * grid->ncol
                        current_word->start = r;
                        current_word->end = word_len - current_word->start;
                        if ((current_word->end < grid->nrow && current_word->end < grid->ncol)) {
                            word_end_pos_defined = true;
                        }
                    
                    if (word_end_pos_defined) {
                        for(int k = 0; k < word_len; k++) {
                            // se a palavra foi definida como tendo orientação horizontal
                            if (current_word->orientation == HORIZONTAL) {
                                current_word->start += k; // o indice i de aij eh incrimentado
                            } else {
                                start_row += k; // se nao o j eh incrementado
                            }

                            // por fim verifico se a celula de destino esta dentro dos limites da grade
                            if (start_row < grid->nrow && start_col < grid->ncol) {
                                grid->list_cells[start_row][start_col].current_letter = toupper(current_word->word[k]); // coloco em minuscula p/ evitar problemas
                            }
                        }
                    }


                //    for(int j = 0; j < grid->nrow; j++) {
                //         for(int k = 0; k < grid->ncol; k++) {
                            
                //         }
                //    }
                }
            } 
            // verifica se o tamanho da palavra é menor ou igual ao numero de LINHAS
            else if (word_len >= grid->nrow) {

            }
        }
    }
}

// void populateGridWithWords(Grid* grid, Word words[], int word_count) {
//     if (grid == NULL || words == NULL) return;

//     for(int r = 0; r < grid->nrow; r++) {
//         for(int c = 0; c < grid->ncol; c++) {
//             grid->list_cells[r][c].current_letter = '\0'; // iniciando todas as celulas com caractere nulo
//         }
//     }

//     // aqui estou iterando sobre cada palavra no banco
//     for(int i = 0; i < word_count; i++) {
//         Word* current_word = &words[i]; // banco de palavras
//         int word_len = strlen(current_word->word);
//         int start_row = current_word->i; // indice inicial da palavra
//         int start_col = current_word->j; // indice final da palabra

//         // aqui itero sobre cada LETRA da palabra
//         for(int k = 0; k < word_len; k++) {

//             // se a palavra foi definida como tendo orientação horizontal
//             if (current_word->orientation == HORIZONTAL) {
//                 start_col += k; // o indice i de aij eh incrimentado
//             } else {
//                 start_row += k; // se nao o j eh incrementado
//             }

//             // por fim verifico se a celula de destino esta dentro dos limites da grade
//             if (start_row < grid->nrow && start_col < grid->ncol) {
//                 grid->list_cells[start_row][start_col].current_letter = toupper(current_word->word[k]); // coloco em minuscula p/ evitar problemas
//             }
//         }
//     }
// }
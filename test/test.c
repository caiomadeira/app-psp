#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "../include/logic.h"

int main(void)
{
    int start = 65; // A
    int end = 90; // Z

    char* letters = init_letters(start, end);
    Grid* grid1 = newGrid(4, 4);
    printGrid(grid1, 4 * 4);
    return 0;
}
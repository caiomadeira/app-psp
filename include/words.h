#ifndef WORDS_H
#define WORDS_H

#include "common.h"

typedef enum {
    WORD_MODE,
    LETTER_MODE,
} SelectionMode;

typedef enum {
    HORIZONTAL,
    VERTICAL
} WordOrientation;

typedef struct {
    int index;
    char* hint;
    char* word;
    int word_size;
    int hint_size;
    // int start; 
    //int end;
    bool is_placed;
    int pos_final_i;
    int pos_final_j;
    WordOrientation orientation;
} Word;

extern Word words[];
extern int words_count;

Word* findWordAt(int r, int c, Word words[], int word_count, WordOrientation orientation);
Word* newWord(int index, char* hint, char* word);
void destroyWord(Word* w);

#endif
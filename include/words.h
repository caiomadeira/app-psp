#ifndef WORDS_H
#define WORDS_H

#include "common.h"

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
    int start; 
    int end;
    WordOrientation orientation;
} Word;

Word* newWord(int index, char* hint, char* word);
void destroyWord(Word* w);

#endif
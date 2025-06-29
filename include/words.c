#include "words.h"

Word words[] = {
    {0, "Some eggs are made of.", "chocolate"},
    {1, "Can be used to talk or to pay debts.", "telephone"},
    {0, "One man walked above.", "water"},
    {0, "A star.", "sun"},
};

int words_count = sizeof(words) / sizeof(words[0]);

Word* newWord(int index, char* hint, char* word) {
    Word* w = (Word*)malloc(sizeof(Word));
    if (w == NULL) {
        return NULL;
    }

    w->hint_size = strlen(hint);
    w->hint = (char*)malloc(w->hint_size * 1);
    if (w->hint == NULL) {
        free(w);
        return NULL;
    }
    
    w->word_size = strlen(word);
    if (w->word == NULL) {
        free(w->hint);
        free(w);
        return NULL;
    }
    strcpy(w->hint, hint);
    strcpy(w->word, word);
    w->orientation = NULL;
    w->index = index;
    
    w->start = 0;
    w->end = 0;
    
    return w;
}

void destroyWord(Word* w) {
    if (w == NULL) return;
    free(w->hint);
    free(w->word);
    free(w);
}
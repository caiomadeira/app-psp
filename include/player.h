#ifndef player_h
#define player_h

#include "common.h"

typedef struct player {
    float score;
    char name[40];
} Player;

Player* init_player(void);

#endif
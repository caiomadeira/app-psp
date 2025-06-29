#include "player.h"


Player* init_player(void) 
{
    Player* player = (Player*)malloc(sizeof(Player));
    if (player == NULL) return NULL;
    player->score = 0;
    return player;
}
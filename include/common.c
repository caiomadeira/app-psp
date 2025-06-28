#include "common.h"

// Print debug
void printDebug(const char* str, int delay)
{
    pspDebugScreenInit();
    pspDebugScreenClear();
    pspDebugScreenSetXY(20, 7);
    pspDebugScreenPrintf(str);
    SDL_Delay(delay);
}

// Pega o caminho do executável e muda o diretório de trabalho
void getBinaryPath(int argc, char *argv[])
{
    if (argc > 0 && argv[0] != NULL)
    {
        char* path = strdup(argv[0]);
        char* last_path = strrchr(path, '/');
        if (last_path)
        {
            *last_path = '\0';
            chdir(path);
        }
        free(path);
    }
}

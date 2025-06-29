#include "include/common.h"
#include "include/player.h"
#include "include/audio.h"
#include "include/graphic.h"
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

// COLORS SDL
SDL_Color SDL_WHITE = { 255, 255, 255, 255 };
SDL_Color SDL_PURPLE = { 192, 92, 255, 255 };

typedef struct app {
	SDL_Window *window;
	SDL_GLContext context;
    GameState currentstate;
    SceCtrlData pad;
    Player *player;
    SDL_Texture *background_texture;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Font *hint_font;
    SceCtrlData prev_pad;
    Grid* grid;
    const char* current_hint;
    WordOrientation current_orientation;
} app_t;

// inicializa com as palavras
/*
    Preciso usar delcarações externas pra acessar o banco.
    TODO: INvestigar o porque
*/
extern Word words[];
extern int words_count;

void updateCurrentHint(app_t* a) {
    if (!a || !a->grid) return;

    int r = a->grid->ai;
    int c = a->grid->aj;

    if (a->grid->list_cells[r][c].current_letter == '\0') {
        a->current_hint = NULL;
        return;
    }

    Word* preferred_word = findWordAt(r, c, words, words_count, a->current_orientation);
    Word* alternative_word = findWordAt(r, c, words, words_count, (a->current_orientation == HORIZONTAL) ? VERTICAL : HORIZONTAL);
    if (preferred_word) {
        a->current_hint = preferred_word->hint;
    } else if (alternative_word) {
        a->current_hint = alternative_word->hint;
    } else {
        a->current_hint = NULL;
    }
}

/*
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *  ON READY
 * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
	app_t *a;
    getBinaryPath(argc, argv);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

    if (init_native_audio(TEST_WAV) != 0) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }
    
    if (TTF_Init() == -1) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

	a = (app_t*)SDL_calloc(1, sizeof(app_t)); // usando calloc p/ zerar a memoria
	if (a == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

    SDL_zero(a->prev_pad);

    a->window = SDL_CreateWindow(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, INIT_OPEN_GL_WINDOW); // janela de renderizacao c open_gl
    if (a->window == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

    a->renderer = SDL_CreateRenderer(a->window, NULL);
    if (a->renderer == NULL) {
        printDebug(SDL_GetError(), 5000);
        SDL_DestroyWindow(a->window);
        SDL_free(a);
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    a->font = TTF_OpenFont(GAME_OVER_TTFF, 80);
    if (a->font == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

    a->hint_font = TTF_OpenFont(GAME_OVER_TTFF, 40);
    if (a->hint_font == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

    // Inicializando o controle nativo
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    SDL_Surface *bg_surface = IMG_Load(BACKGROUND_PNG);
    if (!bg_surface) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }
    // aqui importante: converte a superficie numa texture otimizada pra a GPU e guarda em um buffer
    a->background_texture = SDL_CreateTextureFromSurface(a->renderer, bg_surface);
    SDL_DestroySurface(bg_surface); // libera a superficie da mem. ja que n precisamos mais
    if (!a->background_texture) {
        printDebug(SDL_GetError(), 5000);
        SDL_DestroyRenderer(a->renderer);
        SDL_DestroyWindow(a->window);
        SDL_free(a);
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    a->player = init_player();
    if (a->player == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }
    #define CENTRALIZED false
    int grid_width, grid_height, grid_pos_x, grid_pos_y, padding = 0;
    if (CENTRALIZED) {
        grid_width = WINDOW_WIDTH / 2;
        grid_height = WINDOW_HEIGHT - 15;
        grid_pos_x = (WINDOW_WIDTH - grid_width) / 2;
        grid_pos_y = (WINDOW_HEIGHT - grid_height) / 2;
        padding = 2;
    } else {
        grid_width = WINDOW_WIDTH / 2;
        grid_height = WINDOW_HEIGHT - 15;
        grid_pos_x = 10;
        grid_pos_y = 10;
        padding = 2;
    }
    GridArea* gridArea = newGridArea(grid_pos_x, grid_pos_y, grid_width, grid_height, padding);
    a->grid = newGrid(10, 10, gridArea);
    if (a->grid == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }


    populateGridWithWords(a->grid, words, words_count);

    a->grid->font = TTF_OpenFont(GAME_OVER_TTFF, a->grid->font_size);
    if (a->grid->font == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

    /* LETTERS PRE-RENDERIZATION */
    SDL_Color SDL_BLACK = { 0, 0, 0, 255 };
    for(int i = 0; i < 26; i++) {
        char letter_str[2] = { (char)('A' + i), '\0' };
        SDL_Surface* surface = TTF_RenderText_Solid(a->grid->font, letter_str, strlen(letter_str), SDL_BLACK);
        if (surface) {
            a->grid->letter_textures_cache[i] = SDL_CreateTextureFromSurface(a->renderer, surface);
            SDL_DestroySurface(surface);
        } else {
            a->grid->letter_textures_cache[i] = NULL;
        }
    }

	*appstate = (void *)a;
	return SDL_APP_CONTINUE;
}

/*
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *  GAME LOOP
 * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    app_t *a = (app_t *)appstate;
    a->prev_pad = a->pad;

    // --- LOGICA DE CONTROLES
    readButtonState(&a->pad, 1);
    if (a->pad.Buttons & PSP_CTRL_START)  return SDL_APP_SUCCESS;    // Se o botão START for pressionado, fecha o aplicativo

    // --- ATUALIZAÇÃO DE ESTADO ---
    updateCurrentHint(a);
    // ----------------------------------------------------
    // --- LÓGICA DE RENDERIZAÇÃO ---

    SDL_SetRenderDrawColor(a->renderer, 0, 0, 0, 255);
    SDL_RenderClear(a->renderer);

    SDL_RenderTexture(a->renderer, a->background_texture, NULL, NULL);

    // Draw Text
    // drawTextWithFont("Hello PSP!", 5, 5, a->font, a->renderer, SDL_WHITE);
    // Draw Rect
    // drawRect(30, 50, 30, 30, a->renderer, 255, 255, 255, 255, "filled");
    // Draw Grid
    drawGrid(a->grid, a->renderer);
    // Draw debug
    //drawTextWithFont("font: ", 0, 0, a->font, a->renderer, (SDL_Color){ 0, 0, 0, 255});

    // Draw Hint
    float x = (WINDOW_WIDTH / 2) + 20;
    float y = 10;
    float rectW = (WINDOW_WIDTH / 2) - 50;
    float rectH = WINDOW_HEIGHT - 15;
    if (a->current_hint) drawHint(a->current_hint, x, y, rectW, rectH, a->hint_font, a->renderer);

    // cross -> select letter
    if ((a->pad.Buttons & PSP_CTRL_CROSS) && !(a->prev_pad.Buttons & PSP_CTRL_CROSS)) moveCellLetterSelection(a->grid);
    // cima
    if ((a->pad.Buttons & PSP_CTRL_UP) && !(a->prev_pad.Buttons & PSP_CTRL_UP)) moveGridSelection(a->grid, -1, 0);
    // baixo
    if ((a->pad.Buttons & PSP_CTRL_DOWN) && !(a->prev_pad.Buttons & PSP_CTRL_DOWN)) moveGridSelection(a->grid, 1, 0);
    // direita
    if ((a->pad.Buttons & PSP_CTRL_RIGHT) && !(a->prev_pad.Buttons & PSP_CTRL_RIGHT)) moveGridSelection(a->grid, 0, 1);
    // esquerda
    if ((a->pad.Buttons & PSP_CTRL_LEFT) && !(a->prev_pad.Buttons & PSP_CTRL_LEFT)) moveGridSelection(a->grid, 0, -1);

    SDL_RenderPresent(a->renderer); // Mostra na tela tudo o que foi desenhado    
    return SDL_APP_CONTINUE;
}

/*
::::::::::::::::::::::::::::::::::::::
LÓGICA DE INTERAÇÃO COM EVENTOS SDL
::::::::::::::::::::::::::::::::::::::
*/
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	app_t *a = (app_t *)appstate;
    // free letter texture cache
    for(int i = 0; i < 26; i++) {
        if (a->grid->letter_textures_cache[i] != NULL) {
            SDL_DestroyTexture(a->grid->letter_textures_cache[i]);
        }
    }

    TTF_CloseFont(a->font);
    TTF_CloseFont(a->hint_font);

	SDL_DestroyWindow(a->window);
    cleanup_native_audio();
	SDL_free(a);
	SDL_Quit();
}
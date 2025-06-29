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
    SceCtrlData prev_pad;
    Grid* grid;
} app_t;

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

    int grid_width = WINDOW_WIDTH / 2;
    int grid_height = WINDOW_HEIGHT - 15;
    int grid_pos_x = (WINDOW_WIDTH - grid_width) / 2;
    int grid_pos_y = (WINDOW_HEIGHT - grid_height) / 2;
    int padding = 2;
    GridArea* gridArea = newGridArea(grid_pos_x, grid_pos_y, grid_width, grid_height, padding);
    a->grid = newGrid(10, 10, gridArea);
    if (a->grid == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }
    //char letter = 'A';
    for(int i = 0; i < a->grid->nrow * a->grid->ncol; i++) {
        if (a->grid->list_cells[i] != NULL) {
            a->grid->list_cells[i]->current_letter = 'A';
            // if (letter >= 'A' && letter <= 'Z') {
            //     a->grid->list_cells[i]->current_letter = letter++;
            // } else {
            //     letter = 'A';
            // }
        }
    }

    a->grid->font_size = 60;
    a->grid->font = TTF_OpenFont(GAME_OVER_TTFF, a->grid->font_size);
    if (a->grid->font == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
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
    
    
    if ((a->pad.Buttons & PSP_CTRL_CROSS) && !(a->prev_pad.Buttons & PSP_CTRL_CROSS)) {
        trigger_native_sound();
    }

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
	SDL_DestroyWindow(a->window);
    cleanup_native_audio();
	SDL_free(a);
	SDL_Quit();
}
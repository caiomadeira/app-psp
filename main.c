#include "include/common.h"
#include "include/player.h"
#include "include/audio.h"
#include "include/graphic.h"
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

// COLORS SDL
extern SDL_Color SDL_WHITE = { 255, 255, 255, 255 };

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

    a->font = TTF_OpenFont(ROAST_CHICKEN_TTF, 30);
    if (a->font == NULL) {
        printDebug(SDL_GetError(), 5000);
        return SDL_APP_FAILURE;
    }

	// ativa o v-sync
	//SDL_GL_SetSwapInterval(1);

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
    float analog_x = (a->pad.Lx - 128) / 128.0f;    // Normaliza o valor do analógico (0-255 -> -1.0 a 1.0)
    float analog_y = (a->pad.Ly - 128) / 128.0f;
    if (fabs(analog_x) < 0.2f) analog_x = 0.0f;    // "Zona morta" para evitar movimento com o analógico parado
    if (fabs(analog_y) < 0.2f) analog_y = 0.0f;
    a->player->x += analog_x * a->player->speed;    // Atualiza a posição do jogador
    a->player->y -= analog_y * a->player->speed;
    if (a->pad.Buttons & PSP_CTRL_START)  return SDL_APP_SUCCESS;    // Se o botão START for pressionado, fecha o aplicativo
    
    
    if ((a->pad.Buttons & PSP_CTRL_CROSS) && !(a->prev_pad.Buttons & PSP_CTRL_CROSS)) {
        trigger_native_sound();
    }

    // ----------------------------------------------------
    // --- LÓGICA DE RENDERIZAÇÃO ---

    //SDL_Renderer *renderer = SDL_GetRenderer(a->window);
    SDL_SetRenderDrawColor(a->renderer, 0,0,0, 255);
    SDL_RenderClear(a->renderer);

    SDL_RenderTexture(a->renderer, a->background_texture, NULL, NULL);
    
    // Drawing Text
    drawTextWithFont("Hello PSP!", 50, 50, a->font, a->renderer, SDL_WHITE);
    // ----------------------
    SDL_FRect player_rect = { a->player->x, a->player->y, 20, 20 };

    // Centraliza o retângulo na posição do jogador
    player_rect.x = a->player->x * 480 + (480 - player_rect.w) / 2;
    player_rect.y = a->player->y * 272 + (272 - player_rect.h) / 2;

    // Desenha o retângulo do jogador
    SDL_SetRenderDrawColor(a->renderer, 120, 255, 255, 255); // Branco
    SDL_RenderFillRect(a->renderer, &player_rect);
    
    // Mostra na tela tudo o que foi desenhado
    SDL_RenderPresent(a->renderer);
    //SDL_GL_SwapWindow(a->window);
    
    return SDL_APP_CONTINUE;
}

/*
::::::::::::::::::::::::::::::::::::::
LÓGICA DE INTERAÇÃO COM EVENTOS SDL
::::::::::::::::::::::::::::::::::::::
*/
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    // A única coisa com que nos importamos aqui é se a SDL quer fechar o aplicativo
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }


    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	app_t *a = (app_t *)appstate;
	SDL_DestroyWindow(a->window);
	//SDL_GL_DestroyContext(a->context);
    //cleanup_audio();
    cleanup_native_audio();
	SDL_free(a);
	SDL_Quit();
}
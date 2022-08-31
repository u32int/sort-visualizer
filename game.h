#pragma once

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 800

#define FONT_SIZE 32

#define FPS_CAP 60
#define FPS_DELTA (1000/FPS_CAP)

typedef struct game_t game_t;

typedef enum {
  Run,
  Pause,
  Reset,
  Quit,
} State;

typedef enum {
    Bubble,
    Quick,
    Insertion,
    Merge,

    Algorithm_END
} Algorithm;

typedef struct {
    int x1, y1;
    int x2, y2;

    void (*callback)(game_t *game);
    void (*draw_func)(SDL_Renderer *renderer, game_t *game,
                      int x1, int y1, int x2, int y2);
} button_t;

typedef struct {
    Algorithm algorithm;
    size_t rect_width;
    struct timespec ts;
} game_settings_t;

typedef struct {
    size_t accesses;
    size_t writes;
    size_t swaps;
} game_stats_t;

typedef struct game_t {
    State state;
    game_settings_t settings;
    game_stats_t stats;
    button_t buttons[16];

    int *array;
    int *aux_array;
    int *aux_array_b;
    int hl_elem;
} game_t;


void init_game(game_t *game);
void randomize_array(game_t *game);

void pauseplay_game(game_t *game);
void restart_game(game_t *game);
void handle_key(game_t *game, SDL_Keycode key);
void cycle_algo_left(game_t *game);
void cycle_algo_right(game_t *game);
void elem_size_increase(game_t *game);
void elem_size_decrease(game_t *game);
void delay_decrease(game_t *game);
void delay_increase(game_t *game);

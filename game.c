#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <stdio.h>

#include <pthread.h>
#include <unistd.h>

#include "game.h"
#include "util.h" 

#define ARRAY_SIZE (SCREEN_WIDTH/game->settings.rect_width)

pthread_mutex_t stat_lock = PTHREAD_MUTEX_INITIALIZER;

void randomize_array(game_t *game)
{
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        game->array[i] = rand() % SCREEN_HEIGHT/2;
    }
}

void restart_game(game_t *game)
{
    game->state = Reset;

    while (game->state != Pause) /* wait for sorting function to exit */
        usleep(100);

    game->stats.writes = 0;
    game->stats.accesses = 0;
    game->stats.swaps = 0;
}

void pauseplay_game(game_t *game)
{
    if (game->state == Run)
        game->state = Pause;
    else if (game->state == Pause)
        game->state = Run;
}

void cycle_algo_right(game_t *game)
{
    if (game->settings.algorithm != Algorithm_END-1)
        restart_game(game);
    game->settings.algorithm = clamp_int(game->settings.algorithm+1,
                                         0, Algorithm_END-1);
}

void cycle_algo_left(game_t *game)
{
    if (game->settings.algorithm != 0)
        restart_game(game);
    game->settings.algorithm = clamp_int(game->settings.algorithm-1,
                                         0, Algorithm_END-1);
}

void elem_size_increase(game_t *game)
{
    game->settings.rect_width = clamp_int(game->settings.rect_width-1,
                                        1, 12);
    if (game->state == Pause) {
        if (game->hl_elem != -1)
            restart_game(game);
    } else {
        restart_game(game);
    }
}

void elem_size_decrease(game_t *game)
{
    game->settings.rect_width = clamp_int(game->settings.rect_width+1,
                                            1, 12);
    if (game->state == Pause) {
        if (game->hl_elem != -1)
            restart_game(game);
    } else {
        restart_game(game);
    }
}

void init_game(game_t *game)
{
    game->state = Pause;
    game->hl_elem = -1;

    game->settings.rect_width = 1;
    game->settings.algorithm  = Bubble;
    game->settings.delay_us = 1000;

    game->stats.writes = 0;
    game->stats.accesses = 0;
    game->stats.swaps = 0;

    game->array = (int*)malloc(SCREEN_WIDTH*sizeof(int));
    randomize_array(game);
}


void handle_key(game_t *game, SDL_Keycode key)
{
    switch (key) {
    case SDLK_p:
    case SDLK_SPACE:
        pauseplay_game(game);
        break;
    case SDLK_r:
        restart_game(game);
        break;
    case SDLK_LEFT:
        cycle_algo_left(game);
        break;
    case SDLK_RIGHT:
        cycle_algo_right(game);
        break;
    case SDLK_UP:
        elem_size_increase(game);
        break;
    case SDLK_DOWN:
        elem_size_decrease(game);
        break;
    default: {};
    }
}

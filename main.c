#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>

#include "game.h"
#include "render.h"
#include "algorithm.h"
#include "util.h"


int main()
{
    srand(time(NULL));
    
    game_t game = {
        .buttons = {
            /* Pause button */
            {.x1 = 10, .y1 = 5, .x2 = 55, .y2 = 60,
             .callback = pauseplay_game,
             .draw_func = render_pauseplay_button
            },
            /* Restart button */
            {.x1 = 4, .y1 = 70, .x2 = 55, .y2 = 120,
             .callback = restart_game,
             .draw_func = render_restart_button
            },
            /* Cycle algorithms left-right buttons */
            {.x1 = SCREEN_WIDTH/2-30-80, .y1 = 15, .x2 = SCREEN_WIDTH/2-80, .y2 = 45,
             .callback = cycle_algo_left,
             .draw_func = render_algoleft,
            },
            {.x1 = SCREEN_WIDTH/2+80, .y1 = 15, .x2 = SCREEN_WIDTH/2+30+80, .y2 = 45,
             .callback = cycle_algo_right,
             .draw_func = render_algoright,
            },
            /* Elements */
            {.x1 = SCREEN_WIDTH/12-25, .y1 = FONT_SIZE/2+20+2,
             .x2 = SCREEN_WIDTH/12-5, .y2 = FONT_SIZE/2+40+2,
             .callback = elem_size_decrease,
             .draw_func = render_elemnum_down,
            },
            {.x1 = SCREEN_WIDTH/12-25, .y1 = FONT_SIZE/2-2,
             .x2 = SCREEN_WIDTH/12-5, .y2 = FONT_SIZE/2+20-2,
             .callback = elem_size_increase,
             .draw_func = render_elemnum_up,
            },
            /* Delay */
            {.x1 = SCREEN_WIDTH/12-25, .y1 = FONT_SIZE*2+20+2,
             .x2 = SCREEN_WIDTH/12-5, .y2 = FONT_SIZE*2+40+2,
             .callback = delay_decrease,
             .draw_func = render_delay_down,
            },
            {.x1 = SCREEN_WIDTH/12-25, .y1 = FONT_SIZE*2-2,
             .x2 = SCREEN_WIDTH/12-5, .y2 = FONT_SIZE*2+20-2,
             .callback = delay_increase,
             .draw_func = render_delay_up,
            },
            {.callback = NULL}, /* END */
        }
    };

    init_game(&game);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "sdl2 initialzation error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("VisualSort",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Couldn't create SDL_Window: %s\n", SDL_GetError()); 
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        fprintf(stderr, "Couldn't create SDL_Renderer: %s\n", SDL_GetError()); 
        return EXIT_FAILURE;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "sdl_ttf initialzation error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    TTF_Font *font = TTF_OpenFont("../res/LiberationSans-Regular.ttf", FONT_SIZE);
    if (font == NULL) {
        fprintf(stderr, "Font error: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    pthread_t sort_thread;
    pthread_create(&sort_thread, NULL, sort, &game);

    Uint64 frame_start;
    SDL_Event ev;
    while (game.state != Quit) {
        frame_start = SDL_GetTicks64();
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_QUIT:
                game.state = Quit;
                pthread_cancel(sort_thread);
                break;
            case SDL_KEYDOWN:
                handle_key(&game, ev.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN: {
                int x, y;
                for (int i = 0; game.buttons[i].callback != NULL; ++i) {
                    x = ev.button.x;
                    y = ev.button.y;
                    if (x > game.buttons[i].x1 && y > game.buttons[i].y1) {
                        if (x < game.buttons[i].x2 && y < game.buttons[i].y2) {
                            (*game.buttons[i].callback)(&game);
                        }
                    }
                }
                break;
            }
            default: {}
            }
        }

        SDL_SetRenderDrawColor(renderer, 18,18,18,255);
        SDL_RenderClear(renderer);

        render_game(renderer, &game, font);
        for (int i = 0; game.buttons[i].callback != NULL; ++i)
            game.buttons[i].draw_func(renderer, &game,
                                      game.buttons[i].x1, game.buttons[i].y1,
                                      game.buttons[i].x2, game.buttons[i].y2);

        SDL_RenderPresent(renderer);

        Uint64 frame_time = SDL_GetTicks64() - frame_start;
        if (frame_time < FPS_DELTA) {
            SDL_Delay(FPS_DELTA-frame_time);
        }

    }

    free(game.array);
    free(game.aux_array);
    free(game.aux_array_b);

    TTF_CloseFont(font);

    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

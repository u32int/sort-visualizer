#pragma once

#include "game.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

void render_game(SDL_Renderer *renderer, game_t *game, TTF_Font *font);

void render_pauseplay_button(SDL_Renderer *renderer, game_t *game,
                             int x1, int y1, int x2, int y2);
void render_restart_button(SDL_Renderer *renderer, game_t *game,
                           int x1, int y1, int x2, int y2);

void render_algoleft(SDL_Renderer *renderer, game_t *game,
                     int x1, int y1, int x2, int y2);

void render_algoright(SDL_Renderer *renderer, game_t *game,
                      int x1, int y1, int x2, int y2);

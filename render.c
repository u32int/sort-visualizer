#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "render.h"

static const SDL_Color col_normal = { .r = 255, .b = 255, .g = 255, .a = 255};
static const SDL_Color col_dim = { .r = 90, .b = 90, .g = 90, .a = 255};
static const SDL_Color col_highlight = { .r = 230, .b = 15, .g = 15, .a = 255};

static const char* algo_pretty_names[] = {"Bubble", "Quick", "Insertion"};

#define ARRAY_SIZE (SCREEN_WIDTH/game->settings.rect_width)

struct TextFlags {
    bool center;
} text_flags = { .center = true };

void render_text(SDL_Renderer *renderer, int x, int y,
                 TTF_Font *font, SDL_Color color,
                 const char* text)
{
    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, text, color);
    
    if(!text_surface) {
        fprintf(stderr, "sdl_ttf error: %s", TTF_GetError());
        exit(1);
    }

    int txt_w = text_surface->w;
    int txt_h = text_surface->h;

    if (text_flags.center) {
        x += SCREEN_WIDTH/2-txt_w/2;
        y += SCREEN_HEIGHT/2-txt_h/2;
    }

    SDL_Rect text_rect = { .h = txt_h, .w = txt_w,
                           .x = x, .y = y};

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}


void render_game(SDL_Renderer *renderer, game_t *game, TTF_Font *font)
{
    /* render array contents */
    for (int i = 0; i < SCREEN_WIDTH/game->settings.rect_width; ++i) {
        if (i == game->hl_elem)
            
            boxRGBA(renderer,
                    i*game->settings.rect_width, SCREEN_HEIGHT,
                    (i+1)*game->settings.rect_width, SCREEN_HEIGHT-game->array[i],
                    col_highlight.r, col_highlight.g, col_highlight.b, col_highlight.a);
        else
            boxRGBA(renderer,
                    i*game->settings.rect_width, SCREEN_HEIGHT,
                    (i+1)*game->settings.rect_width, SCREEN_HEIGHT-game->array[i],
                    col_normal.r, col_normal.g, col_normal.b, col_normal.a);
    }

    /* selected algorithm */
    render_text(renderer, 0, -SCREEN_HEIGHT/2+FONT_SIZE,
                font, col_normal,
                algo_pretty_names[game->settings.algorithm]);

    char num_elems[32];
    snprintf(num_elems, 32, "Elem: %lu (%lu)", ARRAY_SIZE, game->settings.rect_width);
    /* elem num */
    render_text(renderer,
                -SCREEN_WIDTH/3, -SCREEN_HEIGHT/2+FONT_SIZE,
                font, col_normal,
                num_elems);

    /* stats */
    char stats[3][32];
    snprintf(stats[0], 32, "accesses: %lu", game->stats.accesses);
    snprintf(stats[1], 32, "writes: %lu", game->stats.writes);
    snprintf(stats[2], 32, "swaps: %lu", game->stats.swaps);

    text_flags.center = false;
    for (int i = 0; i < 3; ++i) {
        render_text(renderer,
                    SCREEN_WIDTH/2+SCREEN_WIDTH/4, i*FONT_SIZE+10,
                    font, col_normal,
                    stats[i]);
    }
    text_flags.center = true;
}


void render_pauseplay_button(SDL_Renderer *renderer, game_t *game,
                             int x1, int y1, int x2, int y2)
{
    if (game->state == Pause) {
        filledTrigonRGBA(renderer,
                         x1, y1,
                         x1, y2,
                         x2, y2/2,
                         col_normal.r, col_normal.g, col_normal.b, col_normal.a);
    } else {
        boxRGBA(renderer,
                x1, y1, x1+(x2-x1)/3, y2,
                col_normal.r, col_normal.g, col_normal.b, col_normal.a);
        boxRGBA(renderer,
                x2-(x2-x1)/3, y1, x2, y2,
                col_normal.r, col_normal.g, col_normal.b, col_normal.a);
    }
}

void render_restart_button(SDL_Renderer *renderer, game_t *game,
                             int x1, int y1, int x2, int y2)
{
    (void) game;

    arcRGBA(renderer,
            (x1+x2)/2, (y1+y2)/2,
            (y2-y1)/3+(y2-y1)/10, -90, 150,
            col_normal.r, col_normal.g, col_normal.b, col_normal.a);

    filledTrigonRGBA(renderer,
                    (x1+x2)/2, y1-3,
                    (x1+x2)/2, y1+(y2-y1)/4,
                    (x1+x2)/2-10, y1+(y2-y1)/8,
                    col_normal.r, col_normal.g, col_normal.b, col_normal.a);
}


void render_algoleft(SDL_Renderer *renderer, game_t *game,
                          int x1, int y1, int x2, int y2)
{

    if (game->settings.algorithm == 0) {
        filledTrigonRGBA(renderer,
                        x1, (y1+y2)/2,
                        x2, y1,
                        x2, y2,
                        col_dim.r, col_dim.g, col_dim.b, col_dim.a);
    } else {
        filledTrigonRGBA(renderer,
                        x1, (y1+y2)/2,
                        x2, y1,
                        x2, y2,
                        col_normal.r, col_normal.g, col_normal.b, col_normal.a);
    }
}

void render_algoright(SDL_Renderer *renderer, game_t *game,
                           int x1, int y1, int x2, int y2)
{
    if (game->settings.algorithm == Algorithm_END-1) {
        filledTrigonRGBA(renderer,
                        x2, (y1+y2)/2,
                        x1, y1,
                        x1, y2,
                        col_dim.r, col_dim.g, col_dim.b, col_dim.a);
    } else {
        filledTrigonRGBA(renderer,
                        x2, (y1+y2)/2,
                        x1, y1,
                        x1, y2,
                        col_normal.r, col_normal.g, col_normal.b, col_normal.a);
    }
}

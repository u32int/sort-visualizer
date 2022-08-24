#include "algorithm.h"
#include "game.h"
#include "util.h"

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define ARRAY_SIZE (SCREEN_WIDTH / game->settings.rect_width)

int check_return_pause(game_t *game)
{
    while(game->state == Pause)
        usleep(100);

    if (game->state == Reset)
        return 1;
    return 0;
}

int array_get(game_t *game, size_t index)
{
    game->stats.accesses++;
    return game->array[index];
}

void array_set(game_t *game, size_t index, int value)
{
    game->array[index] = value;
    game->stats.writes++;
}

void array_swap(game_t *game, size_t index_a, size_t index_b)
{
    int tmp = array_get(game, index_a);
    array_set(game, index_a, array_get(game, index_b));
    array_set(game, index_b, tmp);
    game->stats.swaps++;
}

void bubble_sort(game_t *game)
{
    bool sorted = false;
    while (!sorted) {
        sorted = true;
        for (int i = 0; i < ARRAY_SIZE-1; ++i) {
            if(check_return_pause(game))
                return;

            if (array_get(game, i) > array_get(game, i+1)) {
                array_swap(game, i, i+1);
                sorted = false;
            }
            game->hl_elem = i+2;

            usleep(game->settings.delay_us);
        }
    }
}

void quick_sort(game_t *game, int start, int end)
{
    if (start >= end)
        return;

    int pivot = start;
    int left = start-1;
    int right = end+1;

    while (1) {
        if(check_return_pause(game))
            return;
        
        do {
            left++;
            game->hl_elem = left;
            usleep(game->settings.delay_us);
        } while (array_get(game, left) < array_get(game, pivot));

        do {
            right--; 
            game->hl_elem = right;
            usleep(game->settings.delay_us);
        } while (array_get(game, right) > array_get(game, pivot));

        if (left >= right)
            break;

        array_swap(game, left, right);
    }

    quick_sort(game, start, right);
    quick_sort(game, right+1, end);
}

void insertion_sort(game_t *game)
{
    for (size_t i = 0; i < ARRAY_SIZE-1; ++i) {
        if(check_return_pause(game))
            return;

        game->hl_elem = i;
        if (array_get(game, i) > array_get(game, i+1)) {
            int tmp = array_get(game, i+1);
            size_t j = i;
            while (array_get(game, j) > tmp && j >= 0) {
                game->hl_elem = j;
                usleep(game->settings.delay_us);
                array_swap(game, j, j+1);
                j--;
            }
        }
    }
}

void *sort(void *arg)
{
    game_t *game = (game_t*)arg;

    bool randomize = true;

    while (game->state != Quit) {
        while (game->state == Pause)
            usleep(1000);

        if (randomize)
            randomize_array(game);
        else
            randomize = true;

        switch (game->settings.algorithm) {
        case Algorithm_END: {}
        case Bubble:
            bubble_sort(game);
            break;
        case Quick:
            quick_sort(game, 0, ARRAY_SIZE-1);
            break;
        case Insertion:
            insertion_sort(game);
            break;
        }

        game->hl_elem = -1;

        if (game->state == Reset) {
            randomize_array(game);
            randomize = false;
        }

        game->state = Pause;
    }

    pthread_exit(NULL);
}

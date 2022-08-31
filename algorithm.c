#include <string.h>
#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "algorithm.h"
#include "game.h"
#include "util.h"

#define ARRAY_SIZE (SCREEN_WIDTH / game->settings.rect_width)

int check_return_pause(game_t *game)
{
    while(game->state == Pause)
        sleep_ms(10);

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
    int j = 1;
    while (!sorted) {
        sorted = true;
        for (size_t i = 0; i < ARRAY_SIZE-j; ++i) {
            if(check_return_pause(game))
                return;

            if (array_get(game, i) > array_get(game, i+1)) {
                array_swap(game, i, i+1);
                sorted = false;
            }
            game->hl_elem = i+2;

            nanosleep(&game->settings.ts, NULL);
        }
        j++;
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
            nanosleep(&game->settings.ts, NULL);
        } while (array_get(game, left) < array_get(game, pivot));

        do {
            right--; 
            game->hl_elem = right;
            nanosleep(&game->settings.ts, NULL);
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
            int j = i;
            while (array_get(game, j) > tmp && j >= 0) {
                game->hl_elem = j;
                nanosleep(&game->settings.ts, NULL);
                array_swap(game, j, j+1);
                j--;
            }
        }
    }
}


void merge_sort(int array[], int aux_array[], size_t begin, size_t end, game_t *game)
{
    if (end - begin <= 1 || check_return_pause(game))
        return;

    size_t mid = (begin + end) / 2;

    merge_sort(aux_array, array, begin, mid, game);
    merge_sort(aux_array, array, mid,   end, game);

    size_t left = begin;
    size_t right = mid;

    for (size_t i = begin; i < end; ++i) {
        game->hl_elem = i;
        nanosleep(&game->settings.ts, NULL);
        if (left < mid && (right >= end || aux_array[left] < aux_array[right])) {
            game->array[i] = aux_array[left];  // Visualization purpose only
            array[i] = aux_array[left];
            left++;
        } else {
            game->array[i] = aux_array[right]; // Visualization purpose only
            array[i] = aux_array[right];
            right++;
        }
    }
}

void *sort(void *arg)
{
    game_t *game = (game_t*)arg;

    bool randomize = true;

    while (game->state != Quit) {
        while (game->state == Pause)
            sleep_ms(1);

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
        case Merge:
            memcpy(game->aux_array, game->array, ARRAY_SIZE*sizeof(int));
            memcpy(game->aux_array_b, game->array, ARRAY_SIZE*sizeof(int));
            merge_sort(game->aux_array, game->aux_array_b, 0, ARRAY_SIZE, game);
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

#pragma once

#include <stdint.h>
#include <stdio.h>
#include "Snake.h"
#include "Map.h"
#include "Item.h"
#include "Vector.h"


typedef struct World_t {
    Snake snake;
    Map map;
    Vector items;
} World;

void world_free(World *world) {
    snake_free(&world->snake);
    map_free(&world->map);
    vec_free(&world->items);
}

inline static World world_generate() {
    Map map = map_init(20, 30);

    // Draw walls
    for (size_t y = 0; y < map.height; y++) {
        for (size_t x = 0; x < map.width; x++) {
            const size_t i = y * map.width + x;
            if (x == 0 || y == 0 || x == map.width - 1 || y == map.height - 1) {
                map.tiles[i] = MAP_TILE_WALL;
            }
        }
    }

    Direction random_direction = rand() % 4 + NORTH;
    size_t mid_point = map.height / 2 * map.width + map.width / 2;
    Snake snake = snake_init(mid_point, random_direction);

    World world = {
            .snake = snake,
            .items = vec_init(sizeof(Item)),
            .map = map,
    };

    Item one_fourth = {.position = map.height / 4 * map.width + map.width / 4};
    vec_push_back(&world.items, (uint8_t *) &one_fourth);

    return world;
}

void world_generate_random_item(World *world) {
    Vector full_space = vec_init(sizeof(size_t));
    Vector free_space = vec_init(sizeof(size_t));

    for (size_t y = 0; y < world->map.height; y++) {
        for (size_t x = 0; x < world->map.width; x++) {
            size_t i = y * world->map.width + x;
            if (world->map.tiles[i] == MAP_TILE_WALL) {
                vec_push_back(&full_space, (uint8_t *) &i);
            }
        }
    }

    for (size_t i = 0; i < world->snake.body.size; i++) {
        size_t position = ((size_t *) world->snake.body.data)[i];
        vec_push_back(&full_space, (uint8_t *) &position);
    }

    for (size_t y = 0; y < world->map.height; y++) {
        for (size_t x = 0; x < world->map.width; x++) {
            size_t position = y * world->map.width + x;
            bool full = false;
            for (size_t i = 0; i < full_space.size; i++) {
                if (position == ((size_t *) full_space.data)[i]) {
                    full = true;
                    break;
                }
            }
            if (!full) {
                vec_push_back(&free_space, (uint8_t *) &position);
            }
        }
    }

    size_t rand_position = ((size_t *) free_space.data)[rand() % free_space.size];

    vec_free(&full_space);
    vec_free(&free_space);

    vec_push_back(&world->items, (uint8_t *) &rand_position);
}


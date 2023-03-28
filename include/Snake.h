#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "Map.h"
#include "Vector.h"

typedef enum Direction_t {
    NORTH,
    EAST,
    SOUTH,
    WEST,
} Direction;

typedef struct Snake_t {
    Direction movement_direction;
    Vector body;
} Snake;

Snake snake_init(size_t head, Direction movement_direction) {
    Vector body = vec_init(sizeof(size_t));
    vec_push_back(&body, (char *) &head);

    Snake snake = {
            .movement_direction = movement_direction,
            .body = body,
    };
    return snake;
};

void snake_free(Snake *snake) {
    vec_free(&snake->body);
}

bool snake_can_move(const Snake snake, const Map map) {
    size_t y = *(size_t *) snake.body.data / map.width;
    size_t x = *(size_t *) snake.body.data % map.width;
    if ((snake.movement_direction == NORTH && y == 0)
        || (snake.movement_direction == EAST && x == map.width)
        || (snake.movement_direction == SOUTH && y == map.height)
        || (snake.movement_direction == WEST && x == 0)) {
        return false;
    }
    if (snake.movement_direction == NORTH) y -= 1;
    if (snake.movement_direction == EAST) x += 1;
    if (snake.movement_direction == SOUTH) y += 1;
    if (snake.movement_direction == WEST) x -= 1;

    for (size_t ym = 0; ym < map.height; ym++) {
        for (size_t xm = 0; xm < map.width; xm++) {
            if (map.tiles[y * map.width + x] == MAP_TILE_WALL) {
                return false;
            }
        }
    }

    for (size_t i = 0; i < snake.body.size; i++) {
        if (y * map.width + x == ((size_t *) snake.body.data)[i]) {
            return false;
        }
    }

    return true;
}

void snake_grow_move(Snake *snake, const Map map) {
    if (snake_can_move(*snake, map)) {
        size_t y = *(size_t *) snake->body.data / map.width;
        size_t x = *(size_t *) snake->body.data % map.width;
        if (snake->movement_direction == NORTH) y -= 1;
        if (snake->movement_direction == EAST) x += 1;
        if (snake->movement_direction == SOUTH) y += 1;
        if (snake->movement_direction == WEST) x -= 1;
        size_t head = y * map.width + x;
        vec_push_front(&snake->body, (uint8_t *) &head);
    }
}

void snake_move(Snake *snake, const Map map) {
    if (snake_can_move(*snake, map)) {
        snake_grow_move(snake, map);
        vec_pop_back(&snake->body);
    }
}

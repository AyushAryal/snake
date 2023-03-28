#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef enum MapTileType_t {
    MAP_TILE_EMPTY,
    MAP_TILE_WALL,
} MapTileType;

typedef struct Map_t {
    size_t height, width;
    MapTileType *tiles;
} Map;

Map map_init(size_t height, size_t width) {
    Map map = {
            .height = height,
            .width = width,
            .tiles = malloc(height * width * sizeof(*map.tiles))
    };
    memset(map.tiles, MAP_TILE_EMPTY, map.width * map.height);
    return map;
}

void map_free(Map* map) {
    free(map->tiles);
    map->tiles = NULL, map->height = 0, map->width = 0;
}

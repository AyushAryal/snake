#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include "World.h"
#include "Window.h"
#include "Callbacks.h"
#include "Renderer.h"


void world_tick(World *world, double dt) {
    if (!snake_can_move(world->snake, world->map)) {
        world_free(world);
        *world = world_generate();
    }
    for (size_t i = 0; i < world->items.size; i++) {
        Item item = ((Item *) world->items.data)[i];
        if (item.position == *(size_t *) world->snake.body.data) {
            snake_grow_move(&world->snake, world->map);
            vec_delete(&world->items, i);
            world_generate_random_item(world);
            return;
        }
    }
    snake_move(&world->snake, world->map);
}

void handle_events(GLFWwindow *window, World *world) {
    if (glfwGetKey(window, GLFW_KEY_UP)) {
        if (world->snake.body.size == 1 || world->snake.movement_direction != NORTH) {
            world->snake.movement_direction = SOUTH;
        }
    } else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        if (world->snake.body.size == 1 || world->snake.movement_direction != SOUTH) {
            world->snake.movement_direction = NORTH;
        }
    } else if (glfwGetKey(window, GLFW_KEY_LEFT)) {
        if (world->snake.body.size == 1 || world->snake.movement_direction != EAST) {
            world->snake.movement_direction = WEST;
        }
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
        if (world->snake.body.size == 1 || world->snake.movement_direction != WEST) {
            world->snake.movement_direction = EAST;
        }
    }
}

void gameloop(GLFWwindow *window) {
    World world = world_generate();
    Renderer renderer = renderer_init();
    double start_time = glfwGetTime();
    double movement_speed = 0.2;
    const float fps = 30.0f;
    while (running) {
        double frame_start_time = glfwGetTime();
        double dt = frame_start_time - start_time;
        glfwSwapBuffers(window), glfwPollEvents();
        renderer_clear(renderer);
        renderer_draw(renderer, world);
        handle_events(window, &world);
        if (dt > movement_speed) {
            world_tick(&world, dt);
            start_time = glfwGetTime();
        }

        double frame_time = (glfwGetTime() - frame_start_time);
        if (frame_time < 1 / fps) {
            Sleep((DWORD) ((1.0f / fps - frame_time) * 1000));
        }
    }
    world_free(&world);
    renderer_free(&renderer);
}

int main() {
    srand((uint32_t) time(NULL));
    WindowConfig config = {
            .width = 1280,
            .height = 800,
            .title = "Snake",
            .on_close = on_close,
            .on_window_resize = NULL,
            .on_mouse_click = NULL,
    };
    GLFWwindow *window = window_init(config);

    gameloop(window);
    window_free(window);
    return 0;
}


#ifdef _WIN32
int APIENTRY wWinMain(_In_ HINSTANCE hinstance,
                      _In_opt_ HINSTANCE opt,
                      _In_ LPWSTR cmd,
                      _In_ int i) {
    return main();
}
#endif

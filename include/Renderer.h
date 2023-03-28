#pragma once

#include "World.h"

typedef struct Renderer_t {
    uint32_t shader;
    uint32_t VBO;
    uint32_t VAO;
    uint32_t EBO;
} Renderer;

float vertices[] = {
        // positions         // colors
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // right top
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // right bottom
        -0.5, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // left bottom
        -0.5, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f    // left top
};
unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
};

const char *vertexShaderSource = "#version 330 core \n"
                                 "layout (location = 0) in vec3 aPos;"
                                 "layout (location = 1) in vec3 aColor;"
                                 "out vec3 ourColor;"
                                 "void main() {"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                                 "   ourColor = aColor;"
                                 "}";
const char *fragmentShaderSource =
        "#version 330 core \n"
        "out vec4 FragColor;"
        "in vec3 ourColor;"
        "void main()"
        "{"
        "    FragColor = vec4(ourColor, 1.0f);"
        "}";

Renderer renderer_init() {
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    uint32_t shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    uint32_t VAO;
    glGenVertexArrays(1, &VAO);

    uint32_t VBO;
    glGenBuffers(1, &VBO);

    uint32_t EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);


    Renderer renderer = {
            .shader = shaderProgram,
            .VBO = VBO,
            .VAO = VAO,
            .EBO = EBO,
    };
    return renderer;
}

void renderer_free(Renderer *renderer) {
    glDeleteBuffers(1, &renderer->VBO);
    glDeleteBuffers(1, &renderer->EBO);
    glDeleteVertexArrays(1, &renderer->VBO);
    glDeleteProgram(renderer->shader);
}

void renderer_clear(Renderer renderer) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_draw(Renderer renderer, World world) {
    glUseProgram(renderer.shader);

    const size_t WINDOW_WIDTH = 1280;
    const size_t WINDOW_HEIGHT = 800;

    const size_t SQUARE_SIZE = 30;
    const size_t SQUARE_GAP = 2;

    size_t total_width = world.map.width * SQUARE_SIZE + abs((int) world.map.width - 1) * SQUARE_GAP;
    size_t start_width = (WINDOW_WIDTH - total_width) / 2;

    size_t total_height = world.map.height * SQUARE_SIZE + abs((int) world.map.height - 1) * SQUARE_GAP;
    size_t start_height = (WINDOW_HEIGHT - total_height) / 2;

    for (size_t y = 0; y < world.map.height; y++) {
        for (size_t x = 0; x < world.map.width; x++) {
            float x1 = ((float) (start_width + x * (SQUARE_SIZE + SQUARE_GAP)) / (float) WINDOW_WIDTH) * 2 - 1;
            float y1 = ((float) (start_height + y * (SQUARE_SIZE + SQUARE_GAP)) / (float) WINDOW_HEIGHT) * 2 - 1;

            float x2 =
                    ((float) (start_width + x * (SQUARE_SIZE + SQUARE_GAP) + SQUARE_SIZE) / (float) WINDOW_WIDTH) * 2 -
                    1;
            float y2 = ((float) (start_height + y * (SQUARE_SIZE + SQUARE_GAP) + SQUARE_SIZE) / (float) WINDOW_HEIGHT) *
                       2 - 1;
            vertices[0] = x2;
            vertices[1] = y1; //
            vertices[6] = x2;
            vertices[7] = y2; //
            vertices[12] = x1;
            vertices[13] = y2; //
            vertices[18] = x1;
            vertices[19] = y1; //

            const size_t i = y * world.map.width + x;
            if (world.map.tiles[i] == MAP_TILE_WALL) {
                vertices[3] = vertices[9] = vertices[15] = vertices[21] = 0.5f;
                vertices[4] = vertices[10] = vertices[16] = vertices[22] = 0.5f;
                vertices[5] = vertices[11] = vertices[17] = vertices[23] = 1.0f;
            } else {
                vertices[3] = vertices[9] = vertices[15] = vertices[21] = 1.0f;
                vertices[4] = vertices[10] = vertices[16] = vertices[22] = 1.0f;
                vertices[5] = vertices[11] = vertices[17] = vertices[23] = 1.0f;
            }

            glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glBindVertexArray(renderer.VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

    // Draw snake.
    for (size_t i = 0; i < world.snake.body.size; i++) {
        size_t x = ((size_t *) world.snake.body.data)[i] % world.map.width;
        size_t y = ((size_t *) world.snake.body.data)[i] / world.map.width;
        float x1 = ((float) (start_width + x * (SQUARE_SIZE + SQUARE_GAP)) / (float) WINDOW_WIDTH) * 2 - 1;
        float y1 = ((float) (start_height + y * (SQUARE_SIZE + SQUARE_GAP)) / (float) WINDOW_HEIGHT) * 2 - 1;

        float x2 =
                ((float) (start_width + x * (SQUARE_SIZE + SQUARE_GAP) + SQUARE_SIZE) / (float) WINDOW_WIDTH) * 2 -
                1;
        float y2 = ((float) (start_height + y * (SQUARE_SIZE + SQUARE_GAP) + SQUARE_SIZE) / (float) WINDOW_HEIGHT) *
                   2 - 1;
        vertices[0] = x2;
        vertices[1] = y1; //
        vertices[6] = x2;
        vertices[7] = y2; //
        vertices[12] = x1;
        vertices[13] = y2; //
        vertices[18] = x1;
        vertices[19] = y1; //

        vertices[3] = vertices[9] = vertices[15] = vertices[21] = 0.5f;
        vertices[4] = vertices[10] = vertices[16] = vertices[22] = 1.0f;
        vertices[5] = vertices[11] = vertices[17] = vertices[23] = 0.5f;

        glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(renderer.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // Draw items.
    for (size_t i = 0; i < world.items.size; i++) {
        Item item = ((Item *) world.items.data)[i];
        size_t x = item.position % world.map.width;
        size_t y = item.position / world.map.width;
        float x1 = ((float) (start_width + x * (SQUARE_SIZE + SQUARE_GAP)) / (float) WINDOW_WIDTH) * 2 - 1;
        float y1 = ((float) (start_height + y * (SQUARE_SIZE + SQUARE_GAP)) / (float) WINDOW_HEIGHT) * 2 - 1;

        float x2 =
                ((float) (start_width + x * (SQUARE_SIZE + SQUARE_GAP) + SQUARE_SIZE) / (float) WINDOW_WIDTH) * 2 -
                1;
        float y2 = ((float) (start_height + y * (SQUARE_SIZE + SQUARE_GAP) + SQUARE_SIZE) / (float) WINDOW_HEIGHT) *
                   2 - 1;
        vertices[0] = x2;
        vertices[1] = y1; //
        vertices[6] = x2;
        vertices[7] = y2; //
        vertices[12] = x1;
        vertices[13] = y2; //
        vertices[18] = x1;
        vertices[19] = y1; //

        vertices[3] = vertices[9] = vertices[15] = vertices[21] = (float) sin(glfwGetTime() * 3) * 0.3 + 0.7;
        vertices[4] = vertices[10] = vertices[16] = vertices[22] = 0.5f;
        vertices[5] = vertices[11] = vertices[17] = vertices[23] = 0.5f;

        glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(renderer.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

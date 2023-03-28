#pragma once
#include <stdbool.h>
#include <GLFW/glfw3.h>

bool running = true;

inline static void on_close(GLFWwindow* window) { running = false; }

#include "app.h"

#include "common.h"


void OnFramebufferResize(GLFWwindow* window, i32 width, i32 height) {
    auto app = reinterpret_cast<Tetris3DApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->OnFramebufferResize(width, height);
    }
}

void OnKeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action,
                   i32 mods) {
    auto app = reinterpret_cast<Tetris3DApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->OnKeyChanged(key, scancode, action, mods);
    }

    if (key == Settings::key_quit && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
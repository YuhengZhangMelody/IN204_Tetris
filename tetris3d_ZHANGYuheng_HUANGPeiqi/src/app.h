#pragma once

#include <chrono>
#include <cstdlib>
#include <stdio.h>
#include <thread>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#include "game.h"
#include "input.h"
#include "settings.h"

void OnFramebufferResize(GLFWwindow* window, i32 width, i32 height);
void OnKeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action,
                   i32 mods);

class Timer {
  public:
    void Tick() {
        auto tick = std::chrono::high_resolution_clock::now();
        std::chrono::duration<f32> diff = tick - prev_tick;
        elapsed_seconds = diff.count();
        prev_tick = tick;
    }

    f32 GetElapsedSeconds() const { return elapsed_seconds; }

  private:
    std::chrono::high_resolution_clock::time_point prev_tick =
        std::chrono::high_resolution_clock::now();
    f32 elapsed_seconds = 0.f;
};

class Tetris3DApp {
  public:
    void Run() {
        using namespace std::literals::chrono_literals;

        if (!StartUp()) {
            return;
        }
        if (!game.StartUp()) {
            return;
        }

        {
            i32 framebuffer_width, framebuffer_height;
            glfwGetFramebufferSize(window, &framebuffer_width,
                                   &framebuffer_height);
            game.OnFramebufferResize(framebuffer_width, framebuffer_height);
        }

        timer.Tick();
        while (!glfwWindowShouldClose(window)) {
            input.Update();
            glfwPollEvents();
            timer.Tick();
            game.Update(input, timer.GetElapsedSeconds());

            game.Draw();
            glfwSwapBuffers(window);

            std::this_thread::sleep_for(16ms);
        }
    }

    void OnKeyChanged(i32 key, i32 scancode, i32 action, i32 mods) {
        input.OnKeyChanged(key, scancode, action, mods);
    }

    void OnFramebufferResize(i32 width, i32 height) {
        game.OnFramebufferResize(width, height);
    }

  private:
    const char* window_title = "Tetris3d";
    GLFWmonitor* monitor = nullptr;
    GLFWwindow* window = nullptr;
    Timer timer;
    InputState input;
    Game game;

    bool StartUp() {
        srand(time(nullptr));
        glfwInit();

        if (Settings::graphics_renderer_type == Settings::RendererType::Basic) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        } else {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#if __APPLE__

            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        glfwWindowHint(GLFW_SAMPLES, Settings::graphics_multisampling_samples);

        if (Settings::graphics_fullscreen && Settings::graphics_borderless) {
            monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
            glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
            window = glfwCreateWindow(video_mode->width, video_mode->height,
                                      window_title, monitor, nullptr);
        } else if (Settings::graphics_fullscreen) {
            window =
                glfwCreateWindow(Settings::graphics_resolution_width,
                                 Settings::graphics_resolution_height,
                                 "Tetris3d", glfwGetPrimaryMonitor(), nullptr);
        } else {
            window = glfwCreateWindow(Settings::graphics_resolution_width,
                                      Settings::graphics_resolution_height,
                                      "Tetris3d", nullptr, nullptr);
        }

        if (!window) {
            printf("Failed to create GLFW window\n");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            printf("Failed to initialize GLAD\n");
            return false;
        }

        fprintf(stdout,"%s\n",glGetString(GL_VERSION));
        fprintf(stdout,"%s\n",glGetString(GL_VENDOR));
        fprintf(stdout,"%s\n",glGetString(GL_RENDERER));
        fprintf(stdout,"%s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));


        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, ::OnFramebufferResize);
        glfwSetKeyCallback(window, OnKeyCallback);

        return true;
    }
};

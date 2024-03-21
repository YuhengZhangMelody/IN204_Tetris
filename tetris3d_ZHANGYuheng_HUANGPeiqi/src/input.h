#pragma once

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "common.h"

class InputState {
  public:
    InputState() {
        prev_keyboard.fill(GLFW_RELEASE);
        keyboard.fill(GLFW_RELEASE);
    }

    void Update() {
        prev_keyboard = keyboard;
    }

    void OnKeyChanged(i32 key, i32 scancode, i32 action, i32 mods) {
        // NOTE(panmar): GLFW_REPEAT is converted to GLFW_PRESS
        keyboard[key] = action == GLFW_RELEASE ? GLFW_RELEASE : GLFW_PRESS;
    }

    bool IsKeyDown(i32 key) const {
        assert(key < static_cast<i32>(keyboard.size()));
        return keyboard[key] == GLFW_PRESS;
    }

    bool IsKeyPressed(i32 key) const {
        assert(key < static_cast<i32>(keyboard.size()));
        return keyboard[key] == GLFW_PRESS &&
               prev_keyboard[key] == GLFW_RELEASE;
    }

  private:
    std::array<i32, GLFW_KEY_LAST + 1> keyboard;
    std::array<i32, GLFW_KEY_LAST + 1> prev_keyboard;
};
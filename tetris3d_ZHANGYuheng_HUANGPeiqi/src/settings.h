#pragma once
 
#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "common.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"

namespace Settings {

enum class RendererType { Basic, Advanced };

const RendererType graphics_renderer_type = RendererType::Advanced;
const u32 graphics_resolution_width = 1920;
const u32 graphics_resolution_height = 1080;
const bool graphics_fullscreen = false;
const bool graphics_borderless = true;
const bool graphics_multisampling = true;
const bool graphics_multisampling_samples = 4;

const f32 camera_zoom_min_fov = glm::pi<f32>() / 8.f;
const f32 camera_zoom_max_fov = 4.f * glm::pi<f32>() / 5.f;

const i32 key_playground_rotate_left = GLFW_KEY_LEFT;
const i32 key_playground_rotate_right = GLFW_KEY_RIGHT;
const i32 key_playground_rotate_up = GLFW_KEY_UP;
const i32 key_playground_rotate_down = GLFW_KEY_DOWN;
const i32 key_block_vert_rot_away = GLFW_KEY_W;
const i32 key_block_vert_rot_towards = GLFW_KEY_S;
const i32 key_block_horiz_rot_clock = GLFW_KEY_A;
const i32 key_block_horiz_rot_counterclock = GLFW_KEY_D;
const i32 key_block_move_away = GLFW_KEY_Q;
const i32 key_block_move_towards = GLFW_KEY_E;
const i32 key_block_accelerate = GLFW_KEY_SPACE;
const i32 key_quit = GLFW_KEY_ESCAPE;

const u32 map_width = 10;
const u32 map_depth = 10;
const u32 map_height = 22;

const f32 block_init_fall_step_seconds = 1 / 2.f;
const f32 block_max_fall_step_seconds = 1 / 25.f;
const f32 block_speed_inc_multiplier = 0.02f;
const f32 block_speed_inc_period_seconds = 10.f;

};
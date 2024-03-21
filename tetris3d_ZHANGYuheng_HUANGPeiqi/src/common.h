#pragma once

#include <cstdint>

#include <array>
#include <vector>

using u8 = uint8_t;
using i8 = int8_t;
using u32 = uint32_t;
using i32 = int32_t;
using f32 = float;
using f64 = double;

struct Color {
    f32 r = 0.f;
    f32 g = 0.f;
    f32 b = 0.f;
    f32 a = 1.f;
};

struct ColorR8G8B8 {
    u8 r = 0;
    u8 g = 0;
    u8 b = 0;
};

inline u32 PackColor(const ColorR8G8B8& color) {
    return (color.r << 16) | (color.g << 8) | color.b;
}

inline Color FromPackedColorToColor(u32 color_pack) {
    Color color;
    color.r = ((color_pack & 0xff0000) >> 16) / 255.f;
    color.g = ((color_pack & 0xff00) >> 8) / 255.f;
    color.b = (color_pack & 0xff) / 255.f;
    return color;
}
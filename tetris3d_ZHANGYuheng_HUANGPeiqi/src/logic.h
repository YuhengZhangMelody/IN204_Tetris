#pragma once

#include <algorithm>
#include <random>

#include "common.h"
#include "glm/vec3.hpp"
#include "input.h"
#include "settings.h"

namespace GameLogic {

enum class BlockType {  // block types
    IShape,
    LShape,
    JShape,
    OShape,
    SShape,
    TShape,
    ZShape,

    Undefined //for initialization
};

class Board3D;

class Block {
  public:
    // Create a block
    static Block Create(BlockType type, const ColorR8G8B8& color,
                        const Board3D& board);
    static Block CreateRandom(const Board3D& board);

    // move block
    void Translate(const glm::ivec3& value);
    // rotate block
    void RotateXClockwise();
    void RotateXCounterClockwise();
    void RotateYClockwise();
    void RotateYCounterClockwise();
    void RotateZClockwise();
    void RotateZCounterClockwise();

    bool TryTranslate(const Board3D& board, const glm::ivec3& value);
    bool TryRotateXClockwise(const Board3D& board);
    bool TryRotateXCounterClockwise(const Board3D& board);
    bool TryRotateYClockwise(const Board3D& board);
    bool TryRotateYCounterClockwise(const Board3D& board);
    bool TryRotateZClockwise(const Board3D& board);
    bool TryRotateZCounterClockwise(const Board3D& board);

    // rotate block with fix the invalid position
    bool TryRotateXClockwiseWithFix(const Board3D& board);
    bool TryRotateXCounterClockwiseWithFix(const Board3D& board);
    bool TryRotateYClockwiseWithFix(const Board3D& board);
    bool TryRotateYCounterClockwiseWithFix(const Board3D& board);
    bool TryRotateZClockwiseWithFix(const Board3D& board);
    bool TryRotateZCounterClockwiseWithFix(const Board3D& board);

    bool IsValid(const Board3D& board) const;
    bool IsCollidingWithOtherBlocks(const Board3D& board) const;
    bool TryFix(const Board3D& board, const Block& prev_block);

    void GetWorldBounds(glm::ivec3& min, glm::ivec3& max) const;

    BlockType type = BlockType::Undefined;

    glm::ivec3 position;

    std::vector<glm::ivec3> cube_offsets;
    ColorR8G8B8 color;
};

class Board3D {
  public:
    // Create a board
    Board3D(u32 _width = Settings::map_width, u32 _depth = Settings::map_depth,
            u32 _height = Settings::map_height)
        : width(_width), depth(_depth), height(_height),
          cells(width * depth * height, 0) {}
    
    // Fill the board with block
    void Fill(const glm::ivec3& position, u32 value) {
        auto index = PositionToIndex(position);
        cells[index] = value;
    }

    // Check if the position is empty
    bool IsEmpty(const glm::ivec3& position) const {
        auto index = PositionToIndex(position);
        return !cells[index];
    }

    // Check if the position is filled
    bool Contains(const glm::ivec3& position) const {
        if (position.x < 0 || position.y < 0 || position.z < 0 ||
            position.x >= static_cast<i32>(width) ||
            position.y >= static_cast<i32>(height) ||
            position.z >= static_cast<i32>(depth)) {
            return false;
        }
        return true;
    }

    size_t PositionToIndex(const glm::vec3& world_pos) const {
        return (world_pos.x * width + world_pos.z) +
               (world_pos.y * width * depth);
    }

    // when layer is filled, erase the layer and return the number of filled layers
    u32 EraseFilledLayers() {
        auto filled_layers = 0U;
        for (size_t layer = 0; layer < height;) {
            if (IsLayerFilled(layer)) {
                EraseLayer(layer);
                ++filled_layers;
            } else {
                ++layer;
            }
        }
        return filled_layers;
    }

    void EraseLayer(u32 layer) {
        for (size_t i = 0; i < width; ++i) {
            for (size_t j = 0; j < depth; ++j) {
                auto index = (i * width + j) + (layer * width * depth);
                cells[index] = 0;
            }
        }

        {
            for (size_t layer1 = layer; layer1 < height - 1; ++layer1) {
                auto layer2 = layer1 + 1;
                for (size_t i = 0; i < width; ++i) {
                    for (size_t j = 0; j < depth; ++j) {
                        auto index1 =
                            (i * width + j) + (layer1 * width * depth);
                        auto index2 =
                            (i * width + j) + (layer2 * width * depth);
                        cells[index1] = cells[index2];
                        cells[index2] = 0;
                    }
                }
            }
        }
    }

    bool IsLayerFilled(u32 layer) const {
        for (size_t i = 0; i < width; ++i) {
            for (size_t j = 0; j < depth; ++j) {
                auto index = (i * width + j) + (layer * width * depth);
                if (!cells[index]) {
                    return false;
                }
            }
        }
        return true;
    }

    const u32 width = 0;
    const u32 depth = 0;
    const u32 height = 0;

    std::vector<u32> cells;
};

struct GameState {
    GameState(
        f32 block_init_fall_step_seconds =
            Settings::block_init_fall_step_seconds,
        f32 block_max_fall_step_seconds = Settings::block_max_fall_step_seconds,
        f32 block_speed_inc_multiplier = Settings::block_speed_inc_multiplier,
        f32 block_speed_inc_period_seconds =
            Settings::block_speed_inc_period_seconds)
        : block_current_speed(block_init_fall_step_seconds),
          block_current_normal_speed(block_init_fall_step_seconds),
          block_max_fall_step_seconds(block_max_fall_step_seconds),
          block_speed_inc_multiplier(block_speed_inc_multiplier),
          block_speed_inc_period_seconds(block_speed_inc_period_seconds),
          seconds_from_last_speed_inc(block_speed_inc_period_seconds),
          seconds_to_next_block_fall(block_init_fall_step_seconds) {}

    Board3D board;
    Block falling_block;

    int score = 0; // current score
    int level = 0; // current level

    enum class Phase {
        Uninitialized,
        NewBlockCreation,
        BlockFalling,
        BlockMerge,
        LayersErase,
        Lost
    };

    Phase phase = Phase::Uninitialized;

    bool paused = false;

    f32 block_current_speed = 0.f;
    f32 block_current_normal_speed = 0.f;
    const f32 block_max_fall_step_seconds = 0.f;
    const f32 block_speed_inc_multiplier = 0.f;
    const f32 block_speed_inc_period_seconds = 0.f;

    f32 total_time = 0.f;
    f32 seconds_from_last_speed_inc = 0.f;
    f32 seconds_to_next_block_fall = 0.f;
};

int calculateGameScore(u32 linesCleared, int level);

void processGameUpdate(GameState& state, f32 elapsedSeconds, const InputState& input,
            const glm::vec3& viewDirection);


void SingleStep(GameState& state);

bool IsFallingBlockOutOfBounds(const GameState& state);

bool CanFallingBlockFall(const GameState& state);

void MergeFallingBlock(GameState& state);

};

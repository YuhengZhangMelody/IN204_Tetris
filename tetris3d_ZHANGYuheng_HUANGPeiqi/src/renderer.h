#pragma once

#include <string>

#include "camera.h"
#include "common.h"
#include "logic.h"

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "glm/vec3.hpp"
#include "font.h"

class IRenderer {
  public:
    virtual ~IRenderer() = default;
    virtual void Initialize(const GameLogic::GameState& state){};
    virtual void Render(const GameLogic::GameState& state,
                        const Camera& camera) = 0;
    virtual void SetFramebufferWidth(i32 value) = 0;
    virtual void SetFramebufferHeight(i32 value) = 0;
};

class Shader {
  public:
    ~Shader();
    void Create(const std::string& vs_str, const std::string& fs_str);
    // Set uniform variables
    void SetParam(const char* name, bool value) const;
    void SetParam(const char* name, i32 value) const;
    void SetParam(const char* name, f32 value) const;
    void SetParam(const char* name, const glm::vec2& value) const;
    void SetParam(const char* name, f32 x, f32 y) const;
    void SetParam(const char* name, const glm::vec3& value) const;
    void SetParam(const char* name, f32 x, f32 y, f32 z) const;
    void SetParam(const char* name, const glm::vec4& value) const;
    void SetParam(const char* name, f32 x, f32 y, f32 z, f32 w) const;
    void SetParam(const char* name, const glm::mat2& value) const;
    void SetParam(const char* name, const glm::mat3& value) const;
    void SetParam(const char* name, const glm::mat4& value) const;
    void SetParam(const char* name, const Color& color) const;
    void Bind();

  private:
    bool LogShaderErrorsIfAny(u32 shader);
    bool LogProgramErrorsIfAny(u32 shader);
    u32 id = 0;
};

class SkyBox {
  public:
    ~SkyBox();
    void Create();
    void Render(Shader& shader, const Camera& camera);

    u32 LoadCubemap(const std::vector<std::string>& paths);

    Color color = Color{1.f, 1.f, 1.f, 1.f};
    u32 vao = 0;
    u32 vbo = 0;
    u32 texture = 0;
};

class Cube {
  public:
    void Create(f32 width, f32 depth, f32 height);
    ~Cube();
    void Render(Shader& shader, const Color& color, const glm::mat4& world,
                const Camera& camera);

  private:
    u32 vbo = 0;
    u32 vao = 0;
};

class BoardBounds {
  public:
    ~BoardBounds();
    void Create(const GameLogic::Board3D& board);
    void Render(Shader& shader, const Color& color, const Camera& camera);

  private:
    u32 vbo_wall = 0;
    u32 vao_wall = 0;
    u32 vbo_ground = 0;
    u32 vao_ground = 0;
    u32 ground_vertex_count = 0;
    u32 wall_vertex_count = 0;
    glm::ivec3 board_dimensions;
};




class AdvancedRenderer : public IRenderer {
  public:
    ~AdvancedRenderer() {}
    void Initialize(const GameLogic::GameState& state) override;
    void Render(const GameLogic::GameState& state,
                const Camera& camera) override;
    void SetFramebufferWidth(i32 value) override {
        framebuffer_width = value;
        font.SetResolution(framebuffer_width, framebuffer_height);
    }
    void SetFramebufferHeight(i32 value) override {
        framebuffer_height = value;
        font.SetResolution(framebuffer_width, framebuffer_height);
    }

  private:
    void RenderBoard(const GameLogic::Board3D& board, const Camera& camera);
    void RenderFallingBlock(const GameLogic::Block& block,
                            const Camera& camera);
    void RenderFallingBlockProjection(const GameLogic::GameState& state,
                                      const Camera& camera);
    void RenderCube_style1(const glm::vec3& pos, const Color& color,  // render tetris cube with edge(black) and surface(color)
                          const Camera& camera);
    void RenderCube(const glm::vec3& position, const Color& color, // render tetris cube with a color
                    const glm::vec3& scale, const Camera& camera);


    i32 framebuffer_width = 0;
    i32 framebuffer_height = 0;

    Shader solid_shader;
    Shader solid_wire_shader;
    Shader refract_shader;
    Shader skybox_shader;
    Shader score_shader;
    Cube tetris_cube;
    BoardBounds board_bounds;
    SkyBox skybox;
    Scoreboard font;
};

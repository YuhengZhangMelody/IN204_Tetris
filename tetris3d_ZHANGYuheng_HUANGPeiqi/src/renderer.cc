#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {

const std::string solid_vs = R"~(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * world * vec4(position, 1.0);
}

)~";

const std::string solid_fs = R"~(
#version 330 core
out vec4 finalcolor;

uniform vec4 color;

void main() {
    finalcolor = color;
}

)~";

const std::string solid_wire_vs = R"~(

#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * world * vec4(position, 1.0);
}

)~";

const std::string solid_wire_fs = R"~(

#version 330 core
out vec4 final_color;

uniform vec4 color;

void main() {
    final_color = color;
}

)~";

const std::string refract_vs = R"~(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    Normal = mat3(transpose(inverse(world))) * aNormal;
    Position = vec3(world * vec4(aPos, 1.0));
    gl_Position = projection * view * world * vec4(aPos, 1.0);
}

)~";

const std::string refract_fs = R"~(

#version 330 core
out vec4 final_color;

in vec3 Normal;
in vec3 Position;

uniform vec3 camera_pos;
uniform samplerCube skybox;
uniform vec4 color;

void main() {             
    vec3 I = normalize(Position - camera_pos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 texture_color = vec4(texture(skybox, R).rgb, color.a);
    vec4 mixed_color = mix(color, texture_color, 0.4);
    final_color = mixed_color;
}

)~";

const std::string skybox_vs = R"~(

#version 330 core
layout (location = 0) in vec3 position;

out vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    tex_coords = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}

)~";

const std::string skybox_fs = R"~(

#version 330 core
out vec4 final_color;

in vec3 tex_coords;

uniform samplerCube skybox;
uniform vec4 color;

void main() {    
    final_color = color * texture(skybox, tex_coords) * 0.75;
}

)~";

const std::string score_vs = R"(

#version 330 core
layout (location = 0) in vec2 position;

const vec2 inTexcoords[4]=vec2[4](vec2(0,1),vec2(1,1),vec2(1,0),vec2(0,0));

out vec2 tex_coords;

void main() {
    tex_coords = inTexcoords[gl_VertexID];
    vec3 pos =  vec3(position, 1.0);
    gl_Position = vec4(pos.xy,0,1);
}
)";
const std::string score_fs = R"(

#version 330 core
layout (location = 0) out vec4 fragColor;

in vec2 tex_coords;
uniform sampler2D tex;

uniform vec4 color;
uniform int useTex;

void main() {
    if(bool(useTex))
        fragColor = texture(tex,tex_coords).rrrr;
    else
        fragColor = color;
}
)";

} 



Shader::~Shader() {
    if (id) {
        glDeleteShader(id);
    }
}

void Shader::Create(const std::string& vs_str, const std::string& fs_str) {
    u32 vs = glCreateShader(GL_VERTEX_SHADER);
    const char* vs_cstr = vs_str.c_str();
    glShaderSource(vs, 1, &vs_cstr, nullptr);
    glCompileShader(vs);
    LogShaderErrorsIfAny(vs);
    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fs_cstr = fs_str.c_str();
    glShaderSource(fs, 1, &fs_cstr, nullptr);
    glCompileShader(fs);
    LogShaderErrorsIfAny(fs);
    this->id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    LogProgramErrorsIfAny(id);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::SetParam(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name), static_cast<i32>(value));
}

void Shader::SetParam(const char* name, i32 value) const {
    glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::SetParam(const char* name, f32 value) const {
    glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::SetParam(const char* name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void Shader::SetParam(const char* name, f32 x, f32 y) const {
    glUniform2f(glGetUniformLocation(id, name), x, y);
}

void Shader::SetParam(const char* name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void Shader::SetParam(const char* name, f32 x, f32 y, f32 z) const {
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::SetParam(const char* name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void Shader::SetParam(const char* name, f32 x, f32 y, f32 z, f32 w) const {
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void Shader::SetParam(const char* name, const glm::mat2& value) const {
    glUniformMatrix2fv(glGetUniformLocation(id, name), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetParam(const char* name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetParam(const char* name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetParam(const char* name, const Color& color) const {
    SetParam(name, color.r, color.g, color.b, color.a);
}

void Shader::Bind() {
    assert(id != 0);
    glUseProgram(id);
}

bool Shader::LogShaderErrorsIfAny(u32 shader) {
    char message[1024];
    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, message);
        printf("Shader compilation error:\n");
        printf("%s\n", message);
    }
    return success != 0;
}

bool Shader::LogProgramErrorsIfAny(u32 program) {
    char message[1024];
    i32 success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, message);
        printf("Program compilation error:\n");
        printf("%s\n", message);
    }
    return success != 0;
}

void Cube::Create(f32 width, f32 depth, f32 height) {
    auto wOver2 = width / 2.f;
    auto hOver2 = height / 2.f;
    auto dOver2 = depth / 2.f;
    std::vector<glm::vec3> positions = {
        {-wOver2, -hOver2, -dOver2}, {-wOver2, hOver2, -dOver2},
        {wOver2, hOver2, -dOver2},   {-wOver2, -hOver2, -dOver2},
        {wOver2, hOver2, -dOver2},   {wOver2, -hOver2, -dOver2},
        {wOver2, -hOver2, -dOver2},  {wOver2, hOver2, -dOver2},
        {wOver2, -hOver2, dOver2},   {wOver2, -hOver2, dOver2},
        {wOver2, hOver2, -dOver2},   {wOver2, hOver2, dOver2},
        {wOver2, hOver2, -dOver2},   {-wOver2, hOver2, -dOver2},
        {-wOver2, hOver2, dOver2},   {wOver2, hOver2, -dOver2},
        {-wOver2, hOver2, dOver2},   {wOver2, hOver2, dOver2},
        {wOver2, -hOver2, dOver2},   {-wOver2, hOver2, dOver2},
        {-wOver2, -hOver2, dOver2},  {wOver2, -hOver2, dOver2},
        {wOver2, hOver2, dOver2},    {-wOver2, hOver2, dOver2},
        {-wOver2, -hOver2, dOver2},  {-wOver2, hOver2, -dOver2},
        {-wOver2, -hOver2, -dOver2}, {-wOver2, -hOver2, dOver2},
        {-wOver2, hOver2, dOver2},   {-wOver2, hOver2, -dOver2},
        {-wOver2, -hOver2, -dOver2}, {wOver2, -hOver2, -dOver2},
        {-wOver2, -hOver2, dOver2},  {wOver2, -hOver2, -dOver2},
        {wOver2, -hOver2, dOver2},   {-wOver2, -hOver2, dOver2},
    };

    std::vector<glm::vec3> normals;
    for (size_t i = 0; i < positions.size(); i += 3) {
        auto v1 = positions[i] - positions[i + 1];
        auto v2 = positions[i + 2] - positions[i + 1];
        auto normal = glm::normalize(glm::cross(v1, v2));
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);
    }

    assert(positions.size() == normals.size());

    std::vector<glm::vec3> positions_normals;
    for (size_t i = 0; i < positions.size(); ++i) {
        positions_normals.push_back(positions[i]);
        positions_normals.push_back(normals[i]);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3) * positions.size(),
                 positions_normals.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(f32),
                          (void*)(3 * sizeof(float)));
}

Cube::~Cube() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
}

void Cube::Render(Shader& shader, const Color& color, const glm::mat4& world,
                  const Camera& camera) {
    shader.Bind();
    shader.SetParam("world", world);
    shader.SetParam("view", camera.GetView());
    shader.SetParam("projection", camera.GetProjection());
    shader.SetParam("color", color);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}


BoardBounds::~BoardBounds() {
    if (vbo_ground) {
        glDeleteBuffers(1, &vbo_ground);
    }
    if (vao_ground) {
        glDeleteVertexArrays(1, &vao_ground);
    }
    if (vbo_wall) {
        glDeleteBuffers(1, &vbo_wall);
    }
    if (vao_wall) {
        glDeleteVertexArrays(1, &vao_wall);
    }
}

void BoardBounds::Create(const GameLogic::Board3D& board) {
    board_dimensions = glm::ivec3(board.width, board.height, board.depth);

    {
        std::vector<glm::vec3> vertices;
        for (auto i = 0.f; i < board.width + 0.1f; i += 1.f) {
            vertices.emplace_back(i, 0.f, 0.f);
            vertices.emplace_back(i, 0.f, board.depth);
            vertices.emplace_back(0.f, 0.f, i);
            vertices.emplace_back(board.width, 0.f, i);
        }
        ground_vertex_count = vertices.size();

        glGenVertexArrays(1, &vao_ground);
        glBindVertexArray(vao_ground);

        glGenBuffers(1, &vbo_ground);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ground);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3),
                              (void*)0);
        glEnableVertexAttribArray(0);
    }


    {
        std::vector<glm::vec3> vertices;
        for (auto i = 0.f; i < board.width + 0.1f; i += 1.f) {
            vertices.emplace_back(i, 0.f, 0.f);
            vertices.emplace_back(i, board.height, 0.f);
        }

        for (auto i = 1.f; i < board.height + 0.1f; i += 1.f) {
            vertices.emplace_back(0.f, i, 0.f);
            vertices.emplace_back(board.width, i, 0.f);
        }

        wall_vertex_count = vertices.size();

        glGenVertexArrays(1, &vao_wall);
        glBindVertexArray(vao_wall);

        glGenBuffers(1, &vbo_wall);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_wall);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3),
                              (void*)0);
        glEnableVertexAttribArray(0);
    }
}

void BoardBounds::Render(Shader& shader, const Color& color,
                         const Camera& camera) {

    {
        shader.Bind();
        shader.SetParam("world", glm::mat4(1.f));
        shader.SetParam("view", camera.GetView());
        shader.SetParam("projection", camera.GetProjection());
        shader.SetParam("color", color);
        glBindVertexArray(vao_ground);
        glDrawArrays(GL_LINES, 0, ground_vertex_count);
    }

    {
        auto view_dir = camera.GetForward();
        {
            glm::vec3 wall_dir{0, 0, 1};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glBindVertexArray(vao_wall);
                glDrawArrays(GL_LINES, 0, wall_vertex_count);
            }
        }

        {
            glm::vec3 wall_dir{1, 0, 0};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glm::mat4 world(1.f);
                world = glm::rotate(world, -glm::pi<f32>() / 2,
                                    glm::vec3(0.f, 1.f, 0.f));
                shader.SetParam("world", world);
                glBindVertexArray(vao_wall);
                glDrawArrays(GL_LINES, 0, wall_vertex_count);
            }
        }

        {
            glm::vec3 wall_dir{0, 0, -1};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glm::mat4 world(1.f);
                world = glm::translate(world,
                                       glm::vec3(0.f, 0.f, board_dimensions.z));
                shader.SetParam("world", world);
                glBindVertexArray(vao_wall);
                glDrawArrays(GL_LINES, 0, wall_vertex_count);
            }
        }

        {
            glm::vec3 wall_dir{-1, 0, 0};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glm::mat4 world(1.f);
                world = glm::rotate(world, -glm::pi<f32>() / 2,
                                    glm::vec3(0.f, 1.f, 0.f));
                world = glm::translate(
                    world, glm::vec3(0.f, 0.f, -board_dimensions.z));
                shader.SetParam("world", world);
                glBindVertexArray(vao_wall);
                glDrawArrays(GL_LINES, 0, wall_vertex_count);
            }
        }
    }
}

SkyBox::~SkyBox() {}

void SkyBox::Create() {
    float vertices[] = {
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(f32), (void*)0);


    std::vector<std::string> paths = {
        "data/skybox/px.jpeg", "data/skybox/nx.jpeg",
        "data/skybox/nz.jpeg",   "data/skybox/pz.jpeg",
        "data/skybox/ny.jpeg", "data/skybox/py.jpeg",
    };
    texture = LoadCubemap(paths);
}

void SkyBox::Render(Shader& shader, const Camera& camera) {
    shader.Bind();
    shader.SetParam("skybox", 0);
    shader.SetParam("color", color);

    glDepthFunc(GL_LEQUAL);
    auto view_no_translation = glm::mat4(glm::mat3(camera.GetView()));
    shader.SetParam("view", view_no_translation);
    shader.SetParam("projection", camera.GetProjection());
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

u32 SkyBox::LoadCubemap(const std::vector<std::string>& paths) {
    u32 id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, nr_components;
    for (unsigned int i = 0; i < paths.size(); i++) {
        unsigned char* data =
            stbi_load(paths[i].c_str(), &width, &height, &nr_components, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            printf("Texuture load failure: %s\n", paths[i].c_str());
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return id;
}



void AdvancedRenderer::Initialize(const GameLogic::GameState& state) {
    score_shader.Create(score_vs, score_fs);
    solid_shader.Create(solid_vs, solid_fs);
    solid_wire_shader.Create(solid_wire_vs, solid_wire_fs);
    refract_shader.Create(refract_vs, refract_fs);
    skybox_shader.Create(skybox_vs, skybox_fs);
    tetris_cube.Create(1.f, 1.f, 1.f);
    board_bounds.Create(state.board);
    skybox.Create();
    font.SetSize(5);
}

void AdvancedRenderer::Render(const GameLogic::GameState& state,
                              const Camera& camera) {
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    if (state.phase == GameLogic::GameState::Phase::Lost) {
        skybox.color = {0.9f, 0.1f, 0.1f, 1.f};
    } else if (state.phase == GameLogic::GameState::Phase::LayersErase) {
        skybox.color = {0.2f, 0.9f, 0.2f, 1.f};
    } else if (state.paused) {
        skybox.color = {0.2f, 0.2f, 0.9f, 1.f};
    } else {
        skybox.color = {1.f, 1.f, 1.f, 1.f};
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (Settings::graphics_multisampling) {
        glEnable(GL_MULTISAMPLE);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    board_bounds.Render(solid_wire_shader, Color{0.3f, 0.3f, 0.3f}, camera);

    RenderBoard(state.board, camera);
    RenderFallingBlock(state.falling_block, camera);
    if (state.phase == GameLogic::GameState::Phase::BlockFalling) {
        RenderFallingBlockProjection(state, camera);
    }

    skybox.Render(skybox_shader, camera);

    score_shader.Bind();

    score_shader.SetParam("useTex", 0);

    score_shader.SetParam("color", glm::vec4(0.2, 0.2, 0.2, 1));

    font.RenderBackground(-0.2, 0.75, 0.32, 0.15);

    score_shader.SetParam("useTex", 1);
    static std::string a = "Score:";

    font.RenderString(a + std::to_string(state.score), -0.2, 0.8);

    {
        auto error = glGetError();
        if (error != GL_NO_ERROR) {
            printf("Error = %d\n", error);
        }
        assert(error == GL_NO_ERROR);
    }
}

void AdvancedRenderer::RenderBoard(const GameLogic::Board3D& board,
                                   const Camera& camera) {
    for (size_t k = 0; k < board.height; ++k) {
        for (size_t i = 0; i < board.width; ++i) {
            for (size_t j = 0; j < board.depth; ++j) {
                auto index =
                    (i * board.width + j) + (k * board.width * board.depth);
                if (board.cells[index]) {
                    auto position = glm::vec3(i, k, j);
                    Color color = FromPackedColorToColor(board.cells[index]);
                    RenderCube_style1(position + glm::vec3(0.5f, 0.5f, 0.5f),
                                     color, camera);
                }
            }
        }
    }
}

void AdvancedRenderer::RenderFallingBlock(const GameLogic::Block& block,
                                          const Camera& camera) {
    Color color;
    color.r = block.color.r / 255.f;
    color.g = block.color.g / 255.f;
    color.b = block.color.b / 255.f;

    for (auto& offset : block.cube_offsets) {
        auto position =
            glm::vec3(block.position + offset) + glm::vec3(0.5f, 0.5f, 0.5f);
        RenderCube_style1(position, color, camera);
    }
}

void AdvancedRenderer::RenderFallingBlockProjection(
    const GameLogic::GameState& state, const Camera& camera) {
    auto projected_block = state.falling_block;
    while (projected_block.IsValid(state.board)) {
        projected_block.Translate(glm::ivec3(0, -1, 0));
    }
    projected_block.Translate(glm::ivec3(0, 1, 0));

    Color color{0.9, 0.9f, 0.9f, 0.6f};

    glColorMask(false, false, false, false);
    for (auto& offset : projected_block.cube_offsets) {
        RenderCube(glm::vec3(projected_block.position + offset) +
                       glm::vec3(0.5f, 0.5f, 0.5f),
                   color, glm::vec3(1.f, 1.f, 1.f), camera);
    }

    glColorMask(true, true, true, true);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto& offset : projected_block.cube_offsets) {
        RenderCube(glm::vec3(projected_block.position + offset) +
                       glm::vec3(0.5f, 0.5f, 0.5f),
                   color, glm::vec3(1.f, 1.f, 1.f), camera);
    }
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
}

void AdvancedRenderer::RenderCube_style1(const glm::vec3& pos,
                                        const Color& color,
                                        const Camera& camera) {
    Color cube_edge_color{0.1f, 0.1f, 0.1f};
    RenderCube(pos, cube_edge_color, glm::vec3(1.0f, 1.0f, 1.0f), camera);
    RenderCube(pos, color, glm::vec3(1.01f, 0.8f, 0.8f), camera);
    RenderCube(pos, color, glm::vec3(0.8f, 1.01f, 0.8f), camera);
    RenderCube(pos, color, glm::vec3(0.8f, 0.8f, 1.01f), camera);

}

void AdvancedRenderer::RenderCube(const glm::vec3& position, const Color& color,
                                  const glm::vec3& scale,
                                  const Camera& camera) {

    glm::mat4 world(1.f);
    world = glm::translate(world, position);
    world = glm::scale(world, scale);
    tetris_cube.Render(solid_shader, color, world, camera);
}

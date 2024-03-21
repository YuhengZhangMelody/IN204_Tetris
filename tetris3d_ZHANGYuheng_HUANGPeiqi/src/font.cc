#include "font.h"

Scoreboard::Scoreboard() {
    auto error = FT_Init_FreeType(&library);
    if (error) {
        fprintf(stderr, "failed to init freetype %d\n", error);
    }

    error = FT_New_Face(library, "data/monofonto.regular.otf", 0, &face);

    if (error == FT_Err_Unknown_File_Format) {
        fprintf(stderr, "the font file could be opened and read, but it appears that its font format is unsupported\n");
    } else if (error) {
        fprintf(stderr,
                "another error code means that the font file could not be opened or read, or that it is broken\n");
    }
}
Scoreboard::~Scoreboard() {
    FT_Done_Face(face);

    FT_Done_FreeType(library);

    for (auto&& e : characters) glDeleteTextures(1, &e.second.tex);
}
void Scoreboard::SetResolution(int w, int h) {
    width = w;
    height = h;
    Update();
}
void Scoreboard::SetSize(float sizeH) {
    fontSizeH = sizeH;
    Update();
}
void Scoreboard::Update() {
    characters.clear();

    auto error = FT_Set_Char_Size(face,0, fontSizeH * 64, width,height); 
    if (error) {
        fprintf(stderr, "%d: failed to set font size\n", __LINE__);
    }
}
Character Scoreboard::LoadCharacter(FT_ULong c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        fprintf(stderr, "%d: failed to load glyph\n", __LINE__);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    Character ret{0,
                  static_cast<int>(face->glyph->bitmap.width),
                  static_cast<int>(face->glyph->bitmap.rows),
                  face->glyph->bitmap_left,
                  face->glyph->bitmap_top,
                  static_cast<int>(face->glyph->advance.x)};
    glGenTextures(1, &ret.tex);
    glBindTexture(GL_TEXTURE_2D, ret.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                 GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    return ret;
}
const Character& Scoreboard::GetCharactor(FT_ULong c) {
    auto it = characters.find(c);
    if (it == characters.end()) return characters[c] = LoadCharacter(c);
    return it->second;
}
void Scoreboard::Init() {
    static bool flag = true;
    if (flag) {
        flag = false;

        positions = {glm::vec2{0, 0}, glm::vec2{1, 0}, glm::vec2{1, 1}, glm::vec2{0, 1}};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArray(0);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
void Scoreboard::RenderString(std::string const& str, float offsetX, float offsetY) {
    Init();


    auto sx = 1. / width;
    auto sy = 1. / height;

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    float sumX = 0;
    for (auto c : str) {
        // auto glyphIndex = FT_Get_Char_Index(face, c);


        auto&& ch = GetCharactor(c);
        float x = ch.bitmap_left;
        float t = ch.bitmap_top;
        float w = ch.SizeX;
        float h = ch.SizeY;
        sumX += float(ch.Advance >> 6);

        positions = {
            glm::vec2{x + sumX, t - h},
            glm::vec2{x + w + sumX, t - h},
            glm::vec2{x + w + sumX, t},
            glm::vec2{x + sumX, t},
        };
        for (auto&& e : positions) {
            e = glm::mat2(sx, 0, 0, sy) * e;
            e.x += offsetX;
            e.y += offsetY;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions[0]) * positions.size(), positions.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, ch.tex);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
void Scoreboard::RenderBackground(float offsetX, float offsetY, float w, float h) {
    Init();


    positions = {glm::vec2{0, 0}, glm::vec2{1, 0}, glm::vec2{1, 1}, glm::vec2{0, 1}};

    for (auto&& e : positions) {
        e = glm::mat2(w, 0, 0, h) * e;
        e.x += offsetX;
        e.y += offsetY;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions[0]) * positions.size(), positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
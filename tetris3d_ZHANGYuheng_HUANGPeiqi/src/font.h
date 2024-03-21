#pragma once
#include "glad/glad.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <array>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/ext.hpp"

struct Character
{
    GLuint tex;
    int SizeX;      
    int SizeY;     
    int bitmap_left; 
    int bitmap_top;  
    int Advance;    
};

class Scoreboard {
    FT_Library library; 
    FT_Face face;      

    int width;
    int height;
    int fontSizeH = 16;

    std::unordered_map<FT_ULong, Character> characters;

    std::array<glm::vec2, 4> positions;
    GLuint vbo;
    GLuint vao;

    Character LoadCharacter(FT_ULong c);

    const Character& GetCharactor(FT_ULong c);

    void Update();

    void Init();

public:
    Scoreboard();
    ~Scoreboard();

    void SetResolution(int w, int h);
    void SetSize(float sizeH);

    void RenderString(std::string const& str, float x, float y);
    void RenderBackground(float x, float y, float w, float h);
};
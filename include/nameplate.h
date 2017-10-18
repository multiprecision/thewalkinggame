// nameplate.h

#ifndef OGLPROJ_NAMEPLATE_H_
#define OGLPROJ_NAMEPLATE_H_

#include "scene.h"
#include "texture.h"
#include "character.h"
#include "shader_program.h"

namespace oglproj
{

class Nameplate : public Renderable
{
public:
    Nameplate(Scene* parent, Character* c, const char* name, Texture* tex);
    virtual ~Nameplate();
    void init();
    virtual void render();
    void set_character(Character* c);
    Character* get_character();
protected:
    bool m_initialized;
    float m_char_width;
    float m_char_height;
    float m_char_kerning;
    std::string m_name;
    Character* m_character;
    Shader_program* m_program;
    glm::vec2 m_transform;
    glm::vec2 m_scale;
    unsigned m_num_vertices;
    Scene* m_parent;
    GLuint m_vao;
    GLuint m_vbo[2];
    Texture* m_tex;
};

}

#endif
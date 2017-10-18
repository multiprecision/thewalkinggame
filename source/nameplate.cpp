// nameplate.cpp

#include "nameplate.h"

#include <gl/glew.h>
#include <glfw/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace oglproj
{

Nameplate::Nameplate(Scene* parent, Character* c, const char* name, Texture* tex)
    : m_parent(parent)
    , m_character(c)
    , m_name(name)
    , m_tex(tex)
    , m_initialized(false)
{
}

void Nameplate::init()
{
    m_program = new Shader_program();
    m_program->compile_shader("../shaders/nameplate.vs.txt", GL_VERTEX_SHADER);
    m_program->compile_shader("../shaders/nameplate.fs.txt", GL_FRAGMENT_SHADER);
    m_program->link();
    m_tex->bind(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    unsigned length = (unsigned)m_name.size();
    m_char_width = 0.045F;
    m_char_height = 0.15F;
    m_char_kerning = 0.017F;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(2, m_vbo);
    glBindVertexArray(m_vao);

    // Fill buffers
    std::vector<glm::vec2> pos;
    std::vector<glm::vec2> uv;
    for (unsigned int i = 0; i < length; i++)
    {
        // These are in screen coordinates
        glm::vec2 vertex_up_left = glm::vec2(i * m_char_width, m_char_height);
        glm::vec2 vertex_up_right = glm::vec2(i * m_char_width + m_char_width, m_char_height);
        glm::vec2 vertex_down_right = glm::vec2(i * m_char_width + m_char_width, 0);
        glm::vec2 vertex_down_left = glm::vec2(i * m_char_width, 0);

        pos.push_back(vertex_up_left);
        pos.push_back(vertex_down_left);
        pos.push_back(vertex_up_right);

        pos.push_back(vertex_down_right);
        pos.push_back(vertex_up_right);
        pos.push_back(vertex_down_left);

        char character = m_name[i];
        float uv_x = (character % 16) / 16.0f;
        float uv_y = (character / 16) / 16.0f;

        glm::vec2 uv_up_left = glm::vec2(uv_x + m_char_kerning, 1.F - uv_y);
        glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f - m_char_kerning, 1.F - uv_y);
        glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f - m_char_kerning, 1.F - (uv_y + 1.0f / 16.0f));
        glm::vec2 uv_down_left = glm::vec2(uv_x + m_char_kerning, 1.F - (uv_y + 1.0f / 16.0f));

        uv.push_back(uv_up_left);
        uv.push_back(uv_down_left);
        uv.push_back(uv_up_right);

        uv.push_back(uv_down_right);
        uv.push_back(uv_up_right);
        uv.push_back(uv_down_left);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec2), &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(1);
    m_num_vertices = (unsigned)pos.size();
    m_initialized = true;
}

Nameplate::~Nameplate()
{
    delete m_program;
    glDeleteBuffers(2, m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Nameplate::render()
{
    if (!m_initialized) init();
    glm::vec3 pos = m_character->get_position() + glm::vec3(0, 0, 2);
    glm::vec4 project = glm::vec4(pos.x, pos.y, pos.z, 1);
    project = m_parent->get_camera()->get_projection_matrix() * m_parent->get_camera()->get_view_matrix() * project;
    project /= project.w;
    // farther from camera means smaller
    //float scale = 3.F / glm::distance(m_parent->get_camera()->get_position(), m_character->get_position());
    // but nameplates size is usually static
    float scale = 0.5F;
    if (std::isinf(scale)) scale = 1.F;
    m_scale = glm::vec2(scale, scale);
    float x = project.x - ((float)m_name.size() * m_char_width / 2 * scale);
    float y = project.y;
    m_transform = glm::vec2(x, y);
    m_program->use();
    if(m_name == "player") m_program->set_uniform("color", glm::vec4(1, 1, 1, 1));
    else m_program->set_uniform("color", glm::vec4(0, 1, 0, 1));
    m_program->set_uniform("transform", m_transform);
    m_program->set_uniform("scale", m_scale);
    // Bind texture
    m_tex->bind(GL_TEXTURE0);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
}

void Nameplate::set_character(Character* c)
{
    m_character = c;
}

Character* Nameplate::get_character()
{
    return m_character;
}

}

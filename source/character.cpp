// player.cpp

#define _USE_MATH_DEFINES
#include <cmath>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "character.h"
#include "main_scene.h"

namespace oglproj
{


Character::Character(Scene* parent, float angle, glm::vec3 position, std::vector<Mesh*>* meshes, Texture* tex, int start_walking_cycle_state)
    : m_parent(parent)
    , m_angle(angle)
    , m_position(position)
    , m_meshes(meshes)
    , m_tex(tex)
    , m_walking_speed(1.1932f) // default walking speed
    , m_last_animation_update(0)
    , m_last_update(0)
    , m_start_walking_cycle_state(start_walking_cycle_state)
    , m_program_linked(false)
{
    m_end_walking_cycle_state = (unsigned)m_meshes->size() - 1;
}

void Character::compile_link_shader()
{
    m_program = new Shader_program;
    m_program->compile_shader("../shaders/phong.vs.txt", GL_VERTEX_SHADER);
    m_program->compile_shader("../shaders/phong.fs.txt", GL_FRAGMENT_SHADER);
    m_program->link();
    m_program_linked = true;
}

void Character::set_state(State d)
{
    m_state = d;
}

Character::State Character::get_state()
{
    return m_state;
}

void Character::update(double t)
{
    m_model_matrix = glm::translate(m_position);
    m_model_matrix *= glm::rotate(m_angle - (float)M_PI * 3 / 2, glm::vec3(0, 0, 1));
    m_model_matrix *= glm::rotate((float)M_PI_2, glm::vec3(1, 0, 0));
    m_model_view_matrix = m_parent->get_camera()->get_view_matrix() *
                          m_model_matrix;
    m_normal_matrix = glm::mat3(glm::vec3(m_model_view_matrix[0]),
                                glm::vec3(m_model_view_matrix[1]),
                                glm::vec3(m_model_view_matrix[2]));
    bool collision = false;
    switch (m_state)
    {
    case State::WALK_FORWARD:
        // increment animation
        if (t - m_last_animation_update >= 0.016667f * 1.2 / m_walking_speed) // 60 fps
        {
            m_last_animation_update = t;
            if (m_animation_state + 1 > m_end_walking_cycle_state)
                m_animation_state = m_start_walking_cycle_state;
            else
            {
                m_animation_state++;
            }
        }
        break;
    case State::WALK_BACKWARD:
        // decrement animation
        if (t - m_last_animation_update >= 0.016667f * 1.2f / m_walking_speed) // 60 fps
        {
            m_last_animation_update = t;
            if (m_animation_state - 1 < m_start_walking_cycle_state)
                m_animation_state = m_end_walking_cycle_state;
            else
            {
                m_animation_state--;
            }
        }
        break;
    case State::TURN_LEFT:
    {
        m_angle += (float)M_PI_4 * float(t - m_last_update);
        if (m_angle >= (float)M_PI * 2)
            m_angle -= (float)M_PI * 2;
        m_parent->get_camera()->add_yaw_angle((float)M_PI_4 * float(t - m_last_update));
        break;
    }
    case State::TURN_RIGHT:
    {
        m_angle -= (float)M_PI_4 * float(t - m_last_update);
        if (m_angle <= -(float)M_PI * 2)
            m_angle += (float)M_PI * 2;
        m_parent->get_camera()->add_yaw_angle((float)-M_PI_4 * float(t - m_last_update));
    }
    default:
        break;
    }
    if (m_state == State::WALK_FORWARD || m_state == State::WALK_BACKWARD)
    {
        m_delta = glm::vec3(
                      std::cosf(m_angle) * (t - m_last_update) * m_walking_speed,
                      std::sinf(m_angle) * (t - m_last_update) * m_walking_speed,
                      0
                  );
        if (m_state == State::WALK_BACKWARD)
            m_delta = -m_delta;
        // check collision
        for (auto p : reinterpret_cast<Main_scene*>(m_parent)->get_characters())
        {
            if (p.second == this) continue;
            if (glm::distance(m_position + m_delta, p.second->get_position()) < 0.5f)
                collision = true;
        }
        if (!collision)
            m_position += m_delta;
    }
    m_last_update = t;
}

void Character::set_animation_state(int state)
{
    m_animation_state = state;
}

int Character::get_animation_state()
{
    return m_animation_state;
}

void Character::set_walking_speed(float speed)
{
    m_walking_speed = speed;
}

float Character::get_walking_speed()
{
    return m_walking_speed;
}

void Character::set_position(glm::vec3 position)
{
    m_position = position;
}

glm::vec3 Character::get_position()
{
    return m_position;
}

void Character::set_angle(float angle)
{
    m_angle = angle;
}

float Character::get_angle()
{
    return m_angle;
}

Scene* Character::get_parent()
{
    return m_parent;
}

std::vector<Mesh*>* Character::get_meshes()
{
    return m_meshes;
}

void Character::set_meshes(std::vector<Mesh*>* meshes)
{
    m_meshes = meshes;
}

void Character::render()
{
    if (!m_program_linked)
        compile_link_shader();
    m_program->use();
    m_tex->bind(GL_TEXTURE0);
    m_program->set_uniform("mvp_matrix",
                           m_parent->get_camera()->get_projection_matrix() *
                           m_model_view_matrix);
    m_program->set_uniform("normal_matrix", m_normal_matrix);
    m_program->set_uniform("model_view_matrix", m_model_view_matrix);
    m_program->set_uniform("light.position", m_parent->get_light()->position);
    m_program->set_uniform("light.intensity", m_parent->get_light()->intensity);
    m_program->set_uniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    m_program->set_uniform("material.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
    m_program->set_uniform("material.specular", glm::vec3(0.9f, 0.9f, 0.9f));
    m_program->set_uniform("material.shininess", 180.f);
    m_meshes->at(m_animation_state)->render();
}

void Character::set_texture(Texture* tex)
{
    m_tex = tex;
}

Character::~Character()
{
    delete m_program;
}

}
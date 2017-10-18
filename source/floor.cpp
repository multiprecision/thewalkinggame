// floor.cpp

#define _USE_MATH_DEFINES
#include <cmath>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "scene.h"
#include "floor.h"

namespace oglproj
{

Floor::Floor(Scene* parent, float xsize, float zsize, int xdivs, int zdivs, float smax, float tmax, Texture* tex)
    : m_tex(tex)
    , m_parent(parent)
{
    // Create plane object for the floor
    m_plane = new Plane(xsize, zsize, xdivs, zdivs, smax, tmax);
    m_program = new Shader_program();
    m_program->compile_shader("../shaders/phong.vs.txt", GL_VERTEX_SHADER);
    m_program->compile_shader("../shaders/phong.fs.txt", GL_FRAGMENT_SHADER);
    m_program->link();
    // xz plane to xy plane
    m_model_matrix = glm::rotate((float)M_PI_2, glm::vec3(1, 0, 0));
}

Floor::~Floor()
{
    delete m_plane;
    delete m_program;
}


void Floor::update(double)
{
    m_model_view_matrix = m_parent->get_camera()->get_view_matrix() *
                          m_model_matrix;
    m_normal_matrix = glm::mat3(glm::vec3(m_model_view_matrix[0]),
                                glm::vec3(m_model_view_matrix[1]),
                                glm::vec3(m_model_view_matrix[2]));
}

void Floor::render()
{
    m_program->use();
    m_tex->bind(GL_TEXTURE0);
    m_program->set_uniform("mvp_matrix",
                           m_parent->get_camera()->get_projection_matrix() *
                           m_model_view_matrix);
    m_program->set_uniform("normal_matrix", m_normal_matrix);
    m_program->set_uniform("model_view_matrix", m_model_view_matrix);
    m_program->set_uniform("light.position", m_parent->get_light()->position);
    m_program->set_uniform("light.intensity", m_parent->get_light()->intensity);
    m_program->set_uniform("material.ambient", glm::vec3(0.41176f, 0.30196f, 0.20392f));
    m_program->set_uniform("material.diffuse", glm::vec3(0.68235f, 0.59215f, 0.29019f));
    m_program->set_uniform("material.specular", glm::vec3(0.73725f, 0.59215f, 0.47058f));
    m_program->set_uniform("material.shininess", 255.f);
    m_plane->render();
}

} // end of oglproj namespace
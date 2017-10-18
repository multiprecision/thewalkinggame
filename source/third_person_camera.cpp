// third_person_camera.cpp

#define _USE_MATH_DEFINES
#include <cmath>
#include "third_person_camera.h"

namespace oglproj
{

Third_person_camera::Third_person_camera(
    float yaw_angle,
    float pitch_angle,
    float radius,
    glm::vec3 target,
    float fov,
    float aspect_ratio,
    float near_z,
    float far_z)
    : Camera(fov, aspect_ratio, near_z, far_z)
    , m_yaw_angle(yaw_angle)
    , m_pitch_angle(pitch_angle)
    , m_radius(radius)
    , m_target(target)
{
    update();
}


void Third_person_camera::set_radius(float radius)
{
    m_radius = radius;
    update();
}

void Third_person_camera::set_yaw_angle(float yaw_angle)
{
    m_yaw_angle = yaw_angle;
    while (m_yaw_angle >= (float)M_PI * 2)
        m_yaw_angle -= (float)M_PI * 2;
    while (m_yaw_angle <= (float)-M_PI * 2)
        m_yaw_angle += (float)M_PI * 2;
    update();
}

void Third_person_camera::set_pitch_angle(float pitch_angle)
{
    m_pitch_angle = pitch_angle;
    if (m_pitch_angle < 0) m_pitch_angle = 0;
    else if (m_pitch_angle >= (float)M_PI_2) m_pitch_angle = (float)M_PI_2 - 0.001f;
    update();
}

void Third_person_camera::set_target(glm::vec3 target)
{
    m_target = target;
    update();
}


void Third_person_camera::add_radius(float radius)
{
    m_radius += radius;
    update();
}

void Third_person_camera::add_yaw_angle(float yaw_angle)
{
    m_yaw_angle += yaw_angle;
    while (m_yaw_angle >= (float)M_PI * 2)
        m_yaw_angle -= (float)M_PI * 2;
    while (m_yaw_angle <= (float)-M_PI * 2)
        m_yaw_angle += (float)M_PI * 2;
    update();
}

void Third_person_camera::add_pitch_angle(float pitch_angle)
{
    m_pitch_angle += pitch_angle;
    if (m_pitch_angle < 0) m_pitch_angle = 0;
    else if (m_pitch_angle >= (float)M_PI_2) m_pitch_angle = (float)M_PI_2 - 0.001f;
    update();
}

void Third_person_camera::add_target(glm::vec3 target)
{
    m_target += target;
    update();
}

float Third_person_camera::get_radius()
{
    return m_radius;
}

float Third_person_camera::get_yaw_angle()
{
    return m_yaw_angle;
}

float Third_person_camera::get_pitch_angle()
{
    return m_pitch_angle;
}

glm::vec3 Third_person_camera::get_target()
{
    return m_target;
}

void Third_person_camera::update()
{
    glm::vec3 cam_pos = glm::vec3(m_radius, 0, 0);
    glm::quat rot = glm::angleAxis(m_yaw_angle, glm::vec3(0, 0, 1));
    rot *= glm::angleAxis(m_pitch_angle, glm::vec3(0, -1, 0));
    cam_pos = rot * cam_pos + m_target;
    look_at(cam_pos, m_target, glm::vec3(0, 0, 1));
}

} // end of oglproj namespace
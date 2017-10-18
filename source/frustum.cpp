// frustum.cpp

#include "frustum.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

namespace oglproj
{

Frustum::Frustum()
    : m_fovy(0.0f)
    , m_aspect_ratio(0.0f)
    , m_z_near(0.0f)
    , m_z_far(0.0f)
    , m_is_perspective(false)
    , m_recalc_perspective_matrix(false)
{
}

/**
* Constructs an orthographic projection view \c Frustum.
*
* @note If zNear or z_far are negative, corresponding z clipping planes
* are considered behind the viewer.
*
* @param left - specify location of left clipping plane.
* @param right - specify location of right clipping plane.
* @param bottom - specify location of bottom clipping plane.
* @param top - specify location of top clipping plane.
* @param z_ear - distance to near z clipping plane.
* @param z_far - distance to far z clipping plane.
*/

Frustum::Frustum(float left, float right, float bottom, float top, float z_near, float z_far)
    : m_fovy(0.0f)
    , m_aspect_ratio((right - left) / (top - bottom))
    , m_z_near(z_near)
    , m_z_far(z_far)
    , m_is_perspective(false)
    , m_recalc_perspective_matrix(false)
{
    m_projection_matrix = glm::ortho(left, right, bottom, top, z_near, z_far);
}

/**
* Constructs a perspective projection view \c Frustum.
*
* @note  Depth buffer precision is affected by the values specified for
* z_near and z_far. The greater the ratio of z_far to z_near is, the less
* effective the depth buffer will be at distinguishing between surfaces
* that are near each other. If  r = z_far z_near roughly log2(r) bits
* of depth buffer precision are lost. Because r approaches infinity as
* z_near approaches 0, z_near must never be set to 0.
*
* @param field_of_view_y
* @param aspect_ratio
* @param z_near - distance to near z clipping plane (always positive).
* @param z_far - distance to far z clipping plane (always positive).
*/
Frustum::Frustum(float field_of_view_y, float aspect_ratio, float z_near, float z_far)
    : m_fovy(field_of_view_y)
    , m_aspect_ratio(aspect_ratio)
    , m_z_near(z_near)
    , m_z_far(z_far)
    , m_is_perspective(true)
    , m_recalc_perspective_matrix(false)
{
    m_projection_matrix = glm::perspective(field_of_view_y, aspect_ratio, z_near, z_far);
}


Frustum::~Frustum()
{
}

glm::mat4 Frustum::get_projection_matrix() const
{
    if (m_recalc_perspective_matrix)
    {
        m_projection_matrix = glm::perspective(m_fovy * 0.0174532925f, m_aspect_ratio, m_z_near, m_z_far);
        m_recalc_perspective_matrix = false;
    }
    return m_projection_matrix;
}

float Frustum::get_field_of_view_y() const
{
    return m_fovy;
}

float Frustum::get_aspect_ratio() const
{
    return m_aspect_ratio;
}

float Frustum::get_near_z_distance() const
{
    return m_z_near;
}

float Frustum::get_far_z_distance() const
{
    return m_z_far;
}

bool Frustum::is_perspective() const
{
    return m_is_perspective;
}

bool Frustum::is_orthographic() const
{
    return !m_is_perspective;
}

// Sets the Frustum field of view angle.
//
// @note If parameter field_of_view_y is negative, the field_of_view_y for the \c
// Frustum is set to zero.
//
// @param field_of_view_y
void Frustum::set_field_of_view_y(float field_of_view_y)
{
    if (field_of_view_y < 0.0f)
    {
        field_of_view_y = 0.0f;
    }
    else if (field_of_view_y > 180.0f)
    {
        field_of_view_y = 180.0f;
    }
    m_fovy = field_of_view_y;
    m_recalc_perspective_matrix = true;
}

void Frustum::set_aspect_ratio(float aspect_ratio)
{
    m_aspect_ratio = aspect_ratio;
    m_recalc_perspective_matrix = true;
}

void Frustum::set_near_z_distance(float z_near)
{
    m_z_near = z_near;
    m_recalc_perspective_matrix = true;
}

void Frustum::set_far_z_distance(float z_far)
{
    m_z_far = z_far;
    m_recalc_perspective_matrix = true;
}

void Frustum::set_projection_matrix(const glm::mat4 & projection_matrix)
{
    m_projection_matrix = projection_matrix;
}

}
// camera.cpp

#include "camera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace oglproj
{

// Default Constructor
Camera::Camera()
    : m_recalc_view_matrix(true)
    , m_rotation_hit_count(0)
{

    init_local_coordinate_system();
}

// Constructs an orthographic projected Camera.
//
// @note If z_near or z_far are negative, corresponding z clipping planes
// are considered behind the viewer.
//
// @param left - specify location of left clipping plane.
// @param right - specify location of right clipping plane.
// @param bottom - specify location of bottom clipping plane.
// @param top - specify location of top clipping plane.
// @param z_near - distance to near z clipping plane.
// @param z_far - distance to far z clipping plane.
Camera::Camera(float left, float right, float bottom, float top, float z_near, float z_far)
    : Frustum(left, right, bottom, top, z_near, z_far)
    , m_rotation_hit_count(0)
{
    init_local_coordinate_system();
}

// Constructs a perspective projected Camera.
//
// @note  Depth buffer precision is affected by the values specified for
// z_near and z_far. The greater the ratio of z_far to z_near is, the less
// effective the depth buffer will be at distinguishing between surfaces
// that are near each other. If  r = z_far z_near roughly log2(r) bits
// of depth buffer precision are lost. Because r approaches infinity as
// z_near approaches 0, z_near must never be set to 0.
//
// @param field_of_view_y
// @param aspect_ratio
// @param z_near - distance to near z clipping plane (always positive).
// @param z_far - distance to far z clipping plane (always positive).
Camera::Camera(float field_of_view_y, float aspect_ratio, float z_near, float z_far)
    : Frustum(field_of_view_y, aspect_ratio, z_near, z_far)
    , m_recalc_view_matrix(true)
    , m_rotation_hit_count(0)
{
    init_local_coordinate_system();
}

void Camera::init_local_coordinate_system()
{
    m_left.x = -1.0f;
    m_left.y = 0.0f;
    m_left.z = 0.0f;

    m_up.x = 0.0f;
    m_up.y = 1.0f;
    m_up.z = 0.0f;

    m_forward.x = 0.0f;
    m_forward.y = 0.0f;
    m_forward.z = -1.0f;

    m_eye_position.x = 0.0f;
    m_eye_position.y = 0.0f;
    m_eye_position.z = 0.0f;
}

// Sets the world position of the Camera.
// @param x
// @param y
// @param z
void Camera::set_position(float x, float y, float z)
{
    m_eye_position.x = x;
    m_eye_position.y = y;
    m_eye_position.z = z;

    m_recalc_view_matrix = true;
}

// Sets the world position of the Camera.
// @param v
void Camera::set_position(const glm::vec3& v)
{
    set_position(v.x, v.y, v.z);
}

// @return the world position of the Camera.
glm::vec3 Camera::get_position() const
{
    return m_eye_position;
}


// @return the Camera's left direction vector given in world space coordinates.
glm::vec3 Camera::get_left_direction() const
{
    return m_left;
}

// @return the Camera's up direction vector given in world space coordinates.
glm::vec3 Camera::get_up_direction() const
{
    return m_up;
}

// @return the Camera's forward direction vector given in world space coordinates.
glm::vec3 Camera::get_forward_direction() const
{
    return m_forward;
}

// @note Orientation of the Camera is given by a float glm::quaternion of the form
// (cos(theta/2), sin(theta/2) * u), where the axis of rotation u is given in
// world space coordinates.
//
// @return the Camera's orientation in the form of a float glm::quaternion.
glm::quat Camera::get_orientation() const
{
    return m_orientation;
}

// @return a 4x4 view matrix representing the 'Camera' object's view transformation.
glm::mat4 Camera::get_view_matrix() const
{
    if (m_recalc_view_matrix)
    {
        // Compute inverse rotation q
        glm::quat q = m_orientation;
        q.x *= -1.0f;
        q.y *= -1.0f;
        q.z *= -1.0f;
        m_view_matrix = glm::mat4_cast(q);

        // Translate by inverse eyePosition.
        glm::vec3 v = -m_eye_position;
        glm::mat4 m = m_view_matrix;
        m_view_matrix[3] = (m[0] * v[0]) + (m[1] * v[1]) + (m[2] * v[2]) + m[3];

        m_recalc_view_matrix = false;
    }

    return m_view_matrix;
}

void Camera::normalize_camera()
{
    m_left = glm::normalize(m_left);
    m_up = glm::normalize(m_up);
    m_forward = glm::normalize(m_forward);
    m_orientation = glm::normalize(m_orientation);

    // Assuming forward 'f' is correct
    m_left = glm::cross(m_up, m_forward);
    m_up = glm::cross(m_forward, m_left);
}

void Camera::register_rotation()
{
    m_rotation_hit_count++;

    if (m_rotation_hit_count > c_rotation_hit_count_max)
    {
        m_rotation_hit_count = 0;
        normalize_camera();
    }
}

// Rotates Camera about its local negative z-axis (forward direction)
// by angle radians.
//
// @note Rotation is counter-clockwise if angle > 0, and clockwise if
// angle is < 0.
//
// @param angle - rotation angle in radians.
void Camera::roll(float angle)
{
    glm::quat q = glm::angleAxis(angle, m_forward);

    m_up = glm::rotate(q, m_up);
    m_left = glm::rotate(q, m_left);

    m_orientation = q * m_orientation;

    register_rotation();
    m_recalc_view_matrix = true;
}

// Rotates Camera about its local x (right direction) axis by angle
// radians.
//
// @note Rotation is counter-clockwise if angle > 0, and clockwise if
// angle is < 0.
//
// @param angle - rotation angle in radians.
void Camera::pitch(float angle)
{
    glm::quat q = glm::angleAxis(angle, -m_left);

    m_up = glm::rotate(q, m_up);
    m_forward = glm::rotate(q, m_forward);

    m_orientation = q * m_orientation;

    register_rotation();
    m_recalc_view_matrix = true;
}

// Rotates Camera about its local y (up direction) axis by angle
// radians.
//
// @note Rotation is counter-clockwise if angle > 0, and clockwise if
// angle is < 0.
//
// @param angle - rotation angle in radians.
void Camera::yaw(float angle)
{
    glm::quat q = glm::angleAxis(angle, m_up);

    m_left = glm::rotate(q, m_left);
    m_forward = glm::rotate(q, m_forward);

    m_orientation = q * m_orientation;

    register_rotation();
    m_recalc_view_matrix = true;
}

// Rotates Camera by angle radians about axis whose components are expressed using
// the Camera's local coordinate system.
//
// @note Counter-clockwise rotation for angle > 0, and clockwise rotation otherwise.
//
// @param angle
// @param axis
void Camera::rotate(float angle, const glm::vec3& axis)
{
    glm::vec3 n = glm::normalize(axis);
    glm::quat q = glm::angleAxis(angle, n);

    m_left = glm::rotate(q, m_left);
    m_up = glm::rotate(q, m_up);
    m_forward = glm::rotate(q, m_forward);

    m_orientation = q * m_orientation;

    register_rotation();
    m_recalc_view_matrix = true;
}

// Translates the Camera with respect to the world coordinate system.
//
// @param x
// @param y
// @param z
void Camera::translate(float x, float y, float z)
{
    m_eye_position.x += x;
    m_eye_position.y += y;
    m_eye_position.z += z;

    m_recalc_view_matrix = true;
}

// Translates the Camera with respect to the world coordinate system.
//
// @param v
void Camera::translate(const glm::vec3& v)
{
    translate(v.x, v.y, v.z);
}

// Translates the Camera relative to its local coordinate system.
//
// @param left - translation along the Camera's left direction.
// @param up - translation along the Camera's up direction.
// @param forward - translation along the Camera's forward direction.
void Camera::translate_local(float left, float up, float forward)
{
    m_eye_position += left * m_left;
    m_eye_position += up * m_up;
    m_eye_position += forward * m_forward;

    m_recalc_view_matrix = true;
}

// Translates the Camera relative to its locate coordinate system.
//
// v.x - translation along the Camera's left direction.
// v.y - translation along the Camera's up direction.
// v.z - translation along the Camera's forward direction.
//
// @param v
void Camera::translate_local(const glm::vec3 &v)
{
    translate_local(v.x, v.y, v.z);
}

void Camera::look_at(const glm::vec3 & center)
{
    look_at(center.x, center.y, center.z);
}

// A better algorithm may be to compute the transformation or quaternion
// that takes previous f to new f, then rotate u and l by it.  This will handle the
// case when new f = prior u.
void Camera::look_at(float center_x, float center_y, float center_z)
{
    // f = center - eye.
    m_forward.x = center_x - m_eye_position.x;
    m_forward.y = center_y - m_eye_position.y;
    m_forward.z = center_z - m_eye_position.z;
    m_forward = glm::normalize(m_forward);

    // The following projects u onto the plane defined by the point eye_position,
    // and the normal f. The goal is to rotate u so that it is orthogonal to f,
    // while attempting to keep u's orientation close to its previous direction.
    {
        // Borrow l vector for calculation, so we don't have to allocate a
        // new vector.
        m_left = m_eye_position + m_up;

        float t = -1.0f * glm::dot(m_forward, m_up);

        // Move point l in the normal direction, f, by t units so that it is
        // on the plane.
        m_left.x += t * m_forward.x;
        m_left.y += t * m_forward.y;
        m_left.z += t * m_forward.z;

        m_up = m_left - m_eye_position;
        m_up = glm::normalize(m_up);
    }

    // Update l vector given new f and u vectors.
    m_left = glm::cross(m_up, m_forward);

    // If f and u are no longer orthogonal, make them so.
    if (glm::dot(m_forward, m_up) > 1e-7f)
    {
        m_up = glm::cross(m_forward, m_left);
    }

    glm::mat3 m;
    m[0] = -1.0f * m_left; // Camera's local x axis
    m[1] = m_up;         // Camera's local y axis
    m[2] = -1.0f * m_forward; // Camera's local z axis
    m_orientation = glm::quat_cast(m);

    register_rotation();
    m_recalc_view_matrix = true;
}

void Camera::look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
    m_eye_position = eye;

    // Orient Camera basis vectors.
    m_forward = glm::normalize(center - eye);
    m_left = glm::normalize(glm::cross(up, m_forward));
    m_up = glm::cross(m_forward, m_left);

    // Compute orientation from 3x3 change of basis matrix whose columns are the
    // world basis vectors given in Camera space coordinates.
    glm::mat3 m;
    m[0] = -1.0f * m_left; // first column, representing new x-axis orientation
    m[1] = m_up;         // second column, representing new y-axis orientation
    m[2] = -1.0f * m_forward; // third column, representing new z-axis orientation
    m_orientation = glm::quat_cast(m);

    register_rotation();
    m_recalc_view_matrix = true;
}

} // end of namespace oglproj
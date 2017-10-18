// texture.h

#ifndef OGLPROJ_CAMERA_H_
#define OGLPROJ_CAMERA_H_

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include "frustum.h"

namespace oglproj
{

// Class for encapsulating a view frustum camera, capable of either orthographic or perspective
// projections.  Both view and projection matrices can be retrieved from a Camera object at
// any time, which reflect the current state of the Camera.
//
// Default values for a Camera object in world coordinates include:
// position, located at world coordinate origin (0, 0, 0).
// left direction, aligned with negative x-axis (-1, 0, 0).
// up direction, aligned with y-axis (0, 1, 0).
// forward direction, aligned with negative z-axis (0, 0, -1).

class Camera : public Frustum
{
public:
    Camera();
    Camera(float left, float right, float bottom, float top, float z_near, float z_far);
    Camera(float field_of_view_y, float aspect_ratio, float z_near, float z_far);
    glm::vec3 get_position() const;
    glm::vec3 get_left_direction() const;
    glm::vec3 get_up_direction() const;
    glm::vec3 get_forward_direction() const;
    glm::quat get_orientation() const;
    glm::mat4 get_view_matrix() const;
    void set_position(float x, float y, float z);
    void set_position(const glm::vec3& v);
    void look_at(const glm::vec3& center);
    void look_at(float center_x, float center_y, float center_z);
    void look_at(const glm::vec3& eye,
                 const glm::vec3& center,
                 const glm::vec3& up);
    void roll(float angle);
    void pitch(float angle);
    void yaw(float angle);
    void rotate(float angle, const glm::vec3& axis);
    void translate(float x, float y, float z);
    void translate(const glm::vec3& v);
    void translate_local(float left, float up, float forward);
    void translate_local(const glm::vec3 &v);

private:
    glm::vec3 m_eye_position; // Location of camera in world coordinates.
    glm::quat m_orientation; // Orientation of camera basis vectors specified in world coordinates.

    glm::vec3 m_left; // Camera's left direction vector, given in world coordinates.
    glm::vec3 m_up; // Camera's up direction vector, given in world coordinates.
    glm::vec3 m_forward; // Camera's forward direction vector, given in world coordinates.

    mutable bool m_recalc_view_matrix;
    mutable glm::mat4 m_view_matrix;

    unsigned short m_rotation_hit_count;

    // Normalize Camera vectors after rotating this many times.
    static const unsigned c_rotation_hit_count_max = 1000;

    void init_local_coordinate_system();
    void register_rotation();
    void normalize_camera();

};

} // end of namespace oglproj

#endif
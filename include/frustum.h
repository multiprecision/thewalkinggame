// frustum.h

#ifndef OGLPROJ_FRUSTUM_H_
#define OGLPROJ_FRUSTUM_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace oglproj
{

class Frustum
{
public:
    Frustum();
    Frustum(float left, float right, float bottom, float top, float z_near, float z_far);
    Frustum(float field_of_view_y, float aspect_ratio, float z_near, float z_far);
    ~Frustum();
    glm::mat4 get_projection_matrix() const;
    float get_field_of_view_y() const;
    float get_aspect_ratio() const;
    float get_near_z_distance() const;
    float get_far_z_distance() const;
    void set_field_of_view_y(float field_of_view_y);
    void set_aspect_ratio(float aspect_ratio);
    void set_near_z_distance(float z_near);
    void set_far_z_distance(float z_far);
    void set_projection_matrix(const glm::mat4& projection_matrix);

    bool is_perspective() const;
    bool is_orthographic() const;
private:
    float m_fovy;
    float m_aspect_ratio;
    float m_z_near; // Distance to near z plane.
    float m_z_far;  // Distance to far z plane.
    bool m_is_perspective;
    mutable bool m_recalc_perspective_matrix;

    mutable glm::mat4 m_projection_matrix;

    const static unsigned c_float_tolerance = 5;
};

}

#endif
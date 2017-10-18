// third_person_camera.h

#ifndef OGLPROJ_THIRD_PERSON_CAMERA_H_
#define OGLPROJ_THIRD_PERSON_CAMERA_H_

#include "camera.h"

namespace oglproj
{

class Third_person_camera : public Camera
{
public:
    Third_person_camera(
        float yaw_angle,
        float pitch_angle,
        float radius,
        glm::vec3 m_target,
        float fov,
        float aspect_ratio,
        float near_z,
        float far_z);

    void set_radius(float radius);
    void set_yaw_angle(float yaw_angle);
    void set_pitch_angle(float pitch_angle);
    void set_target(glm::vec3 target);
    void add_radius(float radius);
    void add_yaw_angle(float yaw_angle);
    void add_pitch_angle(float pitch_angle);
    void add_target(glm::vec3 target);
    float get_radius();
    float get_yaw_angle();
    float get_pitch_angle();
    glm::vec3 get_target();
protected:
    void update();
    float m_radius;
    float m_yaw_angle;
    float m_pitch_angle;
    glm::vec3 m_target;
};

}

#endif
// scene.h

#ifndef OGLPROJ_SCENE_H_
#define OGLPROJ_SCENE_H_
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include "renderable.h"
#include "third_person_camera.h"

namespace oglproj
{


struct Light
{
    glm::vec4 position;
    glm::vec3 intensity;
};

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class Scene : public Renderable
{
public:
    Scene(GLFWwindow* parent) :
        m_parent(parent)
    {
    };
    GLFWwindow* get_parent()
    {
        return m_parent;
    }
    // Load textures, initialize shaders, etc.
    virtual void init_scene() = 0;
    // This is called prior to every frame.  Use this
    // to update animation.
    virtual void update(double t) = 0;
    // Draw scene.
    virtual void render() = 0;
    // Camera
    virtual Third_person_camera* get_camera() = 0;
    virtual Light* get_light() = 0;
protected:
    GLFWwindow* m_parent;
};

} // end of namespace oglproj

#endif
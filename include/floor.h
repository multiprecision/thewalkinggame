// floor.h

#ifndef OGLPROJ_FLOOR_H_
#define OGLPROJ_FLOOR_H_

#include "plane.h"
#include "shader_program.h"
#include "scene.h"
#include "texture.h"

namespace oglproj
{

class Floor : public Renderable
{
public:
    Floor(Scene* parent, float xsize, float zsize, int xdivs, int zdivs, float smax, float tmax, Texture* tex);
    virtual ~Floor();
    void update(double t);
    virtual void render();
protected:
    // Program for the floor.
    Shader_program* m_program;
    // For the floor
    Plane* m_plane;
    Texture* m_tex;
    glm::mat4 m_model_matrix;
    glm::mat4 m_model_view_matrix;
    glm::mat3 m_normal_matrix;
    glm::mat3 m_mvp_matrix;
    Scene* m_parent;
};

} //end of oglproj namespace

#endif
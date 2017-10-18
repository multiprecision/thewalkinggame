// plane.h

#ifndef OGLPROJ_PLANE_H_
#define OGLPROJ_PLANE_H_

#include "renderable.h"
#include "GL/glew.h"

namespace oglproj
{

// An OpenGL-based plane
class Plane : public Renderable
{
public:
    Plane(float xsize, float zsize, int xdivs, int zdivs, float smax = 1.0f, float tmax = 1.0f);
    ~Plane();
    void render() override;
private:
    GLuint m_buffer_handle[4];
    GLuint m_vao_handle;
    int m_faces;
};

} // end of namespace oglproj

#endif
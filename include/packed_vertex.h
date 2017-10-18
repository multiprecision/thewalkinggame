// packed_vertex.h

#ifndef OGLPROJ_PACKED_VERTEX_H_
#define OGLPROJ_PACKED_VERTEX_H_

#include <glm/glm.hpp>

namespace oglproj
{

// Stores vertex position, uv, and normal (triangle).
struct Packed_vertex
{
    Packed_vertex(glm::vec3& p_position, glm::vec2& p_uv, glm::vec3& p_normal)
    {
        position = p_position;
        uv = p_uv;
        normal = p_normal;
    }
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
};

bool is_equal(float v1, float v2, float margin);

inline bool operator<(const Packed_vertex & a, const Packed_vertex & b)
{
    if (!is_equal(a.position.x, b.position.x, 0.00001f)) return a.position.x < b.position.x;
    if (!is_equal(a.position.y, b.position.y, 0.00001f)) return a.position.y < b.position.y;
    if (!is_equal(a.position.z, b.position.z, 0.00001f)) return a.position.z < b.position.z;
    if (!is_equal(a.uv.x, b.uv.x, 0.00001f)) return a.uv.x       < b.uv.x;
    if (!is_equal(a.uv.y, b.uv.y, 0.00001f)) return a.uv.y       < b.uv.y;
    if (!is_equal(a.normal.x, b.normal.x, 0.00001f)) return a.normal.x   < b.normal.x;
    if (!is_equal(a.normal.y, b.normal.y, 0.00001f)) return a.normal.y   < b.normal.y;
    if (!is_equal(a.normal.z, b.normal.z, 0.00001f)) return a.normal.z   < b.normal.z;
    return false; // Vertices are equal
};

} // end of namespace oglproj

#endif

// mesh.h
//
// Copyright (c) 2014 Samuel I. Gunadi <samuel.i.gunadi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OGLPROJ_MESH_H_
#define OGLPROJ_MESH_H_

#include <vector>
#include <string>
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "renderable.h"

namespace oglproj
{

// A OpenGL-based mesh with a limited OBJ parser
class Mesh : public Renderable
{
public:
    Mesh(bool recenter = false);
    ~Mesh();
    void render() override;
    void import_obj_file(const std::string& file_name);
    void import_bin_file(const std::string& file_name);
    void store_vbo(bool save_bin);
    void print_info();
private:
    void trim_string(std::string& str);
    void center(std::vector<glm::vec3>&);
    GLuint m_vao_handle;
    bool m_recenter_mesh;
    float* m_vertex_data;
    float* m_normal_data;
    float* m_uv_data;
    float* m_tangent_data;
    GLuint* m_index_data;
    std::string m_name;
    size_t m_num_vertices;
    size_t m_num_normals;
    size_t m_num_uvs;
    size_t m_num_tangents;
    size_t m_num_indices;
    GLuint* m_buffer_handle;
    int m_buffer_num;
};

} // end of namespace oglproj

#endif
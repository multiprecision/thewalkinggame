// mesh.cpp

#include <gl/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdint>
#include <thread>
#include <gl/glew.h>

#include "packed_vertex.h"
#include "mesh.h"

namespace oglproj
{

bool is_equal(float v1, float v2, float margin)
{
    return std::fabs(v1 - v2) < margin;
}

bool get_similar_vertex_index(Packed_vertex& packed, std::map<Packed_vertex,
                              GLuint>& vertex_to_out_index, GLuint& result)
{
    std::map<Packed_vertex, GLuint>::iterator it = vertex_to_out_index.find(packed);
    if (it == vertex_to_out_index.end())
    {
        return false;
    }
    else
    {
        result = it->second;
        return true;
    }
}

Mesh::Mesh(bool recenter)
    : m_recenter_mesh(recenter)
    , m_vao_handle(0)
{
}

Mesh::~Mesh()
{
    glDeleteBuffers(m_buffer_num, m_buffer_handle);
    glDeleteVertexArrays(1, &m_vao_handle);
}

void Mesh::render()
{
    if (!m_vao_handle)
    {
        std::printf("[ERROR] VAO has not been generated.\n");
        throw std::runtime_error("VAO is 0");
    }
    glBindVertexArray(m_vao_handle);
    glDrawElements(GL_TRIANGLES, 3 * (GLsizei) m_num_indices / 3, GL_UNSIGNED_INT, ((GLubyte *)nullptr + (0)));
}

// OBJ File Format
//
// # This is a comment.
//
// # List of Vertices, with(x, y, z[, w]) coordinates, w is optional
// # and defaults to 1.0.
// v 0.123 0.234 0.345 1.0
// v ...
//
// # Texture coordinates, in(u, v[, w]) coordinates, these will vary
// # between 0 and 1, w is optional and defaults to 0.
// vt 0.500 1[0]
// vt ...
//
// # Normals in(x, y, z) form; normals might not be unit.
// vn 0.707 0.000 0.707
// vn ...
//
// # Parameter space vertices in(u[, v][, w]) form; free form geometry
// # statement)
// vp 0.310000 3.210000 2.100000
// vp ...
//
// # Face Definitions
// f 1 2 3
// f 3 / 1 4 / 2 5 / 3
// f 6 / 4 / 1 3 / 5 / 3 7 / 6 / 5
// f ...

// Mesh::importObjFile
// @brief Imports data from obj file.
// @param file_name path to the file.
void Mesh::import_obj_file(const std::string& file_name)
{
    std::vector<glm::vec3> in_vertices;
    std::vector<glm::vec3> in_normals;
    std::vector<glm::vec2> in_uvs;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_uvs;
    std::vector<GLuint> vertex_indices;
    std::vector<GLuint> uv_indices;
    std::vector<GLuint> normal_indices;

    m_name = file_name;
    int n_faces = 0;

    std::ifstream obj_stream(file_name, std::ios::in);

    if (!obj_stream)
    {
        std::printf("[ERROR] Unable to open OBJ file \"%s\".\n", file_name);
        return;
    }

    std::string line, token;

    getline(obj_stream, line);
    while (!obj_stream.eof())
    {
        trim_string(line);
        if (line.length() > 0 && line.at(0) != '#')
        {
            std::istringstream line_stream(line);

            line_stream >> token;

            if (token == "v")
            {
                float x, y, z;
                line_stream >> x >> y >> z;
                temp_vertices.push_back(glm::vec3(x, y, z));
            }
            else if (token == "vt")
            {
                // Process texture coordinate
                float s, t;
                line_stream >> s >> t;
                temp_uvs.push_back(glm::vec2(s, t));
            }
            else if (token == "vn")
            {
                float x, y, z;
                line_stream >> x >> y >> z;
                temp_normals.push_back(glm::vec3(x, y, z));
            }
            else if (token == "f")
            {
                n_faces++;
                int num = 0;
                size_t slash1, slash2;
                while (line_stream.good())
                {
                    std::string vert_string;
                    line_stream >> vert_string;
                    int p_index = -1, n_index = -1, t_c_index = -1;

                    slash1 = vert_string.find("/");
                    if (slash1 == std::string::npos)
                    {
                        p_index = atoi(vert_string.c_str());
                    }
                    else
                    {
                        slash2 = vert_string.find("/", slash1 + 1);
                        p_index = atoi(vert_string.substr(0, slash1).c_str());
                        if (slash2 > slash1 + 1)
                        {
                            t_c_index =
                                atoi(vert_string.substr(slash1 + 1, slash2).c_str());
                        }
                        n_index =
                            atoi(vert_string.substr(slash2 + 1, vert_string.length()).c_str());
                    }
                    //if (static_cast<GLuint>(p_index) >= temp_vertices.size() + 1)
                    //    std::cout << "[ERROR] Vertex index out of bound: " << p_index << std::endl;

                    //if (static_cast<GLuint>(n_index) >= temp_normals.size() + 1)
                    //    std::cout << "[ERROR] Normal index out of bound: " << n_index << std::endl;
                    if (p_index == -1)
                    {
                        std::printf("[ERROR] Missing point index.\n");
                    }
                    else
                    {
                        vertex_indices.push_back(p_index);
                        uv_indices.push_back(t_c_index);
                        normal_indices.push_back(n_index);
                        num++;
                    }

                }
                // If number of edges in face is greater than 3,
                // decompose into triangles as a triangle fan.
                if (num > 3)
                {
                    // Triangles only :(
                    std::printf("[ERROR] OBJ Loader: This operation is not supported. "
                                "Triangulate the mesh first.\n");
                    return;
                }

            }
        }
        getline(obj_stream, line);
    }

    obj_stream.close();

    // For each triangle
    for (size_t idx = 0; idx < vertex_indices.size(); idx += 3)
    {
        // For each vertex of the triangle
        for (size_t i = 0; i < 3; i += 1)
        {
            unsigned int vertex_index = vertex_indices[idx + i];
            glm::vec3 vertex = temp_vertices[vertex_index - 1];

            unsigned int uv_index = uv_indices[idx + i];
            glm::vec2 uv = temp_uvs[uv_index - 1];

            unsigned int normal_index = normal_indices[idx + i];
            glm::vec3 normal = temp_normals[normal_index - 1];

            in_vertices.push_back(vertex);
            in_uvs.push_back(uv);
            in_normals.push_back(normal);
        }
    }

    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec3> out_normals;
    std::vector<glm::vec2> out_uv;
    std::vector<glm::vec4> m_out_tangents;
    std::vector<GLuint> out_indices;

    // Reserve some space in the vectors to avoid mallocs
    out_indices.reserve(in_vertices.size());
    out_vertices.reserve(in_vertices.size());
    out_uv.reserve(in_vertices.size());
    out_normals.reserve(in_vertices.size());

    std::map<Packed_vertex, GLuint> vertex_to_out_index;

    // For each input vertex
    for (unsigned int i = 0; i < in_vertices.size(); i++)
    {
        Packed_vertex packed(in_vertices[i], in_uvs[i], in_normals[i]);

        // Try to find a similar vertex in out_XXXX
        GLuint index;
        bool found = get_similar_vertex_index(packed, vertex_to_out_index, index);

        if (found)
        {
            // A similar vertex is already in the VBO, use it instead
            out_indices.push_back(index);
            // Optional : mix the new and the former normals, tangents & bitangents
            out_normals[index] += in_normals[i];
        }
        else
        {
            // If not, it needs to be added in the output data.
            out_vertices.push_back(in_vertices[i]);
            out_uv.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            GLuint newindex = (GLuint) out_vertices.size() - 1;
            out_indices.push_back(newindex);
            vertex_to_out_index[packed] = newindex;
        }
    }

    // Re-normalize the normals, tangents & bitangents
    for (unsigned int i = 0; i < out_normals.size(); i++)
    {
        out_normals[i] = glm::normalize(out_normals[i]);
    }

    if (m_recenter_mesh)
    {
        center(temp_vertices);
    }

    m_num_vertices = out_vertices.size();
    m_num_normals = out_normals.size();
    m_num_uvs = out_uv.size();
    m_num_tangents = m_out_tangents.size();
    m_num_indices = out_indices.size();

    m_vertex_data = new float[3 * m_num_vertices];
    m_normal_data = new float[3 * m_num_vertices];
    m_uv_data = new float[2 * m_num_vertices];
    m_tangent_data = nullptr;
    if (m_num_tangents > 0)
        m_tangent_data = new float[4 * m_num_vertices];

    m_index_data = new GLuint[m_num_indices];

    int idx = 0, uv_idx = 0, tan_idx = 0;
    for (size_t i = 0; i < m_num_vertices; i++)
    {
        m_vertex_data[idx] = out_vertices[i].x;
        m_vertex_data[idx + 1] = out_vertices[i].y;
        m_vertex_data[idx + 2] = out_vertices[i].z;
        m_normal_data[idx] = out_normals[i].x;
        m_normal_data[idx + 1] = out_normals[i].y;
        m_normal_data[idx + 2] = out_normals[i].z;
        idx += 3;
        m_uv_data[uv_idx] = out_uv[i].x;
        m_uv_data[uv_idx + 1] = out_uv[i].y;
        uv_idx += 2;
        if (m_tangent_data != nullptr)
        {
            m_tangent_data[tan_idx] = m_out_tangents[i].x;
            m_tangent_data[tan_idx + 1] = m_out_tangents[i].y;
            m_tangent_data[tan_idx + 2] = m_out_tangents[i].z;
            m_tangent_data[tan_idx + 3] = m_out_tangents[i].w;
            tan_idx += 4;
        }
    }
    for (unsigned int i = 0; i < m_num_indices; ++i)
    {
        m_index_data[i] = out_indices[i];
    }

    std::stringstream ss;
    ss << "[INFO][THREAD " << std::this_thread::get_id() << "] Imported mesh from: " << file_name << "." << std::endl;
    std::cout << ss.str();
}

// Mesh::center
// @brief recenter the mesh to origin
void Mesh::center(std::vector<glm::vec3> & vertices)
{
    if (vertices.size() < 1) return;

    glm::vec3 max_point = vertices[0];
    glm::vec3 min_point = vertices[0];

    // Find the AABB
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        glm::vec3 & point = vertices[i];
        if (point.x > max_point.x) max_point.x = point.x;
        if (point.y > max_point.y) max_point.y = point.y;
        if (point.z > max_point.z) max_point.z = point.z;
        if (point.x < min_point.x) min_point.x = point.x;
        if (point.y < min_point.y) min_point.y = point.y;
        if (point.z < min_point.z) min_point.z = point.z;
    }

    // Center of the AABB
    glm::vec3 center = glm::vec3((max_point.x + min_point.x) / 2.0f,
                                 (max_point.y + min_point.y) / 2.0f,
                                 (max_point.z + min_point.z) / 2.0f);

    // Translate center of the AABB to the origin
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        glm::vec3 & point = vertices[i];
        point = point - center;
    }
}

// Mesh::importBinFile
// @brief Imports binary VBO data to speed up since parsing large OBJ
// file takes too long.
// file format:
// (header) vertex position number, vertex normal number, vertex uv number,
// vertex tangent number, indices number
// (body) vertex position data, vertex normal data, vertex uv data,
// vertex tangent data, indices number data
void Mesh::import_bin_file(const std::string& file_name)
{
    m_name = file_name;

    std::ifstream bin_stream(file_name, std::ios::in | std::ios::binary);

    if (!bin_stream)
    {
        std::printf("[ERROR] Unable to open BIN file \"%s\".\n", file_name);
        return;
    }

    uint32_t v_num = 0;
    uint32_t n_num = 0;
    uint32_t uv_num = 0;
    uint32_t tan_num = 0;
    uint32_t index_num = 0;
    bin_stream.read(reinterpret_cast<char *>(&v_num), sizeof(uint32_t));
    bin_stream.read(reinterpret_cast<char *>(&n_num), sizeof(uint32_t));
    bin_stream.read(reinterpret_cast<char *>(&uv_num), sizeof(uint32_t));
    bin_stream.read(reinterpret_cast<char *>(&tan_num), sizeof(uint32_t));
    bin_stream.read(reinterpret_cast<char *>(&index_num), sizeof(uint32_t));
    m_vertex_data = new float[v_num];
    m_normal_data = new float[n_num];
    m_uv_data = new float[uv_num];
    m_tangent_data = new float[tan_num];
    m_index_data = new uint32_t[index_num];
    bin_stream.read(reinterpret_cast<char *>(m_vertex_data), sizeof(float) * v_num);
    bin_stream.read(reinterpret_cast<char *>(m_normal_data), sizeof(float) * n_num);
    bin_stream.read(reinterpret_cast<char *>(m_uv_data), sizeof(float) * uv_num);
    bin_stream.read(reinterpret_cast<char *>(m_tangent_data), sizeof(float) * tan_num);
    bin_stream.read(reinterpret_cast<char *>(m_index_data), sizeof(uint32_t) * index_num);
    bin_stream.close();
    m_num_vertices = v_num / 3;
    m_num_normals = n_num / 3;
    m_num_uvs = uv_num / 2;
    m_num_tangents = tan_num / 4;
    m_num_indices = index_num;
    std::stringstream ss;
    ss << "[INFO][THREAD " << std::this_thread::get_id() << "] Loaded mesh from: " << file_name << "." << std::endl;
    std::cout << ss.str();
}

// Mesh::store_vbo
// @brief Store the vertex data in VBO.
// @save_bin save the VBO data in binary format
void Mesh::store_vbo(bool save_bin)
{
    glGenVertexArrays(1, &m_vao_handle);
    glBindVertexArray(m_vao_handle);

    m_buffer_num = 4;
    if (m_num_tangents > 0) m_buffer_num++;
    GLuint element_buffer = m_buffer_num - 1;

    GLuint buf_index = 0;
    m_buffer_handle = new GLuint[m_buffer_num];
    glGenBuffers(m_buffer_num, m_buffer_handle);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_handle[buf_index++]);
    glBufferData(GL_ARRAY_BUFFER, (3 * m_num_vertices) * sizeof(float), m_vertex_data, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)nullptr + (0)));
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_handle[buf_index++]);
    glBufferData(GL_ARRAY_BUFFER, (3 * m_num_vertices) * sizeof(float), m_normal_data, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)nullptr + (0)));
    glEnableVertexAttribArray(1);  // Vertex normal
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_handle[buf_index++]);
    glBufferData(GL_ARRAY_BUFFER, (2 * m_num_vertices) * sizeof(float), m_uv_data, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)nullptr + (0)));
    glEnableVertexAttribArray(2);  // Texture coords
    if (m_num_tangents > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer_handle[buf_index++]);
        glBufferData(GL_ARRAY_BUFFER, (4 * m_num_vertices) * sizeof(float), m_tangent_data, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint)3, 4, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)nullptr + (0)));
        glEnableVertexAttribArray(3);  // Tangent vector
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_handle[element_buffer]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_num_indices * sizeof(GLuint), m_index_data, GL_STATIC_DRAW);

    glBindVertexArray(0);

    if (save_bin)
    {
        std::ofstream file(m_name + ".bin", std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file)
        {
            throw std::runtime_error("[ERROR] Unable to open BIN file \"" + m_name + ".bin\".");
        }
        int v_num = (int)m_num_vertices * 3;
        int n_num = (int)m_num_normals * 3;
        int uv_num = (int)m_num_uvs * 2;
        int tan_num = (int)m_num_tangents * 4;
        int index_num = (int)m_num_indices;
        file.write(reinterpret_cast<const char *>(&v_num), sizeof(uint32_t));
        file.write(reinterpret_cast<const char *>(&n_num), sizeof(uint32_t));
        file.write(reinterpret_cast<const char *>(&uv_num), sizeof(uint32_t));
        file.write(reinterpret_cast<const char *>(&tan_num), sizeof(uint32_t));
        file.write(reinterpret_cast<const char *>(&index_num), sizeof(uint32_t));
        file.write(reinterpret_cast<const char *>(m_vertex_data), sizeof(float) * v_num);
        file.write(reinterpret_cast<const char *>(m_normal_data), sizeof(float) * n_num);
        if (m_uv_data != nullptr)
            file.write(reinterpret_cast<const char *>(m_uv_data), sizeof(float) * uv_num);
        if (m_tangent_data != nullptr)
            file.write(reinterpret_cast<const char *>(m_tangent_data), sizeof(float) * tan_num);
        file.write(reinterpret_cast<const char *>(m_index_data), sizeof(uint32_t) * index_num);
        file.close();
    }

    // Clean up

    delete[] m_vertex_data;
    delete[] m_normal_data;
    delete[] m_uv_data;
    delete[] m_tangent_data;
    delete[] m_index_data;

    std::cout << "[INFO][THREAD " << std::this_thread::get_id() << "] Loaded " << m_name << " [" << m_vao_handle << "] into GPU." << std::endl;
}

void Mesh::trim_string(std::string& str)
{
    const char * whitespace = " \t\n\r";
    size_t location;
    location = str.find_first_not_of(whitespace);
    str.erase(0, location);
    location = str.find_last_not_of(whitespace);
    str.erase(location + 1);
}

void Mesh::print_info()
{
    std::ostringstream ss;

    ss << m_name << std::endl
       << "   " << m_num_vertices << " vertices." << std::endl
       << "   " << m_num_indices / 3 << " faces." << std::endl
       << "   " << m_num_vertices / 3 << " triangles." << std::endl
       << "   " << m_num_normals << " normals." << std::endl
       << "   " << m_num_tangents << " tangents. " << std::endl
       << "   " << m_num_uvs << " texture coordinates." << std::endl;
    std::cout << ss.str();
}

} // end of oglproj namespace
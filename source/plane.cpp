// plane.cpp

#include <gl/glew.h>

#include "plane.h"

namespace oglproj
{

Plane::Plane(float xsize, float zsize, int xdivs, int zdivs, float smax, float tmax)
{
    m_faces = xdivs * zdivs;
    float * v = new float[3 * (xdivs + 1) * (zdivs + 1)];
    float * n = new float[3 * (xdivs + 1) * (zdivs + 1)];
    float * tex = new float[2 * (xdivs + 1) * (zdivs + 1)];
    unsigned int * el = new unsigned int[6 * xdivs * zdivs];

    float x2 = xsize / 2.0f;
    float z2 = zsize / 2.0f;
    float i_factor = static_cast<float>(zsize / zdivs);
    float j_factor = static_cast<float>(xsize / xdivs);
    float texi = smax / zdivs;
    float texj = tmax / xdivs;
    float x, z;
    int vidx = 0, tidx = 0;
    for (int i = 0; i <= zdivs; i++)
    {
        z = i_factor * i - z2;
        for (int j = 0; j <= xdivs; j++)
        {
            x = j_factor * j - x2;
            v[vidx] = x;
            v[vidx + 1] = 0.0f;
            v[vidx + 2] = z;
            n[vidx] = 0.0f;
            n[vidx + 1] = 1.0f;
            n[vidx + 2] = 0.0f;
            vidx += 3;
            tex[tidx] = j * texi;
            tex[tidx + 1] = i * texj;
            tidx += 2;
        }
    }

    unsigned int row_start, next_row_start;
    int idx = 0;
    for (int i = 0; i < zdivs; i++)
    {
        row_start = i * (xdivs + 1);
        next_row_start = (i + 1) * (xdivs + 1);
        for (int j = 0; j < xdivs; j++)
        {
            el[idx] = row_start + j;
            el[idx + 1] = next_row_start + j;
            el[idx + 2] = next_row_start + j + 1;
            el[idx + 3] = row_start + j;
            el[idx + 4] = next_row_start + j + 1;
            el[idx + 5] = row_start + j + 1;
            idx += 6;
        }
    }

    glGenBuffers(4, m_buffer_handle);

    glGenVertexArrays(1, &m_vao_handle);
    glBindVertexArray(m_vao_handle);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs + 1) * (zdivs + 1) * sizeof(float), v, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)nullptr + (0)));
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs + 1) * (zdivs + 1) * sizeof(float), n, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)nullptr + (0)));
    glEnableVertexAttribArray(1);  // Vertex normal

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_handle[2]);
    glBufferData(GL_ARRAY_BUFFER, 2 * (xdivs + 1) * (zdivs + 1) * sizeof(float), tex, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)nullptr + (0)));
    glEnableVertexAttribArray(2);  // Texture coords

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * xdivs * zdivs * sizeof(unsigned int), el, GL_STATIC_DRAW);

    glBindVertexArray(0);

    delete[] v;
    delete[] n;
    delete[] tex;
    delete[] el;
}

Plane::~Plane()
{
    glDeleteBuffers(4, m_buffer_handle);
    glDeleteVertexArrays(1, &m_vao_handle);
}

void Plane::render()
{
    glBindVertexArray(m_vao_handle);
    glDrawElements(GL_TRIANGLES, 6 * m_faces, GL_UNSIGNED_INT, ((GLubyte *)nullptr + (0)));
}

} // end of oglproj namespace
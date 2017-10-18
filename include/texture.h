// bmp_reader.h

#ifndef OGLPROJ_TGA_IMAGE_H_
#define OGLPROJ_TGA_IMAGE_H_

#include <fstream>

#include <gl/glew.h>

namespace oglproj
{

// A class to load TGA image file and to load OpenGL texture.
class Texture
{
public:
    Texture();
    ~Texture();
    // Loads a TGA file into an array suitable for loading into an
    // OpenGL texture.  This method only supports 24 or 32 bpp images.
    // Data is stored in the array as RGBA, 4 bytes per
    // pixel.
    void load_tga(const char* file_name);

    void write_tga(const char* file_name);
    // Loads the texture into an OpenGL texture. This method only supports
    // 24 or 32 bpp images.
    void load_texture();
    void bind(GLenum texture_unit);
    GLuint get_width();
    GLuint get_height();
private:
    // Read a little-endian short (2 bytes) from stream and return
    // the value as an int.
    // @param stream the std::ifstream to read from.
    int read_short_le(std::ifstream& stream);

    // Read a little-endian int (4 bytes) from stream and return
    // the value.
    // @param stream the std::ifstream to read from.
    int read_int_le(std::ifstream& stream);

    // Write a little-endian short (2 bytes) from stream and return
    // the value as an int.
    // @param stream the std::ifstream to read from.
    void write_short_le(std::ofstream& stream, int value);

    GLuint m_tex_id;
    GLubyte* m_pixel_data;
    GLuint m_width;
    GLuint m_height;
};

} // end of namespace oglproj

#endif
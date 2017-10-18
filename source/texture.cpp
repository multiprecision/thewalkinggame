// texture.cpp
// load TGA image file

#include "texture.h"

#include <string>
#include <sstream>
#include <thread>
#include <iostream>

namespace oglproj
{

Texture::Texture()
    : m_tex_id(0)
    , m_width(0)
    , m_height(0)
    , m_pixel_data(nullptr)
{
}

Texture::~Texture()
{
    if (m_pixel_data != nullptr)
        delete[] m_pixel_data;
    glDeleteTextures(1, &m_tex_id);
}

int Texture::read_short_le(std::ifstream& stream)
{
    unsigned char ic; // input char
    int result;  // result

    ic = static_cast<char>(stream.get());
    result = ic;
    ic = static_cast<char>(stream.get());
    result |= ((unsigned int)ic << 8);
    return result;
}

int Texture::read_int_le(std::ifstream& fstr)
{
    int result = 0;
    unsigned char ic = 0;

    ic = static_cast<char>(fstr.get());
    result = ic;
    ic = static_cast<char>(fstr.get());
    result |= (static_cast<char>(ic) << 8);
    ic = static_cast<char>(fstr.get());
    result |= (static_cast<char>(ic) << 16);
    ic = static_cast<char>(fstr.get());
    result |= (static_cast<char>(ic) << 24);
    return result;
}


void Texture::write_short_le(std::ofstream& stream, int value)
{
    unsigned char lowOrder = (unsigned char)value;
    unsigned char highOrder = (unsigned char)(value >> 8);

    stream.put(lowOrder);
    stream.put(highOrder);
}

void Texture::load_tga(const char* file_name)
{
    if (m_pixel_data != nullptr)
        throw std::runtime_error("Data already exist.");
    // Open file for reading
    std::ifstream input_file(file_name, std::ios::binary);
    GLubyte* pixel_data = nullptr;
    GLubyte* image_buffer = nullptr;
    try
    {
        if (!input_file)
        {
            std::string msg = std::string("Error: can't open ") + file_name;
            throw std::runtime_error(msg);
        }

        int id_len = input_file.get();      // Length of image ID field
        int map_type = input_file.get();    // Color map type (expect 0 == no color map)
        int type_code = input_file.get();   // Image type code (expect 2 == uncompressed)
        input_file.ignore(5);              // Color map info (ignored)
        int x_origin = read_short_le(input_file);  // X origin
        int y_origin = read_short_le(input_file);  // Y origin
        m_width = read_short_le(input_file);        // Image width
        m_height = read_short_le(input_file);       // Image height
        int bpp = input_file.get();               // Bits per pixel (expect 24 or 32)
        input_file.ignore();                      // Image descriptor (expect 0 for 24bpp and 8 for 32bpp)

        if (type_code != 2 || map_type != 0)
            throw std::runtime_error("File does not appear to be a non-color-mapped, uncompressed TGA image.");

        if (bpp != 24 && bpp != 32)
            throw std::runtime_error("File must be 24 or 32 bits per pixel.");

        // Skip the image ID data
        if (id_len > 0) input_file.ignore(id_len);

        // Color map data would be here, but we assume no color map

        // Read pixel data
        // 24 bpp -- Blue, Green, Red
        // 32 bpp -- Blue, Green, Red, Alpha
        // pixel_data -- stored as RGBA or RGB
        const unsigned size = m_width * m_height * 4;
        const unsigned bytes_num = (bpp == 32 ? 4 : 3);
        const unsigned buffer_size = m_width * m_height * bytes_num;
        image_buffer = new GLubyte[size];
        input_file.read(reinterpret_cast<char*>(image_buffer), buffer_size);
        pixel_data = new GLubyte[size];
        const unsigned width_height = m_width * m_height;
        for (unsigned int i = 0; i < width_height; i++)
        {
            pixel_data[i * 4 + 2] = image_buffer[i * bytes_num]; // Blue
            pixel_data[i * 4 + 1] = image_buffer[i * bytes_num + 1]; // Green
            pixel_data[i * 4] = image_buffer[i * bytes_num + 2]; // Red
            if (bpp == 32)
                pixel_data[i * 4 + 3] = image_buffer[i * bytes_num + 3];
            else
                pixel_data[i * 4 + 3] = 0xFF;
        }
        delete[] image_buffer;
        input_file.close();
        std::stringstream ss;
        ss << "[INFO][THREAD " << std::this_thread::get_id() << "] Loaded "
           << file_name << " (" << m_width << " x " << m_height << ", " << bpp
           << " bpp, origin(" << x_origin << ", " << y_origin << "))" << std::endl;
        std::cout << ss.str();
    }
    catch (std::runtime_error& ex)
    {
        input_file.close();
        if (image_buffer != nullptr)
            delete[] image_buffer;
        throw ex;
    }
    m_pixel_data = pixel_data;
}


void Texture::write_tga(const char* file_name)
{
    std::ofstream output_file(file_name, std::ios::binary);
    try
    {
        if (!output_file)
        {
            std::string msg = std::string("Unable to open file ") + file_name +
                              std::string(" for writing.");
            throw std::runtime_error(msg);
        }

        const char zero[] = { 0, 0, 0, 0, 0 };

        output_file.put(0);          // Length of image ID field
        output_file.put(0);          // Color map type (0 == no color map)
        output_file.put(2);          // Image type code (2 == uncompressed)
        output_file.write(zero, 5);  // Color map info (ignored)
        output_file.write(zero, 2);  // X origin (0)
        output_file.write(zero, 2);  // Y origin (0)
        write_short_le(output_file, m_width);    // Image width
        write_short_le(output_file, m_height);   // Image height
        output_file.put(32);               // Bits per pixel (32)
        output_file.put(8);                // Image descriptor (8 => 32bpp)
        for (GLuint i = 0; i < m_width * m_height; i++)
        {
            output_file.put(m_pixel_data[i * 4 + 2]);  // Blue
            output_file.put(m_pixel_data[i * 4 + 1]);  // Green
            output_file.put(m_pixel_data[i * 4]);  // Red
            output_file.put(m_pixel_data[i * 4 + 3]);  // alpha
        }
        output_file.close();
    }
    catch (std::runtime_error& ex)
    {
        output_file.close();
        throw ex;
    }
}

void Texture::load_texture()
{
    if (m_pixel_data == nullptr)
        throw std::runtime_error("No image data.");
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_pixel_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    std::cout << "[INFO][THREAD " << std::this_thread::get_id() << "] Loaded texture " << tex_id << " into GPU." << std::endl;
    m_tex_id = tex_id;
}


void Texture::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, m_tex_id);
}

GLuint get_width();
GLuint get_height();

} // end of namespace oglproj
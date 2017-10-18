// shader_program.h

#ifndef OGLPROJ_SHADER_PROGRAM_H_
#define OGLPROJ_SHADER_PROGRAM_H_

#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include <gl/glew.h>
#include <glm/glm.hpp>

namespace oglproj
{

// A GLSL program.
class Shader_program
{
public:
    Shader_program();
    ~Shader_program();
private:
    // Make these private in order to make the object non-copyable
    Shader_program(const Shader_program&) { }
    Shader_program & operator=(const Shader_program&)
    {
        return *this;
    }
public:

    void compile_shader(const char* file_name);
    void compile_shader(const char* file_name, GLenum type);
    void compile_shader(const std::string& source, GLenum type, const char* file_name);
    void link();
    void validate();
    void use();
    int  get_handle();
    bool is_linked();

    void bind_attrib_location(GLuint location, const char* name);
    void bind_frag_data_location(GLuint location, const char* name);

    void set_uniform(const char* name, float x, float y, float z);
    void set_uniform(const char* name, const glm::vec2& v);
    void set_uniform(const char* name, const glm::vec3& v);
    void set_uniform(const char* name, const glm::vec4& v);
    void set_uniform(const char* name, const glm::mat4& m);
    void set_uniform(const char* name, const glm::mat3& m);
    void set_uniform(const char* name, float val);
    void set_uniform(const char* name, int val);
    void set_uniform(const char* name, bool val);
    void set_uniform(const char* name, GLuint val);

    void print_active_uniforms();
    void print_active_uniform_blocks();
    void print_active_attributes();
    const char* get_type_string(GLenum type);
private:
    GLint  get_uniform_location(const char* name);
    bool file_exists(const std::string& file_name);
    std::string get_extension(const char* file_name);

    int  m_handle;
    bool m_linked;
    std::map<std::string, int> m_uniform_locations;
};

} // end of namespace oglproj

#endif

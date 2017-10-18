// shader_program.cpp

#include "shader_program.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace oglproj
{

namespace glsl_shader_info
{

struct Shader_file_ext
{
    const char *ext;
    GLenum type;
};

struct Shader_file_ext extensions[] =
{
    { ".vs", GL_VERTEX_SHADER },
    { ".vert", GL_VERTEX_SHADER },
    { ".gs", GL_GEOMETRY_SHADER },
    { ".geom", GL_GEOMETRY_SHADER },
    { ".tcs", GL_TESS_CONTROL_SHADER },
    { ".tes", GL_TESS_EVALUATION_SHADER },
    { ".fs", GL_FRAGMENT_SHADER },
    { ".frag", GL_FRAGMENT_SHADER },
    { ".cs", GL_COMPUTE_SHADER }
};

}

Shader_program::Shader_program() : m_handle(0), m_linked(false)
{
}

Shader_program::~Shader_program()
{
    if (m_handle == 0) return;

    // Query the number of attached shaders
    GLint n_shaders = 0;
    glGetProgramiv(m_handle, GL_ATTACHED_SHADERS, &n_shaders);

    // Get the shader names
    GLuint * shader_names = new GLuint[n_shaders];
    glGetAttachedShaders(m_handle, n_shaders, nullptr, shader_names);

    // Delete the shaders
    for (int i = 0; i < n_shaders; i++)
        glDeleteShader(shader_names[i]);

    // Delete the program
    glDeleteProgram(m_handle);

    delete[] shader_names;
}

void Shader_program::compile_shader(const char* file_name)
{
    int numExts = sizeof(glsl_shader_info::extensions) / sizeof(glsl_shader_info::Shader_file_ext);

    // Check the file name's extension to determine the shader type
    std::string ext = get_extension(file_name);
    GLenum type = 0;
    bool match_found = false;
    for (int i = 0; i < numExts; i++)
    {
        if (ext == glsl_shader_info::extensions[i].ext)
        {
            match_found = true;
            type = glsl_shader_info::extensions[i].type;
            break;
        }
    }

    // If we didn't find a match, throw an exception
    if (!match_found)
    {
        std::printf("[ERROR] Unrecognized extension: \"%s\".\n", ext);
        throw std::runtime_error(std::string("Unrecognized extension: ") + ext);
    }

    // Pass the discovered shader type along
    compile_shader(file_name, type);
}

std::string Shader_program::get_extension(const char* name)
{
    std::string name_str(name);

    size_t loc = name_str.find_last_of('.');
    if (loc != std::string::npos)
    {
        return name_str.substr(loc, std::string::npos);
    }
    return "";
}

void Shader_program::compile_shader(const char* file_name, GLenum type)
{
    if (!file_exists(file_name))
    {
        std::printf("[ERROR] Shader \"%s\" does not exist.\n", file_name);
        throw std::runtime_error("Shader \"%s\" does not exist");
    }

    if (m_handle <= 0)
    {
        m_handle = glCreateProgram();
        if (m_handle == 0)
        {
            std::printf("[ERROR] Unable to create shader program.\n");
            throw std::runtime_error("Unable to create shader program.");
        }
    }

    std::ifstream input_file(file_name, std::ios::in);
    if (!input_file)
    {
        std::printf("[ERROR] Unable to open: %s.\n", file_name);
        throw std::runtime_error(std::string("Unable to open ") + file_name + ".");
    }

    // Get file contents
    std::stringstream code;
    code << input_file.rdbuf();
    input_file.close();

    compile_shader(code.str(), type, file_name);
}

void Shader_program::compile_shader(const std::string& source, GLenum type,
                                    const char* file_name)
{
    if (m_handle <= 0)
    {
        m_handle = glCreateProgram();
        if (m_handle == 0)
        {
            std::printf("[ERROR] Unable to create shader program.\n");
            throw std::runtime_error("Unable to create shader program.");
        }
    }

    GLuint shader_handle = glCreateShader(type);

    const char * c_code = source.c_str();
    glShaderSource(shader_handle, 1, &c_code, nullptr);

    // Compile the shader
    glCompileShader(shader_handle);

    // Check for errors
    int result;
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &result);
    if (GL_FALSE == result)
    {
        // Compile failed, get log
        int length = 0;
        std::string log_string;
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &length);
        if (length > 0)
        {
            char* c_log = new char[length];
            int written = 0;
            glGetShaderInfoLog(shader_handle, length, &written, c_log);
            log_string = c_log;
            delete[] c_log;
        }
        std::string msg;
        if (file_name)
        {
            msg = std::string("Shader \"") + file_name + "\" compilation failed.\n";
        }
        else
        {
            msg = "Shader compilation failed.\n";
        }
        msg += log_string;

        std::printf("[ERROR] %s", msg.c_str());
        throw std::runtime_error("Shader compilation failed.");

    }
    else
    {
        // Compile succeeded, attach shader
        glAttachShader(m_handle, shader_handle);
    }
}

void Shader_program::link()
{
    if (m_linked) return;
    if (m_handle <= 0)
    {
        std::printf("Program has not been compiled.\n");
        throw std::runtime_error("Program has not been compiled.");
    }
    glLinkProgram(m_handle);

    int status = 0;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
        // Store log and return false
        int length = 0;
        std::string log_string;

        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0)
        {
            char * c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(m_handle, length, &written, c_log);
            log_string = c_log;
            delete[] c_log;
        }

        std::printf("[ERROR] Program link failed.\n%s", log_string);
        throw std::runtime_error("Program link failed.");
    }
    else
    {
        m_uniform_locations.clear();
        m_linked = true;
    }
}

void Shader_program::use()
{
    if (m_handle <= 0 || (!m_linked))
    {
        std::printf("Shader has not been linked.\n");
        throw std::runtime_error("Shader has not been linked.");
    }
    glUseProgram(m_handle);
}

int Shader_program::get_handle()
{
    return m_handle;
}

bool Shader_program::is_linked()
{
    return m_linked;
}

void Shader_program::bind_attrib_location(GLuint location, const char* name)
{
    glBindAttribLocation(m_handle, location, name);
}

void Shader_program::bind_frag_data_location(GLuint location, const char* name)
{
    glBindFragDataLocation(m_handle, location, name);
}


void Shader_program::set_uniform(const char* name, float x, float y, float z)
{
    GLint loc = get_uniform_location(name);
    glUniform3f(loc, x, y, z);
}

void Shader_program::set_uniform(const char* name, const glm::vec2& v)
{
    GLint loc = get_uniform_location(name);
    glUniform2f(loc, v.x, v.y);
}

void Shader_program::set_uniform(const char* name, const glm::vec3& v)
{
    this->set_uniform(name, v.x, v.y, v.z);
}

void Shader_program::set_uniform(const char* name, const glm::vec4& v)
{
    GLint loc = get_uniform_location(name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void Shader_program::set_uniform(const char* name, const glm::mat4& m)
{
    GLint loc = get_uniform_location(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void Shader_program::set_uniform(const char* name, const glm::mat3& m)
{
    GLint loc = get_uniform_location(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void Shader_program::set_uniform(const char* name, float val)
{
    GLint loc = get_uniform_location(name);
    glUniform1f(loc, val);
}

void Shader_program::set_uniform(const char* name, int val)
{
    GLint loc = get_uniform_location(name);
    glUniform1i(loc, val);
}

void Shader_program::set_uniform(const char* name, GLuint val)
{
    GLint loc = get_uniform_location(name);
    glUniform1ui(loc, val);
}

void Shader_program::set_uniform(const char* name, bool val)
{
    int loc = get_uniform_location(name);
    glUniform1i(loc, val);
}

void Shader_program::print_active_uniforms()
{
    GLint numUniforms = 0;
    glGetProgramInterfaceiv(m_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    GLenum properties[] =
    { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

    std::printf("[DEBUG] Active uniforms:\n");
    for (int i = 0; i < numUniforms; ++i)
    {
        GLint results[4];
        glGetProgramResourceiv(m_handle, GL_UNIFORM, i, 4, properties, 4, nullptr, results);

        if (results[3] != -1) continue;  // Skip uniforms in blocks
        GLint name_buf_size = results[0] + 1;
        char * name = new char[name_buf_size];
        glGetProgramResourceName(m_handle, GL_UNIFORM, i, name_buf_size, nullptr, name);
        std::printf("[DEBUG]   %-5d %s (%s)\n", results[2], name, get_type_string(results[1]));
        delete[] name;
    }
}

void Shader_program::print_active_uniform_blocks()
{
    GLint numBlocks = 0;

    glGetProgramInterfaceiv(m_handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
    GLenum blockProps[] =
    { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
    GLenum blockIndex[] =
    { GL_ACTIVE_VARIABLES };
    GLenum props[] =
    { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

    for (int block = 0; block < numBlocks; ++block)
    {
        GLint blockInfo[2];
        glGetProgramResourceiv(m_handle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, nullptr, blockInfo);
        GLint numUnis = blockInfo[0];

        char * blockName = new char[blockInfo[1] + 1];
        glGetProgramResourceName(m_handle, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, nullptr, blockName);
        std::printf("[DEBUG] Uniform block \"%s\":\n", blockName);
        delete[] blockName;

        GLint * unifIndexes = new GLint[numUnis];
        glGetProgramResourceiv(m_handle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, nullptr, unifIndexes);

        for (int unif = 0; unif < numUnis; ++unif)
        {
            GLint uniIndex = unifIndexes[unif];
            GLint results[3];
            glGetProgramResourceiv(m_handle, GL_UNIFORM, uniIndex, 3, props, 3, nullptr, results);

            GLint name_buf_size = results[0] + 1;
            char * name = new char[name_buf_size];
            glGetProgramResourceName(m_handle, GL_UNIFORM, uniIndex, name_buf_size, nullptr, name);
            std::printf("[DEBUG]   %s (%s)\n", name, get_type_string(results[1]));
            delete[] name;
        }

        delete[] unifIndexes;
    }
}

void Shader_program::print_active_attributes()
{
    GLint numAttribs;
    glGetProgramInterfaceiv(m_handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

    GLenum properties[] =
    { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

    std::printf("[DEBUG] Active attributes:\n");
    for (int i = 0; i < numAttribs; ++i)
    {
        GLint results[3];
        glGetProgramResourceiv(m_handle, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, results);

        GLint name_buf_size = results[0] + 1;
        char * name = new char[name_buf_size];
        glGetProgramResourceName(m_handle, GL_PROGRAM_INPUT, i, name_buf_size, nullptr, name);
        std::printf("[DEBUG]   %-5d %s (%s)\n", results[2], name, get_type_string(results[1]));
        delete[] name;
    }
}

const char* Shader_program::get_type_string(GLenum type)
{
    // There are many more types than are covered here
    switch (type)
    {
    case GL_FLOAT:
        return "float";
    case GL_FLOAT_VEC2:
        return "vec2";
    case GL_FLOAT_VEC3:
        return "vec3";
    case GL_FLOAT_VEC4:
        return "vec4";
    case GL_DOUBLE:
        return "double";
    case GL_INT:
        return "int";
    case GL_UNSIGNED_INT:
        return "unsigned int";
    case GL_BOOL:
        return "bool";
    case GL_FLOAT_MAT2:
        return "mat2";
    case GL_FLOAT_MAT3:
        return "mat3";
    case GL_FLOAT_MAT4:
        return "mat4";
    default:
        return "unknown";
    }
}

void Shader_program::validate()
{
    if (!is_linked())
        std::printf("Program is not linked.\n");

    GLint status;
    glValidateProgram(m_handle);
    glGetProgramiv(m_handle, GL_VALIDATE_STATUS, &status);

    if (GL_FALSE == status)
    {
        // Store log and return false
        int length = 0;
        std::string log_string;

        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0)
        {
            char * c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(m_handle, length, &written, c_log);
            log_string = c_log;
            delete[] c_log;
        }

        std::printf("[ERROR] Program failed to validate.\n%s", log_string);

    }
}

int Shader_program::get_uniform_location(const char* name)
{
    std::map<std::string, int>::iterator pos;
    pos = m_uniform_locations.find(name);

    if (pos == m_uniform_locations.end())
    {
        m_uniform_locations[name] = glGetUniformLocation(m_handle, name);
    }

    return m_uniform_locations[name];
}

bool Shader_program::file_exists(const std::string& file_name)
{
    struct stat info;
    int ret = -1;

    ret = stat(file_name.c_str(), &info);
    return 0 == ret;
}

} // end of namespace oglproj
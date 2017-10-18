// utility.cpp
// Implements utility routines for OpenGL

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include "utility.h"

namespace oglproj
{

namespace utility
{

void print_opengl_info(bool include_extensions)
{
    std::cout << "[INFO] GLEW version: " << glewGetString(GLEW_VERSION)
              << std::endl;
    std::cout << "[INFO] GLFW version: " << glfwGetVersionString()
              << std::endl;
    std::cout << "[INFO] GLM version: " << GLM_VERSION_MAJOR << "."
              << GLM_VERSION_MINOR << "." << GLM_VERSION_PATCH << "." <<
              GLM_VERSION_REVISION << std::endl;
    std::cout << "[INFO] OpenGL vendor: " << glGetString(GL_VENDOR)
              << std::endl;
    std::cout << "[INFO] OpenGL renderer: " << glGetString(GL_RENDERER)
              << std::endl;
    std::cout << "[INFO] OpenGL version: " << glGetString(GL_VERSION)
              << std::endl;
    std::cout << "[INFO] GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;
    if (include_extensions)
    {
        std::cout << "[DEBUG] Extensions: ";
        GLint num_extensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
        for (int i = 0; i < num_extensions; i++)
            std::cout << glGetStringi(GL_EXTENSIONS, i) << ", ";
        std::cout << std::endl;
    }

}

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                             GLenum severity, GLsizei, const GLchar* msg,
                             const void*)
{

    std::string source_str;
    switch (source)
    {
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_str = "WindowSystem";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "App";
        break;
    case GL_DEBUG_SOURCE_API:
        source_str = "OpenGL";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_str = "ShaderCompiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_str = "ThirdParty";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        source_str = "Other";
        break;
    default:
        source_str = "Unknown";
    }

    std::string type_str;
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        type_str = "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_str = "Deprecated";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_str = "Undefined";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        type_str = "Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        type_str = "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "PushGroup";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "PopGroup";
        break;
    case GL_DEBUG_TYPE_OTHER:
        type_str = "Other";
        break;
    default:
        type_str = "Unknown";
    }

    std::string severity_str;
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        severity_str = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severity_str = "MED";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severity_str = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity_str = "NOTIFY";
        break;
    default:
        severity_str = "UNKNOWN";
    }

    std::printf("[DEBUG][%s] %s:%s(%d): %s\n", severity_str.c_str(),
                source_str.c_str(), type_str.c_str(), id, msg);
}

int check_for_opengl_error(const char* file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum gl_err;
    int    ret_code = 0;

    gl_err = glGetError();
    while (gl_err != GL_NO_ERROR)
    {
        const char* message = "";
        switch (gl_err)
        {
        case GL_INVALID_ENUM:
            message = "Invalid enum";
            break;
        case GL_INVALID_VALUE:
            message = "Invalid value";
            break;
        case GL_INVALID_OPERATION:
            message = "Invalid operation";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            message = "Invalid framebuffer operation";
            break;
        case GL_OUT_OF_MEMORY:
            message = "Out of memory";
            break;
        default:
            message = "Unknown error";
        }

        std::printf("glError in file %s @ line %d: %s\n", file, line, message);
        ret_code = 1;
        gl_err = glGetError();
    }
    return ret_code;
}

} // end of namespace utility

} // end of namespace oglproj
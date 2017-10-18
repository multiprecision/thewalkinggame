// utility.h
#ifndef OGLPROJ_UTILITY_H_
#define OGLPROJ_UTILITY_H_

// defines APIENTRY
#ifndef APIENTRY
#    if defined(__MINGW32__)
#        ifndef WIN32_LEAN_AND_MEAN
#           define WIN32_LEAN_AND_MEAN 1
#        endif
#        ifndef NOMINMAX
#            define NOMINMAX
#        endif
#        include <windows.h>
#    elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#        ifndef WIN32_LEAN_AND_MEAN
#            define WIN32_LEAN_AND_MEAN 1
#        endif
#        ifndef NOMINMAX
#            define NOMINMAX
#        endif
#        include <windows.h>
#    else
#        define APIENTRY
#    endif
#endif // APIENTRY

namespace oglproj
{

namespace utility
{

void print_opengl_info(bool include_extensions);
void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                             GLenum severity, GLsizei length, const GLchar* msg, const void* param);
int check_for_opengl_error(const char* file, int line);

} // end of utility namespace

} // end of ogl namespace

#endif
// main.cpp
// Defines program main entry point.

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <sstream>
#include <thread>
#include "main_scene.h"
#include "utility.h"

// Note about logger levels
//
// UNKNOWN
// An unknown message that should always be logged.
//
// FATAL
// An unhandleable error that results in a program crash.
//
// ERROR
// A handleable error condition.
//
// WARN
// A warning.
//
// INFO
// Generic (useful) information about system operation.
//
// DEBUG
// Low-level information for developers.

#define WINDOW_INITIAL_WIDTH 800
#define WINDOW_INITIAL_HEIGHT 600

namespace oglproj
{

int g_fps_limit = 0;

void error_callback(int error, const char* description);

void error_callback(int, const char* description)
{
    fputs(description, stderr);
}

} // end of namespace oglproj


int main(int, char**)
{
    GLFWwindow* main_window;
    oglproj::Scene* main_scene;
    glfwSetErrorCallback(oglproj::error_callback);
    if (!glfwInit())
    {
        // glfwInit failed
        std::printf("[FATAL] Failed to initialize GLFW.");
        exit(1000);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    main_window = glfwCreateWindow(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT,
                                   "OpenGL Project", 0, 0); // Windowed
    glfwMakeContextCurrent(main_window);
    if (!main_window)
    {
        std::printf("[FATAL] Failed to create window.\n");
        exit(1001);
    }
    std::printf("[INFO] Window created.\n");
    // Initialize glew.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // glewInit failed
        std::cout << "[FATAL] Failed to initialize GLEW." << std::endl
                  << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        exit(1002);
    }
    glfwSwapInterval(0);
    oglproj::utility::print_opengl_info(false);

    main_scene = new oglproj::Main_scene(main_window);

    // Initialization
    // Enable for debugging
#ifdef _DEBUG
    glDebugMessageCallback(oglproj::utility::debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                         GL_DEBUG_SEVERITY_NOTIFICATION, -1, "OpenGL debug mode.");
#endif
    main_scene->init_scene();
    double t = 0;
    double last_t = 0;
    double dt = 0;
    glfwSetTime(0.0);
    // Enter the main loop
    while (!glfwWindowShouldClose(main_window))
    {
        t = glfwGetTime();
        static const int samples = 100;
        static double time[samples];
        static int index = 0;

        glfwPollEvents();
        main_scene->update(glfwGetTime());
        main_scene->render();
        glfwSwapBuffers(main_window);

        // Update FPS
        time[index] = glfwGetTime();
        index = (index + 1) % samples;

        if (index == 0)
        {
            double sum = 0.0f;
            for (int i = 0; i < samples - 1; i++)
                sum += time[i + 1] - time[i];
            double fps = samples / sum;

            std::stringstream ss;
            ss << "OpenGL Project";
            ss.precision(6);
            ss << " (FPS: " << fps << ")";
            glfwSetWindowTitle(main_window, ss.str().c_str());
        }

        last_t = glfwGetTime();
        dt = last_t - t;
        if (oglproj::g_fps_limit > 0)
            if (dt < 1.0 / oglproj::g_fps_limit)
                std::this_thread::sleep_for(std::chrono::nanoseconds(long long(((1.0 / oglproj::g_fps_limit) - dt) * 1e9)));
    }
    // Close window and terminate GLFW
    glfwTerminate();
    // Exit program
    return 0;
}
// main_event_handler.cpp

#define _USE_MATH_DEFINES
#include <cmath>
#include <functional>
#include <gl/glew.h>
#include <glfw/glfw3.h>
#include "main_event_handler.h"
#include "main_scene.h"

namespace oglproj
{

Main_event_handler* Main_event_handler::m_instance = nullptr;

Main_event_handler::Main_event_handler(Main_scene* parent)
    : m_parent(parent)
{
    m_window = m_parent->get_parent();
    glfwGetWindowSize(m_window, &m_width, &m_height);
    m_pan_speed = (float)M_PI / m_width;
    m_dolly_speed = 0.2f;
    m_dolly_min_rad = 0.5f;
    m_dolly_max_rad = 6.f;
}

void Main_event_handler::create_instance(Main_scene* parent)
{
    if (m_instance != nullptr) return;
    m_instance = new Main_event_handler(parent);
    glfwSetKeyCallback(m_instance->m_parent->get_parent(), Main_event_handler::key);
    glfwSetScrollCallback(m_instance->m_parent->get_parent(), Main_event_handler::scroll);
    glfwSetWindowSizeCallback(m_instance->m_parent->get_parent(), Main_event_handler::resize);
    glfwSetCursorPosCallback(m_instance->m_parent->get_parent(), Main_event_handler::cursor_pos);
    glfwSetMouseButtonCallback(m_instance->m_parent->get_parent(), Main_event_handler::mouse_button);
}

Main_event_handler* Main_event_handler::get_instance()
{
    return m_instance;
}

void Main_event_handler::resize(GLFWwindow*, int width, int height)
{
    glfwMakeContextCurrent(get_instance()->m_window);
    glViewport(0, 0, width, height);
    get_instance()->m_width = width;
    get_instance()->m_height = height;
    if (!height) height = 1;
    get_instance()->m_parent->get_camera()->set_aspect_ratio(static_cast<float>(width) / height);
    get_instance()->m_pan_speed = (float)M_PI / get_instance()->m_width;
}

void Main_event_handler::key(GLFWwindow*, int key, int, int action, int)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_W)
        {
            get_instance()->m_parent->get_player()->set_state(Character::State::WALK_FORWARD);
            get_instance()->m_parent->get_player()->set_animation_state(0);
        }
        else if (key == GLFW_KEY_A)
        {
            get_instance()->m_parent->get_player()->set_state(Character::State::TURN_LEFT);
            get_instance()->m_parent->get_player()->set_animation_state(0);
        }
        else if (key == GLFW_KEY_S)
        {
            get_instance()->m_parent->get_player()->set_state(Character::State::WALK_BACKWARD);
            get_instance()->m_parent->get_player()->set_animation_state(86);
        }
        else if (key == GLFW_KEY_D)
        {
            get_instance()->m_parent->get_player()->set_state(Character::State::TURN_RIGHT);
            get_instance()->m_parent->get_player()->set_animation_state(0);
        }
        else if (key == GLFW_KEY_ESCAPE)
        {
            exit(0);
        }
    }
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_LEFT)
        {
            get_instance()->m_parent->get_camera()->add_yaw_angle( (float)M_PI / 60 );
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            get_instance()->m_parent->get_camera()->add_yaw_angle((float)-M_PI / 60);
        }
        else if (key == GLFW_KEY_UP)
        {
            get_instance()->m_parent->get_camera()->add_pitch_angle((float)M_PI / 60);
        }
        else if (key == GLFW_KEY_DOWN)
        {
            get_instance()->m_parent->get_camera()->add_pitch_angle((float)-M_PI / 60);
        }
        else if (key == GLFW_KEY_PAGE_UP)
        {
            // dolly in
            if (get_instance()->m_parent->get_camera()->get_radius() - 0.2f > get_instance()->m_dolly_min_rad)
                get_instance()->m_parent->get_camera()->add_radius(-0.2f);
        }
        else if (key == GLFW_KEY_PAGE_DOWN)
        {
            // dolly out
            if (get_instance()->m_parent->get_camera()->get_radius() + 0.2f < get_instance()->m_dolly_max_rad)
                get_instance()->m_parent->get_camera()->add_radius(0.2f);
        }
    }
    if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D)
        {
            get_instance()->m_parent->get_player()->set_state(Character::State::IDLE);
            get_instance()->m_parent->get_player()->set_animation_state(0);
        }
    }
}

void Main_event_handler::mouse_button(GLFWwindow*, int, int, int)
{
}

void Main_event_handler::scroll(GLFWwindow*, double, double y_offset)
{
    if (y_offset > 0)
    {
        // dolly in
        if (get_instance()->m_parent->get_camera()->get_radius() - get_instance()->m_dolly_speed > get_instance()->m_dolly_min_rad)
            get_instance()->m_parent->get_camera()->add_radius(-get_instance()->m_dolly_speed);
    }
    else if (y_offset < 0)
    {
        // dolly out
        if (get_instance()->m_parent->get_camera()->get_radius() + get_instance()->m_dolly_speed < get_instance()->m_dolly_max_rad)
            get_instance()->m_parent->get_camera()->add_radius(get_instance()->m_dolly_speed);
    }

}

void Main_event_handler::cursor_pos(GLFWwindow*, double x, double y)
{
    get_instance()->m_prev_cursor_pos_x = get_instance()->m_cursor_pos_x;
    get_instance()->m_prev_cursor_pos_y = get_instance()->m_cursor_pos_y;
    get_instance()->m_cursor_pos_x = static_cast<int>(x);
    get_instance()->m_cursor_pos_y = static_cast<int>(y);

    // Grab cursor
    if (glfwGetMouseButton(get_instance()->m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
            glfwGetMouseButton(get_instance()->m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (x > get_instance()->m_width - 8)
        {
            glfwSetCursorPos(get_instance()->m_window, 8, y);
            get_instance()->m_prev_cursor_pos_x = get_instance()->m_cursor_pos_x = 8;
        }
        if (x < 8)
        {
            glfwSetCursorPos(get_instance()->m_window, get_instance()->m_width - 8, y);
            get_instance()->m_prev_cursor_pos_x = get_instance()->m_cursor_pos_x = get_instance()->m_width - 8;
        }
        if (y > get_instance()->m_height - 8)
        {
            glfwSetCursorPos(get_instance()->m_window, x, 8);
            get_instance()->m_prev_cursor_pos_y = get_instance()->m_cursor_pos_y = 8;
        }
        if (y < 8)
        {
            glfwSetCursorPos(get_instance()->m_window, y, get_instance()->m_height - 8);
            get_instance()->m_prev_cursor_pos_y = get_instance()->m_cursor_pos_y = get_instance()->m_height - 8;
        }
    }
    if (get_instance()->m_prev_cursor_pos_x == 0 || get_instance()->m_prev_cursor_pos_x == 0)
        return;
    if (glfwGetMouseButton(get_instance()->m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
            glfwGetMouseButton(get_instance()->m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {

        get_instance()->m_parent->get_camera()->add_yaw_angle(static_cast<float>(get_instance()->m_cursor_pos_x - get_instance()->m_prev_cursor_pos_x) * -get_instance()->m_pan_speed);
        get_instance()->m_parent->get_camera()->add_pitch_angle(static_cast<float>(get_instance()->m_cursor_pos_y - get_instance()->m_prev_cursor_pos_y) * get_instance()->m_pan_speed);
    }
}


} // end of oglproj namespace
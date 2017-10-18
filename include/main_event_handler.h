// main_event_handler.h

#ifndef OGLPROJ_MAIN_EVENT_HANDLER_H_
#define OGLPROJ_MAIN_EVENT_HANDLER_H_

namespace oglproj
{

class Main_scene;

class Main_event_handler
{
public:
    static void create_instance(Main_scene* parent);

    static Main_event_handler* get_instance();

    Main_event_handler(Main_scene* parent);

    // Called when screen is resized.
    static void resize(GLFWwindow* window, int width, int height);
    // Called when keyboard button is pressed.
    static void key(GLFWwindow* window, int key, int scancode, int action, int mods);
    // Called when mouse wheel is used.
    static void scroll(GLFWwindow* window, double x, double y);
    // Called when cursor position is changed.
    static void cursor_pos(GLFWwindow* window, double x, double y);
    // Called when mouse button is pressed.
    static void mouse_button(GLFWwindow* window, int button, int action, int mods);
private:
    // Used for calculating camera changes.
    int m_prev_cursor_pos_x;
    int m_prev_cursor_pos_y;
    int m_cursor_pos_x;
    int m_cursor_pos_y;
    float m_pan_speed;
    float m_dolly_speed;
    float m_dolly_min_rad;
    float m_dolly_max_rad;
    // Width of the window.
    int m_width;
    // Height of the window.
    int m_height;
    Main_scene* m_parent;
    GLFWwindow* m_window;
    static Main_event_handler* m_instance;
};

} // end of oglproj namespace

#endif
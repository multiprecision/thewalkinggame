// main_scene.cpp
// Implements the main scene.

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <gl/glew.h>
#include <glfw/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "main_scene.h"
#include "shader_program.h"
#include "mesh.h"
#include "utility.h"
#include "thread_pool.h"

#define OGLPROJ_CHARACTERS_NUM 3
#define OGLPROJ_CHARACTER_MESHES_NUM 175
#define OGLPROJ_CHARACTER_START_WALKING_CYCLE 103

namespace oglproj
{

extern int g_fps_limit;

Main_scene::Main_scene(GLFWwindow* window) :
    Scene(window)
{
    int w, h;
    glfwGetWindowSize(m_parent, &w, &h);
    m_camera = new Third_person_camera((float)M_PI * 3 / 2, 0.f, 2, glm::vec3(0, 0, 1.5), 90, (float)w / h, 0.1f, 100.f);
    m_camera->set_aspect_ratio((float)w / h);
    Main_event_handler::create_instance(this);
    m_light = new Light;
    m_light->intensity = glm::vec3(1, 1, 1);
    m_light->position = glm::vec4(0, 0, 12, 1);
}

Main_scene::~Main_scene()
{
    for (int outer_idx = 0; outer_idx < OGLPROJ_CHARACTERS_NUM; outer_idx++)
    {
        for (Mesh* it : *m_character_meshes[outer_idx])
        {
            delete it;
        }
        delete m_character_meshes[outer_idx];
    }
    for (auto tex : m_textures)
        delete tex;
    for (auto c : m_characters)
        delete c.second;
    for (auto n : m_nameplates)
        delete n.second;
    delete m_floor;
    delete console_thread;
    delete m_camera;
    delete m_light;
}


void Main_scene::init_scene()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    m_character_meshes.resize(OGLPROJ_CHARACTERS_NUM);
    size_t c_num_threads_supported
        = std::thread::hardware_concurrency();
    Thread_pool* pool;
    std::cout << "[INFO] Loading " << OGLPROJ_CHARACTER_MESHES_NUM * OGLPROJ_CHARACTERS_NUM << " meshes using "
              << (c_num_threads_supported > OGLPROJ_CHARACTER_MESHES_NUM
                  ? OGLPROJ_CHARACTER_MESHES_NUM : c_num_threads_supported)
              << " threads." << std::endl;
    std::string file;
    m_character_meshes.resize(OGLPROJ_CHARACTERS_NUM);
    // load characters meshes
    for (int outer_idx = 0; outer_idx < OGLPROJ_CHARACTERS_NUM; outer_idx++)
    {
        m_character_meshes[outer_idx] = new std::vector<Mesh *>();
        m_character_meshes[outer_idx]->resize(OGLPROJ_CHARACTER_MESHES_NUM);
        pool = new Thread_pool(c_num_threads_supported);
        for (int idx = 0; idx < OGLPROJ_CHARACTER_MESHES_NUM; idx++)
        {
            file = static_cast<std::string>("../content/meshes/character_") + std::to_string(outer_idx + 1) + "_"
                   + std::to_string(idx) + ".obj.bin";
            m_character_meshes[outer_idx]->at(idx) = new Mesh();
            pool->enqueue(&Mesh::import_bin_file, m_character_meshes[outer_idx]->at(idx), file);
        }
        delete pool;
        for (int idx = 0; idx < OGLPROJ_CHARACTER_MESHES_NUM; idx++)
        {
            m_character_meshes.at(outer_idx)->at(idx)->store_vbo(false);
        }
    }
    // Load texture file
    const int c_tex_num = 5;
    m_textures.resize(c_tex_num);
    for (size_t idx = 0; idx < c_tex_num; idx++)
        m_textures[idx] = new Texture();
    std::cout << "[INFO] Loading " << c_tex_num << " textures using "
              << (c_num_threads_supported > 5 ? 5 : c_num_threads_supported) << " threads." << std::endl;
    pool = new Thread_pool(c_num_threads_supported);
    pool->enqueue(&Texture::load_tga, m_textures[0], "../content/textures/floor.tga");
    pool->enqueue(&Texture::load_tga, m_textures[1], "../content/fonts/inconsolata.tga");
    pool->enqueue(&Texture::load_tga, m_textures[2], "../content/textures/character_1.tga");
    pool->enqueue(&Texture::load_tga, m_textures[3], "../content/textures/character_2.tga");
    pool->enqueue(&Texture::load_tga, m_textures[4], "../content/textures/character_3.tga");
    delete pool;

    for (size_t idx = 0; idx < c_tex_num; idx++)
        m_textures[idx]->load_texture();

    m_characters["player"] = new Character(this, (float)M_PI_2, glm::vec3(0, 0, 0),
                                           m_character_meshes[0], m_textures[2], OGLPROJ_CHARACTER_START_WALKING_CYCLE);
    m_nameplates["player"] = new Nameplate(this, m_characters["player"], "player", m_textures[1]);
    m_characters["npc_1"] = new Character(this, (float)-M_PI_2, glm::vec3(1, 0, 0),
                                          m_character_meshes[1], m_textures[3], OGLPROJ_CHARACTER_START_WALKING_CYCLE);
    m_nameplates["npc_1"] = new Nameplate(this, m_characters["npc_1"], "npc_1", m_textures[1]);
    m_characters["npc_2"] = new Character(this, (float)M_PI, glm::vec3(0, 1, 0),
                                          m_character_meshes[2], m_textures[4], OGLPROJ_CHARACTER_START_WALKING_CYCLE);
    m_nameplates["npc_2"] = new Nameplate(this, m_characters["npc_2"], "npc_2", m_textures[1]);

    m_characters["npc_1"]->set_walking_speed(0.7f);
    m_characters["npc_2"]->set_walking_speed(1.9f);

    m_floor = new Floor(this, 128, 128, 1, 1, 64, 64, m_textures[0]);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "[INFO] Initialization successful." << std::endl;
    console_thread = new std::thread(&Main_scene::process_console_input, this);
    console_thread->detach();
}

void Main_scene::update(double t)
{
    static float last_t = 0;
    m_floor->update(t);
    for (auto c : m_characters)
        c.second->update(t);
    // track
    m_camera->set_target(m_characters["player"]->get_position() + glm::vec3(0, 0, 1.5f));
    if (glfwGetMouseButton(m_parent, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        m_characters["player"]->set_angle(m_camera->get_yaw_angle() - (float)M_PI);
    for (auto c : m_characters)
    {
        if (c.first == "player")
            continue;
        // simple AI--walk around in circles
        if (c.second->get_state() != Character::State::WALK_FORWARD)
            c.second->set_state(Character::State::WALK_FORWARD);
        c.second->set_angle(c.second->get_angle() - (((float)t - last_t) * 0.628318f)); // 36 deg per second
    }
    last_t = (float)t;
}


void Main_scene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_floor->render();
    for (auto c : m_characters)
        c.second->render();
    glDisable(GL_DEPTH_TEST);
    for (auto c : m_nameplates)
        c.second->render();
    glEnable(GL_DEPTH_TEST);
}


void Main_scene::process_console_input()
{
    std::cout << "Type \"help\" for available commands." << std::endl;
    std::string line;
    std::string token;
    std::string* arg_s;
    float* arg_f;
    double* arg_d;
    int* arg_i;
    bool load_file = false;
    std::stringstream input_stream;
    for (;;)
    {
        if (input_stream.eof())
        {
            load_file = false;
            input_stream.clear();
        }
        if (!load_file)
        {
            std::cout << "> ";
            std::getline(std::cin, line);
            input_stream.str(line);
        }
        input_stream >> token;
        if (token == "help")
            std::cout << "Available commands:" << std::endl
                      << "help" << std::endl
                      << "exit" << std::endl
                      << "showallnames" << std::endl
                      << "createnpc [string:name] [int:model_id]" << std::endl
                      << "createnpc [string:name] [int:model_id] [float:x] [float:y] [float:z]" << std::endl
                      << "deletenpc [string:name]" << std::endl
                      << "setmodel [string:name] [int:model_id]" << std::endl
                      << "getmodelinfo [string:name]" << std::endl
                      << "setwalkingspeed [string:name] [float:speed]" << std::endl
                      << "getwalkingspeed [string:name]" << std::endl
                      << "setposition [string:name] [float:x] [float:y] [float:z]" << std::endl
                      << "getposition [string:name]" << std::endl
                      << "setangle [string:name] [float:angle]" << std::endl
                      << "getangle [string:name]" << std::endl
                      << "setanimationstate [string:name] [int:state]" << std::endl
                      << "getanimationstate [string:name]" << std::endl
                      << "takecontrol [string:name]" << std::endl
                      << "getcamerainfo" << std::endl
                      << "setcamerapos [float:x] [float:y] [float:z]" << std::endl
                      << "setcameratarget [float:x] [float:y] [float:z]" << std::endl
                      << "setcameraradius [float:radius]" << std::endl
                      << "getlightinfo" << std::endl
                      << "setlightpos [float:x] [float:y] [float:z]" << std::endl
                      << "setlightintensity [float:r] [float:g] [float:b]" << std::endl
                      << "setfpslimit [int:fps]" << std::endl
                      << "sleep [double:seconds] -- pause (for scripting)" << std::endl
                      << "loadscript [string:path] -- the file must be in ASCII encoding (sample script can be found in /binary/testscript.txt)" << std::endl
                      << "test -- for testing purposes" << std::endl
                      << "NOTE: All strings must not contain spaces." << std::endl;
        else if (token == "exit")
        {
            exit(0);
        }
        else if (token == "showallnames")
            for (auto it = m_characters.begin(); it != m_characters.end(); it++)
                std::cout << it->first << std::endl;
        else if (token == "createnpc")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            if (*arg_s == "")
            {
                std::cout << "[ERROR] Name cannot be blank." << std::endl;
                delete arg_s;
                continue;
            }
            if (m_characters.find(*arg_s) != m_characters.end())
            {
                std::cout << "[ERROR] Name already exists." << std::endl;
                delete arg_s;
                continue;
            }
            arg_i = new int;
            input_stream >> *arg_i;
            if (*arg_i >= 0 && *arg_i < OGLPROJ_CHARACTERS_NUM)
            {
                arg_f = new float[3] {0};
                input_stream >> arg_f[0] >> arg_f[1] >> arg_f[2];
                glm::vec3 pos = { arg_f[0], arg_f[1], arg_f[2] };
                m_characters[*arg_s] = new Character(this, 0, pos, m_character_meshes[*arg_i],
                                                     m_textures[*arg_i + 2], OGLPROJ_CHARACTER_START_WALKING_CYCLE);
                m_nameplates[*arg_s] = new Nameplate(this, m_characters[*arg_s], arg_s->c_str(), m_textures[1]);
                delete[] arg_f;
            }
            else
            {
                std::cout << "[ERROR] Range of model_id is 0 - " << OGLPROJ_CHARACTERS_NUM - 1 << std::endl;
            }
            delete arg_i;
            delete arg_s;
        }
        else if (token == "deletenpc")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (*arg_s == "player")
            {
                std::cout << "[ERROR] Cannot delete player." << std::endl;
                delete arg_s;
                continue;
            }
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            m_characters.erase(*arg_s);
            m_nameplates.erase(*arg_s);
            delete ref;
            delete arg_s;
        }
        else if (token == "setmodel")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            arg_i = new int;
            input_stream >> *arg_i;
            if (*arg_i >= 0 && *arg_i < OGLPROJ_CHARACTERS_NUM)
            {
                ref->set_meshes(m_character_meshes[*arg_i]);
                ref->set_texture(m_textures[*arg_i + 2]);
            }
            else
            {
                std::cout << "[ERROR] Range of model_id is 0 - " << OGLPROJ_CHARACTERS_NUM - 1 << std::endl;
            }
            delete arg_i;
        }
        else if (token == "getmodelinfo")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            ref->get_meshes()->at(ref->get_animation_state())->print_info();
        }
        else if (token == "setwalkingspeed")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            arg_f = new float { 1 };
            input_stream >> *arg_f;
            ref->set_walking_speed(*arg_f);
            delete arg_f;
        }
        else if (token == "getwalkingspeed")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            std::cout << ref->get_walking_speed() << std::endl;
        }
        else if (token == "setposition")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            arg_f = new float[3] { 0 };
            input_stream >> arg_f[0] >> arg_f[1] >> arg_f[2];
            ref->set_position(glm::vec3(arg_f[0], arg_f[1], arg_f[2]));
            delete[] arg_f;
        }
        else if (token == "getposition")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            glm::vec3 pos = ref->get_position();
            std::cout << "(" << pos.x << ", " << pos.y << ", "
                      << pos.z << ")" << std::endl;
        }

        else if (token == "setangle")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            arg_f = new float { 1 };
            input_stream >> *arg_f;
            ref->set_angle(*arg_f);
            delete arg_f;
        }
        else if (token == "getangle")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            std::cout << ref->get_angle() << std::endl;
        }

        else if (token == "getposition")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            glm::vec3 pos = ref->get_position();
            std::cout << "(" << pos.x << ", " << pos.y << ", "
                      << pos.z << ")" << std::endl;
        }
        else if (token == "setanimationstate")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            arg_i = new int { 0 };
            input_stream >> *arg_i;
            if (*arg_i < 0 || *arg_i >= (int)ref->get_meshes()->size())
            {
                std::cout << "[ERROR] Argument must be within range 0 - " << ref->get_meshes()->size() << std::endl;
                continue;
            }
            ref->set_animation_state(*arg_i);
            delete arg_i;
        }
        else if (token == "getanimationstate")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            Character* ref;
            if (m_characters.find(*arg_s) != m_characters.end())
                ref = m_characters[*arg_s];
            else
            {
                std::cout << "[ERROR] Name not found." << std::endl;
                delete arg_s;
                continue;
            }
            delete arg_s;
            std::cout << ref->get_animation_state() << std::endl;
        }
        else if (token == "takecontrol")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            if (m_characters.find(*arg_s) != m_characters.end())
            {
                Character* ref = m_characters[*arg_s];
                Character* temp = m_characters["player"];
                m_characters["player"] = ref;
                m_characters[*arg_s] = temp;
                m_nameplates["player"]->set_character(ref);
                m_nameplates[*arg_s]->set_character(temp);
                m_camera->set_yaw_angle(m_characters["player"]->get_angle() - (float)M_PI);
            }
            else
                std::cout << "[ERROR] Name not found." << std::endl;
            delete arg_s;
        }
        else if (token == "getcamerainfo")
        {
            std::cout << "Camera-to-target yaw angle: " << m_camera->get_yaw_angle() << std::endl;
            std::cout << "Camera-to-target pitch angle: " << m_camera->get_pitch_angle() << std::endl;
            std::cout << "Camera distance to target: " << m_camera->get_radius() << std::endl;
            std::cout << "Camera target: (" << m_camera->get_target().x << ", "
                      << m_camera->get_target().y << ", " << m_camera->get_target().z << ")"
                      << std::endl;
            std::cout << "Camera FoV: " << m_camera->get_field_of_view_y() << std::endl;
            std::cout << "Camera aspect ratio: " << m_camera->get_aspect_ratio() << std::endl;
            std::cout << "Camera near-z distance: " << m_camera->get_near_z_distance() << std::endl;
            std::cout << "Camera far-z distance: " << m_camera->get_far_z_distance() << std::endl;
            std::cout << "Camera position: (" << m_camera->get_position().x << ", "
                      << m_camera->get_position().y << ", " << m_camera->get_position().z << ")"
                      << std::endl;
            std::cout << "Camera orientation: (" << m_camera->get_orientation().x << ", "
                      << m_camera->get_orientation().y << ", " << m_camera->get_orientation().z << ", "
                      << m_camera->get_orientation().w << ")"
                      << std::endl;
            std::cout << "Camera up direction: (" << m_camera->get_up_direction().x << ", "
                      << m_camera->get_up_direction().y << ", " << m_camera->get_up_direction().z << ")"
                      << std::endl;
            std::cout << "Camera left direction: (" << m_camera->get_left_direction().x << ", "
                      << m_camera->get_left_direction().y << ", " << m_camera->get_left_direction().z << ")"
                      << std::endl;
            std::cout << "Camera forward direction: (" << m_camera->get_forward_direction().x << ", "
                      << m_camera->get_forward_direction().y << ", " << m_camera->get_forward_direction().z << ")"
                      << std::endl;
        }
        else if (token == "setcamerapos")
        {
            arg_f = new float[3] { 0 };
            input_stream >> arg_f[0] >> arg_f[1] >> arg_f[2];
            m_camera->set_position(arg_f[0], arg_f[1], arg_f[2]);
            delete[] arg_f;
        }
        else if (token == "setcameratarget")
        {
            arg_f = new float[3] { 0 };
            input_stream >> arg_f[0] >> arg_f[1] >> arg_f[2];
            m_camera->set_target(glm::vec3(arg_f[0], arg_f[1], arg_f[2]));
            delete[] arg_f;
        }
        else if (token == "setcameraradius")
        {
            arg_f = new float { 0 };
            input_stream >> arg_f[0];
            m_camera->set_radius(arg_f[0]);
            delete arg_f;
        }
        else if (token == "getlightinfo")
        {
            glm::vec4 position = m_light->position;
            glm::vec3 intensity = m_light->intensity;
            std::cout << "Light position: (" << m_light->position.x << ", "
                      << m_light->position.y << ", " << m_light->position.z << ")"
                      << std::endl;
            std::cout << "Light intensity: " << std::endl;
            std::cout << "R: " << intensity.x << " G: " << intensity.y << " B: " << intensity.z << std::endl;
        }
        else if (token == "setlightpos")
        {
            arg_f = new float[3] { 0 };
            input_stream >> arg_f[0] >> arg_f[1] >> arg_f[2];
            m_light->position = glm::vec4(arg_f[0], arg_f[1], arg_f[2], 1);
            delete[] arg_f;
        }
        else if (token == "setlightintensity")
        {
            arg_f = new float[3] { 0 };
            input_stream >> arg_f[0] >> arg_f[1] >> arg_f[2];
            m_light->intensity = glm::vec3(arg_f[0], arg_f[1], arg_f[2]);
            delete[] arg_f;
        }
        else if (token == "setfpslimit")
        {
            arg_i = new int;
            input_stream >> *arg_i;
            g_fps_limit = *arg_i;
            delete arg_i;
        }
        else if (token == "sleep")
        {
            arg_d = new double;
            input_stream >> *arg_d;
            std::this_thread::sleep_for(std::chrono::nanoseconds(long long(*arg_d * 1e9)));
            delete arg_d;
        }
        else if (token == "loadscript")
        {
            arg_s = new std::string;
            input_stream >> *arg_s;
            std::ifstream input_file(*arg_s);
            input_stream.clear();
            input_stream << input_file.rdbuf();
            input_file.close();
            load_file = true;
            delete arg_s;
        }
        else if (token == "test")
        {
            arg_f = new float[3] { 0 };
            input_stream >> arg_f[0] >> arg_f[1] >> arg_f[2];
            glm::vec3 pos(arg_f[0], arg_f[1], arg_f[2]);
            glm::vec4 project = glm::vec4(pos.x, pos.y, pos.z, 1);
            project = get_camera()->get_projection_matrix() * get_camera()->get_view_matrix() * project;
            std::cout << project.x / project.w << ", " << project.y / project.w << ", " << project.z / project.w << std::endl;
            delete[] arg_f;
        }
        else
        {
            std::cout << "Unrecognized command: " << token << std::endl;
        }
    }
}

Third_person_camera* Main_scene::get_camera()
{
    return m_camera;
}

Character* Main_scene::get_player()
{
    return m_characters["player"];
}

std::map<std::string, Character*> Main_scene::get_characters()
{
    return m_characters;
}

Light* Main_scene::get_light()
{
    return m_light;
}

} // end of oglproj namespace

// main_scene.h
//
// Copyright (c) 2014 Samuel I. Gunadi <samuel.i.gunadi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OGLPROJ_MAIN_SCENE_H_
#define OGLPROJ_MAIN_SCENE_H_

#include <gl/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <thread>

#include "scene.h"
#include "texture.h"
#include "third_person_camera.h"
#include "main_event_handler.h"
#include "character.h"
#include "floor.h"
#include "nameplate.h"

namespace oglproj
{

// A scene with a walking man and a floor.

class Main_scene : public Scene
{
public:
    Main_scene(GLFWwindow* m_window);
    ~Main_scene();

    virtual void init_scene() override;
    virtual void update(double t) override;
    virtual void render() override;
    virtual Third_person_camera* get_camera();
    Character* get_player();
    std::map<std::string, Character*> get_characters();
    virtual Light* get_light();
private:
    Light* m_light;
    // Main camera
    Third_person_camera* m_camera;
    // All characters
    std::map<std::string, Character*> m_characters;
    // All nameplates
    std::map<std::string, Nameplate*> m_nameplates;
    // Floor
    Floor* m_floor;
    // Each representing an animation state.
    std::vector<std::vector<Mesh*>*> m_character_meshes;
    // Textures
    std::vector<Texture*> m_textures;
    // input processor, running in another thread
    std::thread* console_thread;
    void process_console_input();
};

} // end of namespace oglproj

#endif
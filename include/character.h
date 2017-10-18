// player.h

#ifndef OGLPROJ_PLAYER_H_
#define OGLPROJ_PLAYER_H_

#include <vector>
#include "shader_program.h"
#include "renderable.h"
#include "scene.h"
#include "mesh.h"
#include "texture.h"

namespace oglproj
{

class Character : public Renderable
{
public:
    enum class State : char
    {
        IDLE, WALK_FORWARD, WALK_BACKWARD, TURN_LEFT, TURN_RIGHT
    };
    Character(Scene* parent, float angle, glm::vec3 position, std::vector<Mesh*>* meshes, Texture* tex, int start_walking_cycle_state);
    virtual ~Character();
    void compile_link_shader();
    virtual void render();
    void set_state(State d);
    State get_state();
    void update(double t);
    void set_animation_state(int state);
    int get_animation_state();
    void set_walking_speed(float speed);
    float get_walking_speed();
    void set_position(glm::vec3 position);
    glm::vec3 get_position();
    void set_angle(float angle);
    float get_angle();
    Scene* get_parent();
    std::vector<Mesh*>* get_meshes();
    void set_meshes(std::vector<Mesh*>* meshes);
    void set_texture(Texture* tex);
protected:
    Scene* m_parent;
    // The player animation state.
    // 0 is standing / idle.
    // 1 to m_start_walking_cycle_state - 1 are start walking.
    // m_start_walking_cycle_state to m_end_walking_cycle_state are walking cycle.
    unsigned int m_animation_state;
    unsigned int m_start_walking_cycle_state; // 51
    unsigned int m_end_walking_cycle_state; // 86
    // The walking direction of the player.
    State m_state;
    // The player position.
    glm::vec3 m_position;
    // The angle in radians (x-y axis) that the player is facing.
    float m_angle;
    // The the player walking speed in meter per second.
    float m_walking_speed;
    // Model matrix.
    glm::mat4 m_model_matrix;
    glm::mat4 m_model_view_matrix;
    glm::mat3 m_normal_matrix;
    // GLSL program for the player.
    Shader_program* m_program;
    // Texture
    Texture* m_tex;
    bool m_program_linked;
    std::vector<Mesh*>* m_meshes;
private:
    // animation related
    double m_last_animation_update;
    double m_last_update;
    glm::vec3 m_delta;

};

} // end of oglproj namespace

#endif
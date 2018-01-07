#ifndef _GAME__H_
#define _GAME__H_

#include <memory.hpp>
#include <inttypes.h>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

#include <game_platform.hpp>

// Introspection macro
#define INTROSPECT(params)

#define OFFSET(type, prop) \
    ((u64)&(((type *)0)->prop))

#define Kilobytes(value) (((uint64_t)value)*1024)
#define Megabytes(value) (Kilobytes((uint64_t)value)*1024)
#define Gigabytes(value) (Megabytes((uint64_t)value)*1024)

#define MAX_ENTITIES 65536
#define MAX_MATERIALS 65536
#define MAX_ANIMATIONS 65536
#define MAX_FRAMES 65536

typedef char byte;

typedef u64 flags_t;
typedef u16 eid;
typedef u16 mat_id;
typedef u16 anim_id;


INTROSPECT()
struct material {
    char *name;
    u32 shader_program;
    u32 texture;
    char *vert_filename;
    char *frag_filename;
};

INTROSPECT()
struct frame {
    u32 index;
    f32 time;
};

INTROSPECT()
struct animation {
    frame *frames;
    u32 num_frames;
    u32 curr_frame;
};

typedef std::unordered_map<std::string, animation*> animation_dict;

INTROSPECT()
struct animator_t {
    u32 current_frame;
    u16 x_divisions;
    u16 y_divisions;
    mat_id mat;
    animation *current_animation;
    // TODO(Marce): Potser buscar alguna altre forma de guardar aixo
    // en el futur
    animation_dict animations;
};

INTROSPECT()
struct entity {
    eid id;
    u16 priority;
    glm::vec3 position;
    glm::mat4 transform;
    animator_t animator;
    flags_t flags;
    mat_id mat;
};

INTROSPECT()
struct camera_t {
    glm::mat4 view;
    glm::mat4 projection;
};

INTROSPECT()
struct world_t {
    entity entities[MAX_ENTITIES];
    eid num_entities;
    bool entities_dirty;

    camera_t camera;
};

struct game_state {
    world_t world;
    memory_arena world_arena;

    material materials[MAX_MATERIALS];
    mat_id num_materials;

    animation animations[MAX_ANIMATIONS];
    u16 num_animations;

    platform_code *platform;
    
    bool is_initialized;
    bool update_paused;
    bool stepping;

    bool open_inspector;
    bool open_entity_list;
    bool open_material_list;

    GLuint vbo;
    GLuint vao;
    u32 ebo;

    u32 texture;

    u32 shader_program;
};

// NOTE(Marce): We define this macros to have consistency between the
// typedefs and the function definitions in game.cpp
#define GAME_UPDATE_AND_RENDER_FUNC(name) void(name)(game_memory *memory, float dt)
typedef GAME_UPDATE_AND_RENDER_FUNC(game_update_func);

#define INITIALIZE_GAME_STATE_FUNC(name) void(name)(platform_code* platform, game_memory *memory)
typedef INITIALIZE_GAME_STATE_FUNC(initialize_game_state_func);

#endif

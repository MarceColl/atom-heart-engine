#ifndef _GAME__H_
#define _GAME__H_

#include <memory.hpp>
#include <inttypes.h>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <game_platform.hpp>

// Introspection macro
#define INTROSPECT(params)


#define Kilobytes(value) (((uint64_t)value)*1024)
#define Megabytes(value) (Kilobytes((uint64_t)value)*1024)
#define Gigabytes(value) (Megabytes((uint64_t)value)*1024)

#define MAX_ENTITIES 65536
#define MAX_MATERIALS 65536

typedef char byte;

typedef u64 flags_t;
typedef u16 eid;
typedef u16 mat_id;

INTROSPECT()
struct material {
    u32 shader_program;
    u32 texture;
    char *vert_filename;
    char *frag_filename;
};

INTROSPECT()
struct entity {
    eid id;
    u16 priority;
    glm::vec3 position;
    glm::mat4 transform;
    flags_t flags;
    mat_id mat;
};

INTROSPECT()
struct world_t {
    entity entities[MAX_ENTITIES];
    eid num_entities;
    bool entities_dirty;
};

struct game_state {
    world_t world;
    memory_arena world_arena;

    material materials[MAX_MATERIALS];
    mat_id num_materials;

    platform_code *platform;
    
    bool editor_mode;
    bool is_initialized;
    bool update_paused;
    bool stepping;

    GLuint vbo;
    GLuint vao;

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

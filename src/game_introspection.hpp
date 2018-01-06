
#ifndef _GAME_INTROSPECTION__H_
#define _GAME_INTROSPECTION__H_

#include <inttypes.h>
#include <game.hpp>

#define _GAME_GENERATED__H_
#include <game_generated.hpp>
#undef _GAME_GENERATED__H_

enum meta_types {
    // Basic types
    metatype_float,
    metatype_uint8_t,
    metatype_uint16_t,
    metatype_uint32_t,
    metatype_uint64_t,
    metatype_int8_t,
    metatype_int16_t,
    metatype_int32_t,
    metatype_int64_t,

    metatype_u8,
    metatype_u16,
    metatype_u32,
    metatype_u64,
    metatype_s8,
    metatype_s16,
    metatype_s32,
    metatype_s64,

    metatype_pchar, // string
    metatype_char,
    metatype_bool,
    // GLM
    metatype_glm__mat4,
    metatype_glm__vec2,
    metatype_glm__quaterion,
    metatype_glm__vec3,
    // Custom types
    metatype_eid,
    metatype_mat_id,
    metatype_flags_t,
    INTROSPECTION_ENUM_TYPE_HELPER
};

struct property_entry {
    meta_types type;
    char *name;
    uint64_t offset;
};

#include <game_generated.hpp>

#endif

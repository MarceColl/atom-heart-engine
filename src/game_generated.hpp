#ifndef _GAME_GENERATED__H_
#define _GAME_GENERATED__H_
// Do not modify! File generated using the INTROSPECTION macro
#include <game_introspection.hpp>

property_entry properties_of_material[] = {
        {metatype_u32,"shader_program",(uint64_t)&(((material *)0)->shader_program)},
        {metatype_pchar,"vert_filename",(uint64_t)&(((material *)0)->vert_filename)},
        {metatype_pchar,"frag_filename",(uint64_t)&(((material *)0)->frag_filename)},
};


property_entry properties_of_entity[] = {
        {metatype_eid,"id",(uint64_t)&(((entity *)0)->id)},
        {metatype_u16,"priority",(uint64_t)&(((entity *)0)->priority)},
        {metatype_glm__vec2,"position",(uint64_t)&(((entity *)0)->position)},
        {metatype_glm__mat4,"transform",(uint64_t)&(((entity *)0)->transform)},
        {metatype_flags_t,"flags",(uint64_t)&(((entity *)0)->flags)},
        {metatype_material,"mat",(uint64_t)&(((entity *)0)->mat)},
};


property_entry properties_of_world_t[] = {
        {metatype_entity,"entities",(uint64_t)&(((world_t *)0)->entities)},
        {metatype_eid,"num_entities",(uint64_t)&(((world_t *)0)->num_entities)},
};


#endif
#define INTROSPECTION_SWITCH_TYPE_HELPER\
    case metatype_world_t:\
    {\
        DEBUG_inspect_struct(2, properties_of_world_t, member_ptr, member->name);\
    } break; \
    case metatype_entity:\
    {\
        DEBUG_inspect_struct(6, properties_of_entity, member_ptr, member->name);\
    } break; \
    case metatype_material:\
    {\
        DEBUG_inspect_struct(3, properties_of_material, member_ptr, member->name);\
    } break; \

#define INTROSPECTION_ENUM_TYPE_HELPER\
    metatype_world_t,\
    metatype_entity,\
    metatype_material,\


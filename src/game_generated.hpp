#ifndef _GAME_GENERATED__H_
#define _GAME_GENERATED__H_
// Do not modify! File generated using the INTROSPECTION macro
#include <game_introspection.hpp>

property_entry properties_of_material[] = {
        {metatype_pchar,"name",(uint64_t)&(((material *)0)->name)},
        {metatype_u32,"shader_program",(uint64_t)&(((material *)0)->shader_program)},
        {metatype_u32,"texture",(uint64_t)&(((material *)0)->texture)},
        {metatype_pchar,"vert_filename",(uint64_t)&(((material *)0)->vert_filename)},
        {metatype_pchar,"frag_filename",(uint64_t)&(((material *)0)->frag_filename)},
};


property_entry properties_of_frame[] = {
        {metatype_u32,"index",(uint64_t)&(((frame *)0)->index)},
        {metatype_f32,"time",(uint64_t)&(((frame *)0)->time)},
};


property_entry properties_of_animation[] = {
        {metatype_pframe,"frames",(uint64_t)&(((animation *)0)->frames)},
        {metatype_u32,"num_frames",(uint64_t)&(((animation *)0)->num_frames)},
        {metatype_u32,"curr_frame",(uint64_t)&(((animation *)0)->curr_frame)},
};


property_entry properties_of_animator_t[] = {
        {metatype_u32,"current_frame",(uint64_t)&(((animator_t *)0)->current_frame)},
        {metatype_u16,"x_divisions",(uint64_t)&(((animator_t *)0)->x_divisions)},
        {metatype_u16,"y_divisions",(uint64_t)&(((animator_t *)0)->y_divisions)},
        {metatype_mat_id,"mat",(uint64_t)&(((animator_t *)0)->mat)},
        {metatype_panimation,"current_animation",(uint64_t)&(((animator_t *)0)->current_animation)},
        {metatype_animation_dict,"animations",(uint64_t)&(((animator_t *)0)->animations)},
};


property_entry properties_of_entity[] = {
        {metatype_eid,"id",(uint64_t)&(((entity *)0)->id)},
        {metatype_u16,"priority",(uint64_t)&(((entity *)0)->priority)},
        {metatype_glm__vec3,"position",(uint64_t)&(((entity *)0)->position)},
        {metatype_glm__mat4,"transform",(uint64_t)&(((entity *)0)->transform)},
        {metatype_animator_t,"animator",(uint64_t)&(((entity *)0)->animator)},
        {metatype_flags_t,"flags",(uint64_t)&(((entity *)0)->flags)},
        {metatype_mat_id,"mat",(uint64_t)&(((entity *)0)->mat)},
};


property_entry properties_of_camera_t[] = {
        {metatype_glm__mat4,"view",(uint64_t)&(((camera_t *)0)->view)},
        {metatype_glm__mat4,"projection",(uint64_t)&(((camera_t *)0)->projection)},
};


property_entry properties_of_world_t[] = {
        {metatype_entity,"entities",(uint64_t)&(((world_t *)0)->entities)},
        {metatype_eid,"num_entities",(uint64_t)&(((world_t *)0)->num_entities)},
        {metatype_bool,"entities_dirty",(uint64_t)&(((world_t *)0)->entities_dirty)},
        {metatype_camera_t,"camera",(uint64_t)&(((world_t *)0)->camera)},
};


#endif
#define INTROSPECTION_SWITCH_TYPE_HELPER\
    case metatype_world_t:\
    {\
        DEBUG_inspect_struct_header(state, 4, properties_of_world_t, member_ptr, member->name);\
    } break; \
    case metatype_camera_t:\
    {\
        DEBUG_inspect_struct_header(state, 2, properties_of_camera_t, member_ptr, member->name);\
    } break; \
    case metatype_entity:\
    {\
        DEBUG_inspect_struct_header(state, 7, properties_of_entity, member_ptr, member->name);\
    } break; \
    case metatype_animator_t:\
    {\
        DEBUG_inspect_struct_header(state, 6, properties_of_animator_t, member_ptr, member->name);\
    } break; \
    case metatype_animation:\
    {\
        DEBUG_inspect_struct_header(state, 3, properties_of_animation, member_ptr, member->name);\
    } break; \
    case metatype_frame:\
    {\
        DEBUG_inspect_struct_header(state, 2, properties_of_frame, member_ptr, member->name);\
    } break; \
    case metatype_material:\
    {\
        DEBUG_inspect_struct_header(state, 5, properties_of_material, member_ptr, member->name);\
    } break; \

#define INTROSPECTION_ENUM_TYPE_HELPER\
    metatype_world_t,\
    metatype_pworld_t,\
    metatype_camera_t,\
    metatype_pcamera_t,\
    metatype_entity,\
    metatype_pentity,\
    metatype_animator_t,\
    metatype_panimator_t,\
    metatype_animation,\
    metatype_panimation,\
    metatype_frame,\
    metatype_pframe,\
    metatype_material,\
    metatype_pmaterial,\

#define DEBUG_world_t(entity)\
    DEBUG_inspect_struct(state, 4, properties_of_world_t, entity, #entity)\

#define DEBUG_camera_t(entity)\
    DEBUG_inspect_struct(state, 2, properties_of_camera_t, entity, #entity)\

#define DEBUG_entity(entity)\
    DEBUG_inspect_struct(state, 7, properties_of_entity, entity, #entity)\

#define DEBUG_animator_t(entity)\
    DEBUG_inspect_struct(state, 6, properties_of_animator_t, entity, #entity)\

#define DEBUG_animation(entity)\
    DEBUG_inspect_struct(state, 3, properties_of_animation, entity, #entity)\

#define DEBUG_frame(entity)\
    DEBUG_inspect_struct(state, 2, properties_of_frame, entity, #entity)\

#define DEBUG_material(entity)\
    DEBUG_inspect_struct(state, 5, properties_of_material, entity, #entity)\


#define DEBUG_world_t_header(entity, title)\
    DEBUG_inspect_struct_header(state, 4, properties_of_world_t, entity, title)\

#define DEBUG_camera_t_header(entity, title)\
    DEBUG_inspect_struct_header(state, 2, properties_of_camera_t, entity, title)\

#define DEBUG_entity_header(entity, title)\
    DEBUG_inspect_struct_header(state, 7, properties_of_entity, entity, title)\

#define DEBUG_animator_t_header(entity, title)\
    DEBUG_inspect_struct_header(state, 6, properties_of_animator_t, entity, title)\

#define DEBUG_animation_header(entity, title)\
    DEBUG_inspect_struct_header(state, 3, properties_of_animation, entity, title)\

#define DEBUG_frame_header(entity, title)\
    DEBUG_inspect_struct_header(state, 2, properties_of_frame, entity, title)\

#define DEBUG_material_header(entity, title)\
    DEBUG_inspect_struct_header(state, 5, properties_of_material, entity, title)\



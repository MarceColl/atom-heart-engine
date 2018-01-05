#ifndef _GAME_GENERATED__H_
#define _GAME_GENERATED__H_
// Do not modify! File generated using the INTROSPECTION macro
#include <game_introspection.hpp>

property_entry properties_of_entity[] = {
        {metatype_eid,"id",(uint64_t)&(((entity *)0)->id)},
};


property_entry properties_of_test_struct[] = {
        {metatype_float,"s",(uint64_t)&(((test_struct *)0)->s)},
        {metatype_float,"t",(uint64_t)&(((test_struct *)0)->t)},
        {metatype_float,"m",(uint64_t)&(((test_struct *)0)->m)},
};


property_entry properties_of_test2_struct[] = {
        {metatype_float,"a",(uint64_t)&(((test2_struct *)0)->a)},
        {metatype_float,"b",(uint64_t)&(((test2_struct *)0)->b)},
        {metatype_float,"c",(uint64_t)&(((test2_struct *)0)->c)},
};


property_entry properties_of_test_entity[] = {
        {metatype_float,"x",(uint64_t)&(((test_entity *)0)->x)},
        {metatype_float,"y",(uint64_t)&(((test_entity *)0)->y)},
        {metatype_bool,"is_initialized",(uint64_t)&(((test_entity *)0)->is_initialized)},
        {metatype_test_struct,"t",(uint64_t)&(((test_entity *)0)->t)},
        {metatype_test2_struct,"a",(uint64_t)&(((test_entity *)0)->a)},
};


#endif
#define INTROSPECTION_SWITCH_TYPE_HELPER\
    case metatype_test_entity:\
    {\
        DEBUG_inspect_struct(5, properties_of_test_entity, member_ptr, member->name);\
    } break; \
    case metatype_test2_struct:\
    {\
        DEBUG_inspect_struct(3, properties_of_test2_struct, member_ptr, member->name);\
    } break; \
    case metatype_test_struct:\
    {\
        DEBUG_inspect_struct(3, properties_of_test_struct, member_ptr, member->name);\
    } break; \
    case metatype_entity:\
    {\
        DEBUG_inspect_struct(1, properties_of_entity, member_ptr, member->name);\
    } break; \

#define INTROSPECTION_ENUM_TYPE_HELPER\
    metatype_test_entity,\
    metatype_test2_struct,\
    metatype_test_struct,\
    metatype_entity,\


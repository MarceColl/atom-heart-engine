#ifndef _GAME_INTROSPECTION__H_
#define _GAME_INTROSPECTION__H_

#include <inttypes.h>
#include <game.hpp>


enum meta_types {
	metatype_float,
	metatype_uint8_t,
};

struct property_entry {
	meta_types type;
	char *name;
	uint64_t offset;
};

property_entry properties_of_test_entity[] =
{ 
	{metatype_float,"x",(uint64_t)&((test_entity*)0)->x},
	{metatype_float,"y",(uint64_t)&((test_entity*)0)->y},
};

#endif

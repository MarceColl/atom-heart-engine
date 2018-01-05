#ifndef _GAME_INTROSPECTION__H_
#define _GAME_INTROSPECTION__H_

#include <inttypes.h>
#include <game.hpp>
#include <entity.hpp>


enum meta_types {
	metatype_float,
	metatype_uint8_t,
	metatype_eid,
};

struct property_entry {
	meta_types type;
	char *name;
	uint64_t offset;
};

#endif

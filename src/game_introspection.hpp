#ifndef _GAME_INTROSPECTION__H_
#define _GAME_INTROSPECTION__H_

#include <inttypes.h>
#include <game.hpp>
#include <entity.hpp>

#define _GAME_GENERATED__H_
#include <game_generated.hpp>
#undef _GAME_GENERATED__H_

enum meta_types {
	metatype_float,
	metatype_uint8_t,
	metatype_uint16_t,
	metatype_uint32_t,
	metatype_bool,
	metatype_eid,
	INTROSPECTION_ENUM_TYPE_HELPER
};

struct property_entry {
	meta_types type;
	char *name;
	uint64_t offset;
};

#include <game_generated.hpp>

#endif

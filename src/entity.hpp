#include <inttypes.h>

#include <game.hpp>

// Maximum number of entities in a scene at the same time. It's 2^16
// entities, so we can set the entity id to be a uint16_t
#define MAX_ENTITES 65536

typedef uint16_t eid;

// An Entity is an object of the world. It's a very cheap object to
// move around as it's only an id.
INTROSPECT()
struct entity {
	eid id;
};

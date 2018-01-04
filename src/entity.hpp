#include <inttypes.h>

#include <ah/world.hpp>

// Maximum number of entities in a scene at the same time. It's 2^16
// entities, so we can set the entity id to be a uint16_t
#define MAX_ENTITES 65536

typedef eid uint16_t;

// An Entity is an object of the world.
struct Entity {
	World *world;
	eid id;

	bool hasComponent<T>() {
		world->getComponentArray<T>(id);
	}
};

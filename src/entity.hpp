#include <inttypes.h>

#include <ah/world.hpp>


typedef eid uint16;

// An Entity is an object of the world.
struct Entity {
	World *world;
	eid id;

	bool hasComponent<T>() {
		world->getComponentArray<T>(id);
	}
};

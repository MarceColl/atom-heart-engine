#ifndef _GAME__H_
#define _GAME__H_

#include <inttypes.h>


#define Kilobytes(value) (((uint64_t)value)*1024)
#define Megabytes(value) (Kilobytes((uint64_t)value)*1024)
#define Gigabytes(value) (Megabytes((uint64_t)value)*1024)

// NOTE(Marce): We don't want any allocations, we want to reserve all
// the memory upfront and not have to reserve anything else. It's also
// good because of the live reload, the memory will be preserved.
struct game_memory {
	uint64_t permanent_storage_space;
	void *permanent_storage;

	uint64_t transient_storage_space;
	void *transient_storage;

	bool isInitialized;
};

// NOTE(Marce): We define this macros to have consistency between the
// typedefs and the function definitions in game.cpp
#define GAME_UPDATE_FUNC(name) void(name)(float dt)
typedef GAME_UPDATE_FUNC(game_update_func);
#define GAME_RENDER_FUNC(name) void(name)(void)
typedef GAME_RENDER_FUNC(game_render_func);

#endif

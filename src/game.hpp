#ifndef _GAME__H_
#define _GAME__H_

#include <inttypes.h>

#define INTROSPECT(params)

typedef char byte;

#define Kilobytes(value) (((uint64_t)value)*1024)
#define Megabytes(value) (Kilobytes((uint64_t)value)*1024)
#define Gigabytes(value) (Megabytes((uint64_t)value)*1024)

// This struct holds the memory space we use all across the
// engine. There are two memory spaces. *transient*, for working
// memory, and *permanent*, for long-lasting memory.
INTROSPECT(category:"tetesiko")
struct game_memory {
	// About: Allocation
	//
	// This engine has a strong no allocation
	// policy. There must be no other allocation except the initial memory
	// allocation for these two pointers.

	uint64_t permanent_storage_space;
	void *permanent_storage;

	uint64_t transient_storage_space;
	void *transient_storage;

	// Property: is_initialized
	//
	// Sometimes we want to know if the game_memory has been initialized.
	bool is_initialized;
};

// Struct: game_state
//
// Holds game state
struct game_state {
	uint64_t counter;
	uint64_t offset;

	bool is_initialized;
	bool update_paused;
};


// NOTE(Marce): We define this macros to have consistency between the
// typedefs and the function definitions in game.cpp
#define GAME_UPDATE_AND_RENDER_FUNC(name) void(name)(game_memory *memory, float dt)
typedef GAME_UPDATE_AND_RENDER_FUNC(game_update_func);

#define INITIALIZE_GAME_STATE_FUNC(name) void(name)(game_memory *memory)
typedef INITIALIZE_GAME_STATE_FUNC(initialize_game_state_func);

#endif

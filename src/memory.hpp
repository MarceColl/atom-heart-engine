#ifndef _MEMORY__H_
#define _MEMORY__H_

#include <game_math.hpp>


struct memory_arena {
    u64 size;
    u8 *base;
    u64 used;
};

struct game_memory {
    u64 permanent_storage_space;
    void *permanent_storage;

    u64 transient_storage_space;
    void *transient_storage;

    bool is_initialized;
};


void set_game_memory_ptr(game_memory *memory);

void initialize_arena(memory_arena *arena,
		      u64 size,
		      u8 *storage);

void *push_struct(memory_arena *arena,
		  u64 size,
		  void *data);

#endif

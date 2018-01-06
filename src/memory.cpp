#include <memory.hpp>

game_memory *global_game_memory;

void set_game_memory_ptr(game_memory *memory) {
    global_game_memory = memory;
}

void
initialize_arena(memory_arena *arena,
		 u64 size,
		 u8 *base) {
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

#define push_struct(arena, type) (type*)push_struct_(arena, sizeof(type))

void *push_struct_(memory_arena *arena,
		  u64 size) {
    void *result = arena->base + arena->used;
    arena->used += size;
    return result;
}

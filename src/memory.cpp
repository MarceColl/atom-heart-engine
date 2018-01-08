#include <memory.hpp>

game_memory *global_game_memory;

void set_game_memory_ptr(game_memory *memory) {
    global_game_memory = memory;
}

void
initialize_stack(memory_stack *stack,
		 u64 size,
		 u8 *base) {
    stack->size = size;
    stack->base = base;
    stack->used = 0;
}

#define push_struct(stack, type) (type*)push_struct_(stack, sizeof(type))
void *push_struct_(memory_stack *stack,
		   u64 size) {
    void *result = stack->base + stack->used;
    stack->used += size;
    return result;
}
